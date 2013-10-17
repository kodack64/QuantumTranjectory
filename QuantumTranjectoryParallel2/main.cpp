
#include <fstream>
#include <iostream>
#include <sstream>
#include <complex>
#include <chrono>
#include <cmath>
#include <random>
#include <map>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <omp.h>
#include <conio.h>

using namespace std;
using namespace boost;

#define DEBUG_OUT

class Random{
public:
	std::random_device rd;
	std::mt19937_64 mt;
	std::uniform_real_distribution<double> dis;
	Random(int s){
		mt=std::mt19937_64(rd());
		dis=std::uniform_real_distribution<double>(0.0,1.0);
	}
	virtual double next(){
		return dis(mt);
	}
};

class Simulator{
private:
	Random* r;
	int seed;
	double dt;
	complex<double>* state;
	complex<double>* dif;
	double* ene;
	int totAtom;
	int maxEne;
	int maxPG;
	int maxPF;
	int maxAE;
	int maxAF;
	int indPG;
	int indPF;
	int indAE;
	int indAF;
	int vecSize;
	int totalLossPG;
	vector <double> lossTime;

	double pump;
	double cohg;
	double cohf;
	double lossPG;
	double lossPF;
	double life;

	double detune;
	double width;

	double eps;

	bool usePulse;
	bool useLossPG;
	bool useLossAE;
	bool useLossPF;
	bool useLossAF;
	int debug;

	int div;
	long maxstep;
	complex<double> img;

	vector<pair<string,string> > restarg;

