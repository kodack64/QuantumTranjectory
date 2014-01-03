
#pragma warning(disable : 4819)

#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "CommandFactory.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

#ifdef _OPENMP
#include <omp.h>
#endif

// 値の設定
void CommandSetParameter::execute(ParameterSet* par,queue<Command*>& coms){
	par->setParam(_paramName,_value);
	cout << ">> set " << _paramName << " -> " << _value << endl;
}

// 値の加減
void CommandDifSetParameter::execute(ParameterSet* par,queue<Command*>& coms){
	try{
		if(_type=="int"){
			int val = boost::lexical_cast<int>(par->getParamInt(_paramName,0));
			int dif = boost::lexical_cast<int>(_value);
			par->setParam(_paramName,boost::lexical_cast<string>(val+dif));
			cout << ">> difset " << _paramName << " " << val << " -> " << val+dif << endl;
		}else if(_type=="double"){
			double val = boost::lexical_cast<double>(par->getParamDouble(_paramName,0));
			double dif = boost::lexical_cast<double>(_value);
			par->setParam(_paramName,boost::lexical_cast<string>(val+dif));
			cout << ">> difset " << _paramName << " " << val << " -> " << val+dif << endl;
		}else{
			cout << ">> unknown type " << _type <<endl;
		}
	}catch(boost::bad_lexical_cast e){
		cout << ">> cannot cast param : " << _paramName << " or " << _value << endl;
	}
}

// 設定値の表示
void CommandDescParameter::execute(ParameterSet* par,queue<Command*>& coms){
	par->outputAllParameter(cout);
}

// パラメータファイルのロード
void CommandSetParameterFile::execute(ParameterSet* par,queue<Command*>& coms){
	ifstream ifs(_fileName,ios::in);
	if(!ifs){
		cout << ">> file not found" << endl;
	}
	while(ifs && !ifs.eof()){
		string line;
		vector<string> stringArray;
		getline(ifs,line);
		boost::algorithm::split(stringArray,line,boost::is_space());
		if(stringArray.size()<=1)continue;
		if(stringArray[0][0]=='#')continue;
		par->setParam(stringArray[0],stringArray[1]);
		cout << ">>set " << stringArray[0] << " " << stringArray[1] << " @" << _fileName << endl; 
	}
}

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


	// 実行時のパラメータをログに保存
	ofstream ofs;
	stringstream ss;
	ss << "log_" << i_unit << "_parameter.txt" << endl;
	ofs.open(ss.str(),ios::out);
	if(!ofs.bad()){
		par->outputAllParameter(ofs);
		ofs.close();
	}

	// 並列計算時はスレッドを分割し計算
#ifdef _OPENMP
	int count=0;
#pragma omp parallel
	{
		int myid;
		Simulator* sim = new Simulator();
		while(true){
#pragma omp atomic
			{
				if(count>=i_repeatNum)break;
				else{
					myid = count;
					count++;
				}
			}
			sim->execute(i_unit,myid,par);
		}
		delete sim;
	}

#else

	// 並列計算でない場合は普通に計算
	cout << ">> start simulator " << endl;
	Simulator* sim = new Simulator();
	for(int i=0;i<i_repeatNum;i++){
		cout << ">> " << i << " / " << i_repeatNum << " try" << endl;
		sim->execute(i_unit,i,par);
	}
	delete sim;
	cout << ">>finish" << endl;

#endif
}

// データから相関の計算
void CommandCalcG2::execute(ParameterSet* par,queue<Command*>& coms){
	int i_unit;
	try{
		i_unit = boost::lexical_cast<int>(par->getParamInt("unit",0));
		par->setParam("unit",boost::lexical_cast<string>(i_unit+1));
	}catch(boost::bad_lexical_cast e){
		i_unit=1;
	}

	ofstream ofs;
	stringstream ss;
	ss << "log_" << i_unit << "_parameter.txt" << endl;
	ofs.open(ss.str(),ios::out);
	if(!ofs.bad()){
		par->outputAllParameter(ofs);
		ofs.close();
	}
	cout << ">> start simulator " << endl;
	Simulator* sim = new Simulator();
	cout << ">> base try" << endl;
	sim->execute(i_unit,0,par);
	delete sim;
	cout << ">>finish" << endl;
}

// コマンドファイルのロード
void CommandScript::execute(ParameterSet* par,queue<Command*>& coms){
	int i_repeatNum;
	try{
		i_repeatNum = boost::lexical_cast<int>(_repeatNum);
	}catch(boost::bad_lexical_cast e){
		i_repeatNum=1;
	}
	ifstream ifs(_scriptFile,ios::in);
	if(!ifs || ifs.eof()){
		cout << ">>file not found" << endl;
		return;
	}
	ifs.close();
	for(int i=0;i<i_repeatNum;i++){
		ifs.open(_scriptFile,ios::in);
		string line;
		while(ifs && !ifs.eof()){
			getline(ifs,line);
			if(line.length()>0){
				Command* com =CommandFactory::createCommand(line);
				if(com!=NULL){
					cout << ">> add queue : " << line << endl;
					coms.push(com);
				}
			}
		}
		ifs.close();
	}
}

// ヒストグラムの生成
void CommandCalcHistgram::execute(ParameterSet* par,queue<Command*>& coms){
}

void CommandCalcPower::execute(ParameterSet* par,queue<Command*>& coms){
}
