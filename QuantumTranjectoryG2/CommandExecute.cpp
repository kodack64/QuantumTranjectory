
#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

#ifdef _OPENMP
#include <omp.h>
#endif

// 計算の実行
void CommandExecute::execute(ParameterSet* par,queue<Command*>& coms){

	// 試行回数と実行IDの取得
	int i_repeatNum;
	try{
		i_repeatNum = boost::lexical_cast<int>(_executeNum);
	}catch(boost::bad_lexical_cast e){
		i_repeatNum=1;
	}
	int i_unit;
	try{
		i_unit = boost::lexical_cast<int>(par->getParamInt("unit",0));
//		par->setParam("unit",boost::lexical_cast<string>(i_unit+1));
	}catch(boost::bad_lexical_cast e){
		i_unit=0;
	}
	int core;
	try{
		core = boost::lexical_cast<int>(par->getParamInt("core",0));
	}catch(boost::bad_lexical_cast e){
		core=8;
	}


	// 実行時のパラメータをログに保存
	ofstream ofs;
	stringstream ss;
	ss << "data\\log_" << i_unit << "_parameter.txt";
	ofs.open(ss.str(),ios::out);
	if(!ofs.bad()){
		ofs << par->toString();
		ofs.close();
	}

	// 並列計算時はスレッドを分割し計算
#ifdef _OPENMP
	int count=0;
	int totLossPG=0;
	int totLossPF=0;
	int totLossAE=0;
	double totMaxEdge=0;
	cout << "# start simulator " << endl;
	omp_set_num_threads(core);
#pragma omp parallel
	{
		int mycore = omp_get_thread_num();
		int myid;
		int lossPG=0;
		int lossPF=0;
		int lossAE=0;
		double maxEdge=0;
		Simulator* sim = new Simulator();
		while(true){
#pragma omp critical
			{
				if(count<i_repeatNum){
					myid = count;
					count++;
					cout << " #" << mycore << " start " << myid  << " / " << i_repeatNum << " try ave(pg,pf,ae)=" << totLossPG*1.0/count << "," << totLossPF*1.0/count << "," <<totLossAE*1.0/count << " edge=" << totMaxEdge << endl;
					totLossPG+=lossPG;
					totLossPF+=lossPF;
					totLossAE+=lossAE;
					if(totMaxEdge<maxEdge)totMaxEdge=maxEdge;
				}else{
					myid=-1;
				}
			}
			if(myid==-1)break;
			sim->execute(i_unit,myid,par);
			lossPG = sim->getTotalProbeLossCount();
			lossPF = sim->getTotalControlLossCount();
			lossAE = sim->getTotalAtomLossCount();
			maxEdge = sim->getMaxEdge();
			cout << " #" << mycore << " result * pg" << lossPG << " pf" << lossPF << " ae" << lossAE << " edge" << maxEdge <<  endl;
		}
#pragma omp critical
		cout << " #" << mycore << "end" << endl;
		delete sim;
	}

#else

	// 並列計算でない場合は普通に計算
	cout << "# start simulator " << endl;
	Simulator* sim = new Simulator();
	for(int i=0;i<i_repeatNum;i++){
		cout << ">> " << i << " / " << i_repeatNum << " try" << endl;
		sim->execute(i_unit,i,par);
		cout << " #" << mycore << " result * " << lastCount << endl;
	}
	delete sim;
	cout << "# finish" << endl;

#endif
}

// 相関の計算
void CommandCalcG2::execute(ParameterSet* par,queue<Command*>& coms){
	int i_unit;
	try{
		i_unit = boost::lexical_cast<int>(par->getParamInt("unit",0));
//		par->setParam("unit",boost::lexical_cast<string>(i_unit+1));
	}catch(boost::bad_lexical_cast e){
		i_unit=1;
	}

	ofstream ofs;
	stringstream ss;
	ss << "data\\log_" << i_unit << "_parameter.txt";
	ofs.open(ss.str(),ios::out);
	if(!ofs.bad()){
		ofs << par->toString();
		ofs.close();
	}

	par->setParam("postselProbe","true");
	par->setParam("postselControl","true");
	par->setParam("postselAtom","true");
	par->setParam("loggingTime","false");
	par->setParam("loggingProb","true");
	par->setParam("loggingJump","false");

	int jumpStep = par->getParamInt("forceLossProbeTime",-1);
	double logDiv = par->getParamDouble("dt",0) * par->getParamInt("loggingUnit",1);

	if(jumpStep<=0){
		cout << "!!!jump step is undefined or disabled" << endl;
		return;
	}

	cout << "#  start simulator " << endl;
	Simulator* sim = new Simulator();

	cout << "#  jump try" << endl;
	sim->execute(i_unit,1,par);

	cout << "#  base try" << endl;
	par->setParam("forceLossProbeTime","-1");
	sim->execute(i_unit,0,par);
	par->setParam("forceLossProbeTime",boost::lexical_cast<string>(jumpStep));

	stringstream ss1,ss2,ss3;
	ss1 << "data\\log_" << i_unit << "_" << 0 << "_prob_atom.txt";
	ss2 << "data\\log_" << i_unit << "_" << 1 << "_prob_atom.txt";
	ss3 << "analyze\\log_" << i_unit << "_d_prob_atom.txt";
	calcG2(ss1.str(),ss2.str(),ss3.str(),jumpStep,logDiv);
	ss1.str("");ss2.str("");ss3.str("");
	ss1 << "data\\log_" << i_unit << "_" << 0 << "_prob_probe.txt";
	ss2 << "data\\log_" << i_unit << "_" << 1 << "_prob_probe.txt";
	ss3 << "analyze\\log_" << i_unit << "_d_prob_probe.txt";
	calcG2(ss1.str(),ss2.str(),ss3.str(),jumpStep,logDiv);
	ss1.str("");ss2.str("");ss3.str("");
	ss1 << "data\\log_" << i_unit << "_" << 0 << "_prob_control.txt";
	ss2 << "data\\log_" << i_unit << "_" << 1 << "_prob_control.txt";
	ss3 << "analyze\\log_" << i_unit << "_d_prob_control.txt";
	calcG2(ss1.str(),ss2.str(),ss3.str(),jumpStep,logDiv);

	cout << "# finish" << endl;
	delete sim;
}
void CommandCalcG2::calcG2(string base,string jump,string out,int jumpStep , double logDiv){
	ifstream ibase,ijump;
	ofstream og2;
	vector<double> pbase;
	vector<double> pjump;
	double div=0;
	ibase.open(base);
	ijump.open(jump);
	while(ibase){
		double time,val;
		ibase >> time >> val;
		if(time!=0 && div==0)div=time;
		pbase.push_back(val);
	}
	while(ijump){
		double time,val;
		ijump >> time >> val;
		pjump.push_back(val);
	}
	og2.open(out,ios::out);
	for(unsigned int s=jumpStep+1 ; s<pbase.size() && s<pjump.size();s++){
		og2 << (s-jumpStep)*logDiv << " " << pjump[s]/pbase[s] << endl;
	}
	og2.close();
}