	virtual int getIdToPG(int i){return (i%indAE)/indPG;}
	virtual int getIdToAE(int i){return (i%indPF)/indAE;}
	virtual int getIdToPF(int i){return (i%indAF)/indPF;}
	virtual int getIdToAF(int i){return (i%vecSize)/indAF;}

public:
	Simulator()
	:r(__nullptr)
	,state(__nullptr)
	,dif(__nullptr)
	,ene(__nullptr)
	{
	}
	virtual int init(vector<pair<string,string> > arg){
		// state limit
		maxPG=4;
		maxAE=4;
		maxPF=0;
		maxAF=0;
		totAtom=10;

		// use flag
		useLossPG=true;
		useLossAE=true;
		useLossPF=true;
		useLossAF=true;
		usePulse=true;

		//	time
		dt=1e-4;
		div=1000;
		maxstep=100000000;

		//	input
		pump=1.5e-1;
		detune=0e-1;
		width=1e1;

		//	coherence
		cohg=1e-1;
		cohf=0e-0;

		//	lifetime
		lossPG=3e-1;
		lossPF=3e-1;
		life=6e-3;

		// end condition
		eps=1e-3;
		seed=(int)time(NULL);

		// setup
		auto ite = arg.begin();
		while(ite!=arg.end()){
			if((*ite).first=="maxPG"){maxPG=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="maxAE"){maxAE=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="maxPF"){maxPF=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="maxAF"){maxAF=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="totAtom"){totAtom=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="dt"){dt=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="div"){div=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="maxstep"){maxstep=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="pump"){pump=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="detune"){detune=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="width"){width=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="cohg"){cohg=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="cohf"){cohf=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="cohg"){cohg=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="lossPG"){lossPG=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="lossPF"){lossPF=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="life"){life=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="eps"){eps=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
			else if((*ite).first=="debug"){debug=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
			else {restarg.push_back(*ite); ite++;}
		}

		// no change
		indPG=1;
		indAE=indPG*(maxPG+1);
		indPF=indAE*(maxAE+1);
		indAF=indPF*(maxPF+1);
		vecSize=indAF*(maxAF+1);
		maxEne=maxPG+maxAE+maxPF;
		img=complex<double>(0,1);

		// memory allocate
		r=new Random(seed);
		state= new complex<double>[vecSize];
		dif= new complex<double>[vecSize];
		ene = new double[maxEne+1];

		// setting
		cout.precision(3);

		return 0;
	}
	virtual void close(){
		delete r;
		delete[] state;
		delete[] dif;
		delete[] ene;
	}
	virtual int run(){

#ifdef DEBUG_OUT

		fstream fcount;
		stringstream datafile;
		fstream fout;

#pragma omp critical
		{
			if(debug){
				int cc;
				fcount.open("count.txt",ios::in);
				if(!fcount){
					fcount.open("count.txt",ios::out);
					fcount << 0 << endl;
					fcount.close();
					cc=0;
				}else{
					fcount >> cc;
					fcount.close();
				}

				datafile << "./data/data" << cc << ".txt";

				fcount.open("count.txt",ios::out);
				fcount << cc+1;
				fcount.close();

				fout.open(datafile.str(),ios::out);
				fout.precision(8);
				fout << "# 1-time(micro sec)  2-Energy 3-PhotonG 4-AtomE 5-PhotonF 6-AtomF 7-NonZeroProb 8-DetectProb 9-PhotonLoss"  << endl;
			}
		}
#endif DEBUG_OUT
		
		// temporal variables
		int pg,ae,pf,af;
		double poslosg;
		complex<double> rot;
		double pulse;
		int i;
		long step;
		bool flaghit;
		double val;

		// initialize
		for(int i=0;i<vecSize;i++)state[i]=0;
		state[0]=1;
		totalLossPG=0;
		step=-1;
		poslosg=0;
		lossTime.clear();

		double sum;
		double totPG;
		double totAE;
		double totPF;
		double totAF;
		double totEN;
		double trace;

		while(step<maxstep){

			step++;
			rot = complex<double>(cos(step*dt*detune),sin(step*dt*detune));
			if(usePulse){
				if(dt*step<width){
					pulse=(-cos(dt*step/width*acos(0)*4)+1)/2;
				}else{
					pulse=0;
				}
			}else{
				pulse=1;
			}

			for(i=0;i<vecSize;i++)dif[i]=0;
			for(i=0;i<vecSize;i++){
				pg = getIdToPG(i);
				ae = getIdToAE(i);
				pf = getIdToPF(i);
				af = getIdToAF(i);

				// pump
				if(pg<maxPG){
					dif[i]+=img*pump*pulse*rot*sqrt(pg+1)*state[i+indPG];
				}
				if(pg>0){
					dif[i]+=img*pump*pulse*conj(rot)*sqrt(pg)*state[i-indPG];
				}

				// coh e-g
				if(pg>0 && ae<maxAE){
					dif[i]+=img*cohg*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i-indPG+indAE];
				}
				if(pg<maxPG && ae>0){
					dif[i]+=img*cohg*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i+indPG-indAE];
				}

				// coh e-f
				if(pf>0 && af>0 && ae<maxAE){
					dif[i]+=img*cohf*sqrt(pf)*sqrt(af)*sqrt(ae+1)*state[i-indPF-indAF+indAE];
				}
				if(pf<maxPF && af<maxAF && ae>0){
					dif[i]+=img*cohf*sqrt(pf+1)*sqrt(af+1)*sqrt(ae)*state[i+indPF+indAF-indAE];
				}

			}
			for(i=0;i<vecSize;i++)state[i]+=dt*dif[i];

			// loss g photon
			sum=0;
			for(i=0;i<vecSize;i++)dif[i]=0;
			for(i=0;i<vecSize;i++){
				pg = getIdToPG(i);
				sum+=norm(state[i])*sqrt(pg);
			}
			poslosg=sum*dt*lossPG;
			flaghit = (r->next()<poslosg);
			if(flaghit){
				totalLossPG++;
				lossTime.push_back(dt*step);
			}
			sum=0;

			if(flaghit){
				for(i=0;i<vecSize;i++)dif[i]=0;
				for(i=0;i<vecSize;i++){
					pg = getIdToPG(i);
					if(pg<maxPG){
						dif[i]=state[i+indPG]*sqrt(pg+1);
					}
				}
				for(i=0;i<vecSize;i++)state[i]=dif[i];
				for(i=0;i<vecSize;i++){
					ae = getIdToAE(i);
					pf = getIdToPF(i);
					af = getIdToAF(i);
					state[i]-=0.5*((ae+af)*life+pf*lossPF)*dt*state[i];
					sum+=norm(state[i]);
				}

			}else{

				for(i=0;i<vecSize;i++){
					pg = getIdToPG(i);
					ae = getIdToAE(i);
					pf = getIdToPF(i);
					af = getIdToAF(i);
					state[i]-=0.5*((ae+af)*life+pf*lossPF+pg*lossPG)*dt*state[i];
					sum+=norm(state[i]);
				}
			}
			sum=sqrt(sum);

			for(i=0;i<vecSize;i++){
				state[i]/=sum;
			}

			// logging and end check
			if(step%div==0){
				totPG=totAE=totPF=totAF=totEN=trace=0;
				for(i=0;i<=maxEne;i++)ene[i]=0;
				for(i=0;i<vecSize;i++){
					pg=getIdToPG(i);
					ae=getIdToAE(i);
					pf=getIdToPF(i);
					af=getIdToAF(i);
					val = norm(state[i]);

					totPG+=val*pg;
					totAE+=val*ae;
					totPF+=val*pf;
					totAF+=val*af;
					totEN+=val*(pg+ae+pf);
					trace+=val;
					ene[pg+ae+pf]+=val;
				}

#ifdef DEBUG_OUT
				if(debug){
					fout << dt*step << " " << totEN << " " << totPG
					<< " " << totAE << " " << totPF << " " << totAF
					<< " " << 1-ene[0] << " " << poslosg << " " << totalLossPG << endl;
	//					cout << dt*step << " " << totEN << " " << totPG
	//					<< " " << totAE << " " << totPF << " " << totAF
	//					<< " " << 1-ene[0] << " " << poslosg << " " << totalLossPG << endl;

	//					for(i=0;i<vecSize;i++){
	//						if(norm(state[i])>0){
	//						fout << "(" << getIdToPG(i) << "," << getIdToAE(i) << "," << getIdToPF(i) << "," << getIdToAF(i) << ")" << "=" << state[i] << endl;
	//						}
	//					}
				}
#endif DEBUG_OUT
				if(dt*step>width && (1-ene[0])<eps)break;
			}
		}


#ifdef DEBUG_OUT
		if(debug){
			fout.close();
		}
#endif DEBUG_OUT
		return 0;
	}

