
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <complex>
#include <ctime>
#include <cmath>
#include <random>
#include <map>
#include <algorithm>
#include <chrono>
#include <omp.h>

using namespace std;

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
	int totalLossG;
	vector <double> lossTime;

	double pump;
	double cohg;
	double cohf;
	double lossPG;
	double lossPF;
	double life;

	double detune;
	double width;

	double* ene;
	double eps;

	bool usePulse;
	bool useLossPG;
	bool useLossAE;
	bool useLossPF;
	bool useLossAF;

	int div;
	long maxstep;
	complex<double> img;

	virtual void interpret(int cc,string s){
		seed=(int)time(NULL);
	}

public:
	Simulator(){
	}
	virtual int init(int argc,char** argv){

		// setup
		for(int cc=0;cc<argc;cc++)interpret(cc,argv[cc]);
		r=new Random(seed);
		img=complex<double>(0,1);

		// state limit
		maxPG=5;
		maxAE=5;
		maxPF=5;
		maxAF=5;
		totAtom=10;

		// use flag
		useLossPG=true;
		useLossAE=true;
		useLossPF=true;
		useLossAF=true;
		usePulse=true;

		//	time
		dt=1e-2;
		div=1;
		maxstep=100000000;

		//	input
		pump=1e-0;
		detune=1e-1;
		width=1e2;

		//	coherence
		cohg=1e-1;
		cohf=1e-0;

		//	lifetime
		lossPG=3e-2;
		lossPF=3e-2;
		life=6e-0;

		// end condition
		eps=1e-4;

		// no change
		indPG=1;
		indAE=indPG*(maxPG+1);
		indPF=indAE*(maxAE+1);
		indAF=indPF*(maxPF+1);
		vecSize=indAF*(maxAF+1);
		maxEne=maxPG+maxAE+maxPF;

		// memory allocate
		state= new complex<double>[vecSize];
		dif= new complex<double>[vecSize];
		ene = new double[maxEne+1];

		// setting
		cout.precision(3);

		return 0;
	}
	virtual int getIdToPG(int i){return (i%indAE)/indPG;}
	virtual int getIdToAE(int i){return (i%indPF)/indAE;}
	virtual int getIdToPF(int i){return (i%indAF)/indPF;}
	virtual int getIdToAF(int i){return (i%vecSize)/indAF;}
	virtual int run(){

#ifdef DEBUG_OUT
		fstream fcount;
		int cc;
		fcount.open("count.txt",ios::in);
		fcount >> cc;
		fcount.close();
		stringstream datafile;
		datafile << "./data/data" << cc << ".txt";
		fcount.open("count.txt",ios::out);
		fcount << cc+1;
		fcount.close();
		fstream fout(datafile.str(),ios::out);
		fout.precision(8);
#endif DEBUG_OUT
		
		int pg,ae,pf,af;
		double sum;
		double poslosg;
		double totPg,totAe,totPf,totAf,totEn,trace;
		long st;
		complex<double> rot;
		double pulse;
		int i;

		// initialize
		for(int i=0;i<vecSize;i++)state[i]=0;
		state[0]=1;
		totalLossG=0;

		for(st=0;st<maxstep;st++){

			rot = complex<double>(cos(st*dt*detune),sin(st*dt*detune));
			if(usePulse){
				if(dt*st<width){
					pulse=(-cos(dt*st/width*acos(0)*4)+1)/2;
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
			if(useLossPG){
				for(i=0;i<vecSize;i++)dif[i]=0;
				sum=0;
				for(i=0;i<vecSize;i++){
					pg = getIdToPG(i);
					sum+=norm(state[i])*sqrt(pg);
				}
				poslosg=sum*dt*lossPG;
				if(r->next()<poslosg){
					totalLossG++;
					lossTime.push_back(dt*st);
					for(i=0;i<vecSize;i++){
						pg = getIdToPG(i);
						if(pg<maxPG){
							dif[i]=state[i+indPG]*sqrt(pg+1);
						}
					}
				}else{
					for(i=0;i<vecSize;i++){
						pg = getIdToPG(i);
						dif[i]=state[i]-0.5*pg*lossPG*dt*state[i];
					}
				}
				for(i=0;i<vecSize;i++)state[i]=dif[i];
			}

			// loss ae atom
			if(useLossAE){
				for(i=0;i<vecSize;i++)dif[i]=0;
				for(i=0;i<vecSize;i++){
					ae = getIdToAE(i);
					dif[i]=state[i]-0.5*ae*life*dt*state[i];
				}
				for(i=0;i<vecSize;i++)state[i]=dif[i];
			}

			// loss pf atom
			if(useLossPF){
				for(i=0;i<vecSize;i++)dif[i]=0;
				for(i=0;i<vecSize;i++){
					pf = getIdToPF(i);
					dif[i]=state[i]-0.5*pf*lossPF*dt*state[i];
				}
				for(i=0;i<vecSize;i++)state[i]=dif[i];
			}

			// loss af atom
			if(useLossAF){
				for(i=0;i<vecSize;i++)dif[i]=0;
				for(i=0;i<vecSize;i++){
					af = getIdToAF(i);
					dif[i]=state[i]-0.5*af*life*dt*state[i];
				}
				for(i=0;i<vecSize;i++)state[i]=dif[i];
			}

			// normalize
			sum=0;
			for(i=0;i<vecSize;i++){
				sum+=norm(state[i]);
			}
			sum=sqrt(sum);
			for(i=0;i<vecSize;i++)state[i]/=sum;

			// logging and end check
			if(st%div==0){
				totPg=totAe=totPf=totAf=totEn=trace=0;
				for(i=0;i<=maxEne;i++)ene[i]=0;
				for(i=0;i<vecSize;i++){
					pg=getIdToPG(i);
					ae=getIdToAE(i);
					pf=getIdToPF(i);
					af=getIdToAF(i);

					double val = norm(state[i]);
					totPg+=val*pg;
					totAe+=val*ae;
					totPf+=val*pf;
					totAf+=val*af;
					totEn+=val*(pg+ae+pf);
					trace+=val;
					ene[pg+ae+pf]+=val;
				}
#ifdef DEBUG_OUT
					fout << dt*st << " " << totEn << " " << totPg
					<< " " << totAe << " " << totPf << " " << totAf
					<< " " << 1-ene[0] << " " << poslosg << " " << totalLossG << endl;
				for(i=0;i<vecSize;i++){
					if(norm(state[i])>0){
						fout << "(" << getIdToPG(i) << "," << getIdToAE(i) << "," << getIdToPF(i) << "," << getIdToAF(i) << ")" << "=" << state[i] << endl;
					}
				}
#endif DEBUG_OUT
//				cout << dt*st << " " << totEn << " " << totPg << " " << totAe << " " << totPf << " " << totAf << " " << 1-ene[0] << " " << poslosg << " " << totalLossG << endl;
				if(dt*st>width && (1-ene[0])<eps)break;
			}

		}
#ifdef DEBUG_OUT
		fout.close();
#endif DEBUG_OUT

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
		if(hit.count(totalLossG)) hit[totalLossG]++;
		else hit[totalLossG]=1;
		fhit.close();
		fhit.open("hit.txt",ios::out);
		map<int,int>::iterator ite = hit.begin();
		while(ite!=hit.end()){
			fhit << (*ite).first << " " << (*ite).second << endl;
			ite++;
		}
		fhit.close();

		stringstream ss;
		ss << "hit" << totalLossG << ".txt";
		fhit.open(ss.str(),ios::app);
		vector<double>::iterator vite = lossTime.begin();
		while(vite!=lossTime.end()){
			fhit << (*vite) << " ";
			vite++;
		}
		fhit << endl;
		fhit.close();

		cout << "	end count=" << totalLossG << endl; 

		return 0;
	}
	virtual void close(){
		delete[] state;
		delete[] dif;
		delete[] ene;
		delete r;
	}
};

int main(int argc,char** argv){

	int repeat=10000;
	bool init=false;
	if(init){
		fstream fs;
		fs.open("count.txt",ios::out);fs << "0" << endl;fs.close();
		fs.open("plot.txt",ios::out);fs.close();
		fs.open("hit.txt",ios::out);fs << "0 0" << endl;fs.close();
	}
	Simulator* sim;
	int i;
	sim=new Simulator();
	sim->init(argc,argv);
	for(i=0;i<repeat;i++){
		cout << "try " << i+1 << endl;
		auto start = chrono::high_resolution_clock::now();
		sim->run();
		auto end = chrono::high_resolution_clock::now();
		cout << "	" << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms" << endl;
	}
	delete sim;
	return 0;
}