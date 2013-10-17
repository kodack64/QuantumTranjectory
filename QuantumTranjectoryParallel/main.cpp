
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
#include <conio.h>

using namespace std;

//#define DEBUG_OUT


class Random{
public:
	std::random_device rd;
	std::mt19937_64 mt;
	std::uniform_real_distribution<double> dis;
	Random(int s){
//		mt=std::mt19937_64(rd());
		mt=std::mt19937_64(1);
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
		maxPG=2;
		maxAE=2;
		maxPF=2;
		maxAF=2;
		totAtom=10;

		// use flag
		useLossPG=true;
		useLossAE=true;
		useLossPF=true;
		useLossAF=true;
		usePulse=true;

		//	time
		dt=1e-2;
		div=100;
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

		// initialize
		for(int i=0;i<vecSize;i++)state[i]=0;
		state[0]=1;
		totalLossG=0;

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
		fout.precision(6);
#endif DEBUG_OUT
		
		int pg,ae,pf,af;
		double poslosg;
		complex<double> rot;
		double pulse;
		int i;
		long step;
		bool flaghit;
		double val;

		step=-1;
		poslosg=0;

#ifdef _OPENMP

		int numcore;
#pragma omp parallel
		{
			if(omp_get_thread_num()==0)numcore = omp_get_num_threads();
		}
		double* sum=new double[numcore];
		double* totPG= new double[numcore];
		double* totAE= new double[numcore];
		double* totPF= new double[numcore];
		double* totAF= new double[numcore];
		double* totEN= new double[numcore];
		double* trace= new double[numcore];
		double** ene = new double*[numcore];
		for(i=0;i<numcore;i++)ene[i]=new double[maxEne+1];
#else
		int numcore = 1;
		double* sum=new double[numcore];
		double* totPG= new double[numcore];
		double* totAE= new double[numcore];
		double* totPF= new double[numcore];
		double* totAF= new double[numcore];
		double* totEN= new double[numcore];
		double* trace= new double[numcore];
		double** ene = new double*[numcore];
		for(i=0;i<numcore;i++)ene[i]=new double[maxEne+1];
#endif _OPENMP


#ifdef _OPENMP
#pragma omp parallel private(i,pg,ae,pf,af,val)
#endif
		{

#ifdef _OPENMP
			int myid = omp_get_thread_num();
			int block = vecSize/numcore;
			int rest=vecSize%numcore;
			int myst=block*myid+min(myid,rest);
			int myen=block*(myid+1)+min(myid+1,rest);
#else
			int myid = 0;
			int block = vecSize/numcore;
			int rest=vecSize%numcore;
			int myst=block*myid+min(myid,rest);
			int myen=block*(myid+1)+min(myid+1,rest);
#endif _OPENMP

			while(step<maxstep){

				if(myid==0){
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
				}

#pragma omp barrier

				for(i=myst;i<myen;i++)dif[i]=0;
				for(i=myst;i<myen;i++){
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

#pragma omp barrier

				// loss g photon
				for(i=myst;i<myen;i++)state[i]+=dt*dif[i];
				sum[myid]=0;
				for(i=myst;i<myen;i++)dif[i]=0;
				for(i=myst;i<myen;i++){
					pg = getIdToPG(i);
					sum[myid]+=norm(state[i])*sqrt(pg);
				}

#pragma omp barrier

				if(myid==0){
					for(i=1;i<numcore;i++){
						sum[0]+=sum[i];
					}
					poslosg=sum[0]*dt*lossPG;
					flaghit = (r->next()<poslosg);
					if(flaghit){
						totalLossG++;
						lossTime.push_back(dt*step);
					}
					for(i=0;i<numcore;i++){
						sum[i]=0;
					}
				}

#pragma omp barrier

				if(flaghit){

					for(i=myst;i<myen;i++)dif[i]=0;
					for(i=myst;i<myen;i++){
						pg = getIdToPG(i);
						if(pg<maxPG){
							dif[i]=state[i+indPG]*sqrt(pg+1);
						}
					}

#pragma omp barrier

					for(i=myst;i<myen;i++)state[i]=dif[i];
					for(i=myst;i<myen;i++){
						ae = getIdToAE(i);
						pf = getIdToPF(i);
						af = getIdToAF(i);
						state[i]-=0.5*((ae+af)*life+pf*lossPF)*dt*state[i];
						sum[myid]+=norm(state[i]);
					}

				}else{

					for(i=myst;i<myen;i++){
						pg = getIdToPG(i);
						ae = getIdToAE(i);
						pf = getIdToPF(i);
						af = getIdToAF(i);
						state[i]-=0.5*((ae+af)*life+pf*lossPF+pg*lossPG)*dt*state[i];
						sum[myid]+=norm(state[i]);
					}
				}

#pragma omp barrier

				if(myid==0){
					for(i=1;i<numcore;i++){
						sum[0]+=sum[i];
					}
					sum[0]=sqrt(sum[0]);
				}

#pragma omp barrier

				for(i=myst;i<myen;i++){
					state[i]/=sum[0];
				}
#pragma omp barrier

				// logging and end check
				if(step%div==0){

					totPG[myid]=totAE[myid]=totPF[myid]=totAF[myid]=totEN[myid]=trace[myid]=0;
					for(i=0;i<=maxEne;i++)ene[myid][i]=0;
					for(i=myst;i<myen;i++){
						pg=getIdToPG(i);
						ae=getIdToAE(i);
						pf=getIdToPF(i);
						af=getIdToAF(i);
						val = norm(state[i]);

						totPG[myid]+=val*pg;
						totAE[myid]+=val*ae;
						totPF[myid]+=val*pf;
						totAF[myid]+=val*af;
						totEN[myid]+=val*(pg+ae+pf);
						trace[myid]+=val;
						ene[myid][pg+ae+pf]+=val;
					}

#pragma omp barrier

					if(myid==0){

						for(i=1;i<numcore;i++){
							totPG[0]+=totPG[i];
							totAE[0]+=totAE[i];
							totPF[0]+=totPF[i];
							totAF[0]+=totAF[i];
							totEN[0]+=totEN[i];
							trace[0]+=trace[i];
							for(int j=0;j<=maxEne;j++){
								ene[0][j]+=ene[i][j];
							}
						}
#ifdef DEBUG_OUT
						fout << dt*step << " " << totEN[0] << " " << totPG[0]
						<< " " << totAE[0] << " " << totPF[0] << " " << totAF[0]
						<< " " << 1-ene[0][0] << " " << poslosg << " " << totalLossG << endl;
						cout << dt*step << " " << totEN[0] << " " << totPG[0]
						<< " " << totAE[0] << " " << totPF[0] << " " << totAF[0]
						<< " " << 1-ene[0][0] << " " << poslosg << " " << totalLossG << endl;

						for(i=0;i<vecSize;i++){
							if(norm(state[i])>0){
							fout << "(" << getIdToPG(i) << "," << getIdToAE(i) << "," << getIdToPF(i) << "," << getIdToAF(i) << ")" << "=" << state[i] << endl;
							}
						}
#endif DEBUG_OUT
					}

#pragma omp barrier

					if(dt*step>width && (1-ene[0][0])<eps)break;
				}
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

		delete[] state;
		delete[] dif;
		delete r;
		delete[] sum;
		delete[] totPG;
		delete[] totAE;
		delete[] totPF;
		delete[] totAF;
		delete[] totEN;
		delete[] trace;
		for(i=0;i<numcore;i++){
			delete[] ene[i];
		}
		delete[] ene;


		return 0;
	}
};

int main(int argc,char** argv){
	Simulator* sim;

	int repeat=1000;
	bool init=false;
	if(init){
		fstream fs;
		fs.open("count.txt",ios::out);fs << "0" << endl;fs.close();
		fs.open("plot.txt",ios::out);fs.close();
		fs.open("hit.txt",ios::out);fs << "0 0" << endl;fs.close();
	}
	for(int i=0;i<repeat;i++){
		cout << "try " << i+1 << endl;
		auto start = chrono::high_resolution_clock::now();
		sim=new Simulator();
		sim->init(argc,argv);
		sim->run();
		delete sim;
		auto end = chrono::high_resolution_clock::now();
		cout << "	" << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms" << endl;
	}
	return 0;
}