	virtual int output(){
		fstream fhit;
		map<int,int> hit;
		fhit.open("hit.txt",ios::in);
		while(fhit && !fhit.eof()){
			int num,c;
			fhit >> num >> c;
			if(!fhit.fail()){
				hit.insert(map<int,int>::value_type(num,c));
			}
		}
		if(hit.count(totalLossPG)) hit[totalLossPG]++;
		else hit[totalLossPG]=1;
		fhit.close();
		fhit.open("hit.txt",ios::out);
		map<int,int>::iterator ite = hit.begin();
		while(ite!=hit.end()){
			fhit << (*ite).first << " " << (*ite).second << endl;
			ite++;
		}
		fhit.close();

		stringstream ss;
		ss << "hit" << totalLossPG << ".txt";
		fhit.open(ss.str(),ios::app);
		vector<double>::iterator vite = lossTime.begin();
		while(vite!=lossTime.end()){
			fhit << (*vite) << " ";
			vite++;
		}
		fhit << endl;
		fhit.close();
		return 0;
	}
	virtual int getTotalLossPG(){
		return totalLossPG;
	}
	virtual void outputProperty(){
		fstream fprop("property.txt",ios::out);
		fprop << "*** Energy Limit ***" << endl;
		fprop << " maxPG	=" << maxPG << endl;
		fprop << " maxAE	=" << maxAE << endl;
		fprop << " maxPF	=" << maxPF << endl;
		fprop << " maxAF	=" << maxAF << endl;
		fprop << " totAtom	=" << totAtom << endl;
		fprop << endl;
		fprop << "*** Time ***" << endl;
		fprop << " dt	=" << dt << endl;
		fprop << " maxstep	=" << maxstep << endl;
		fprop << " eps	=" << eps << endl;
		fprop << " div	=" << div << endl;
		fprop << " debug	=" << debug << endl;
		fprop << endl;
		fprop << "*** Input ***" << endl;
		fprop << " pump	=" << pump << endl;
		fprop << " detune	=" << detune << endl;
		fprop << " width	=" << width << endl;
		fprop << endl;
		fprop << "*** Interaction ***" << endl;
		fprop << " cohg	=" << cohg << endl;
		fprop << " cohf	=" << cohf << endl;
		fprop << endl;
		fprop << "*** Loss ***" << endl;
		fprop << " lossPG	=" << lossPG << endl;
		fprop << " lossPF	=" << lossPF << endl;
		fprop << " life	=" << life << endl;
		if(restarg.size()>0){
			fprop << "!!! unknown arg !!!" << endl;
			auto ite = restarg.begin();
			while(ite!=restarg.end()){
				fprop << " " << (*ite).first << "	=" << (*ite).second << endl;
				ite++;
			}
		}
		fprop.close();
	}
};

