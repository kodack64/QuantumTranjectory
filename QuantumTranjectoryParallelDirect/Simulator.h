
#pragma once

#include <complex>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

class Random;


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
	int totalLossAE;
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

	int noDetect;

	vector<pair<string,string> > restarg;

	virtual int getIdToPG(int i){return (i%indAE)/indPG;}
	virtual int getIdToAE(int i){return (i%indPF)/indAE;}
	virtual int getIdToPF(int i){return (i%indAF)/indPF;}
	virtual int getIdToAF(int i){return (i%vecSize)/indAF;}

	fstream fout;
	fstream nodOut;
	int pg;
	int ae;
	int pf;
	int af;
	double poslosg;
	complex<double> rot;
	double pulse;
	int i;
	long step;
	bool flaghit;
	int baseout;
	double val;

	double sum;
	double totPG;
	double totAE;
	double totPF;
	double totAF;
	double totEN;
	double trace;

	vector<double> poslist;
	double possum;
	long hitstep;


	virtual void runPrepareDebugOut();
	virtual void runInitLoop();
	virtual void runLoop();
	virtual void runCloseLoop();

	virtual void runLoopLiouville();
	virtual void runLoopLindblad();
	virtual void runLoopLogging();

public:
	Simulator();
	virtual ~Simulator();
	virtual int init(vector<pair<string,string> > arg);
	virtual void close();
	virtual int run();
	virtual int getTotalLossPG();
	virtual int output();
	virtual void outputProperty();
};