int main(int argc,char** argv){

	int corenum=0;
	int totalTask=1;
	string baseText="";
	vector< pair<string,string> > arg;
	for(int cc=1;cc<argc;cc++){
		string temp = argv[cc];
		vector<string> keyval;
		split(keyval,temp,is_any_of("="));
		if(keyval.size()>=2){
			if(keyval[0]=="base"){
				baseText = keyval[1];
				fstream bs(baseText,ios::in);
				while(bs && !bs.eof()){
					string str;
					vector<string> vstr;
					getline(bs,str);
					split(vstr,str,is_any_of("="));
					if(vstr.size()>=2){
						arg.push_back(make_pair(vstr[0],vstr[1]));
					}
				}
				if(bs)bs.close();
			}
			else{
				arg.push_back(make_pair(keyval[0],keyval[1]));
			}
		}
	}
	auto ite = arg.begin();
	while(ite!=arg.end()){
		if((*ite).first=="core"){corenum = lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="task"){totalTask = lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else ite++;
	}

	for(unsigned int cc=0;cc<arg.size();cc++){
		if(arg[cc].first=="core"){
			corenum = lexical_cast<int>(arg[cc].second);
		}
		else if(arg[cc].first=="task"){
			totalTask = lexical_cast<int>(arg[cc].second);
		}
	}

	int totalEndTask=0;
	int totalStartTask=0;

	if(corenum!=0){
		omp_set_num_threads(corenum);
	}

	auto totalStartPoint = chrono::high_resolution_clock::now();
	cout << "***start : #core = " << omp_get_max_threads() 
		<< " #task = " << totalTask << endl;

#pragma omp parallel
	{
		int myOwnTaskId;
		int myTotalTaskId=0;
		chrono::system_clock::time_point myStart,myEnd;
		long long myTime,myTotalTime;

		Simulator* sim = new Simulator();
		sim->init(arg);
		if(omp_get_thread_num()==0){
			sim->outputProperty();
		}

#pragma omp critical (output)
		{
			myOwnTaskId=0;
			myTotalTaskId=totalStartTask;
			totalStartTask++;
		}

		while(myTotalTaskId<totalTask){

			myStart = chrono::high_resolution_clock::now();
			sim->run();
			myEnd = chrono::high_resolution_clock::now();
			myTime = chrono::duration_cast<chrono::milliseconds>(myEnd-myStart).count();
			myTotalTime = chrono::duration_cast<chrono::seconds>(myEnd-totalStartPoint).count();

#pragma omp critical (output)
			{
				totalEndTask++;

				sim->output();
				cout << omp_get_thread_num() << " : " << totalEndTask << "/" << totalTask << " count = " << sim->getTotalLossPG() << " time = " << myTime << "ms  : totalTime = " << myTotalTime << "s" << endl;

				myTotalTaskId=totalStartTask;
				totalStartTask++;
				myOwnTaskId++;
			}
		}
		sim->close();
		delete sim;
	}
	auto totalEndPoint = chrono::high_resolution_clock::now();
	long long totalTime = chrono::duration_cast<chrono::milliseconds>(totalEndPoint-totalStartPoint).count(); 
	cout << "***end	time = " << totalTime << "ms" << " ( " << totalTime/totalTask << " ms per task )" << endl;

	return 0;
}