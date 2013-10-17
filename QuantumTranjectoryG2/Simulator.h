
#pragma once

#include <complex>
using namespace std;

class ParameterSet;
class Random;

class Simulator{
private:
	virtual void init();
	virtual void run();
	virtual void close();

	int maxPG;
	int maxAE;
	int maxPF;
	int maxAF;
	int totAtom;

	// use flag
	bool useLossPG;
	bool useLossAE;
	bool useLossPF;
	bool useLossAF;
	bool usePulse;

	//	time
	double dt;
	int div;
	long maxstep;

	//	input
	double pump;
	double detune;
	double width;

	//	coherence
	double cohg;
	double cohf;

	//	lifetime
	double lossPG;
	double lossPF;
	double life;

	// end condition
	double eps;
	int seed;

	// predetection
	int noDetect;
	int baseout;


	int indPG;
	int indAE;
	int indPF;
	int indAF;
	int vecSize;
	int maxEne;
	complex<double> img;

	// memory allocate
	Random *r;
	complex<double>* state;
	complex<double>* dif;
	double* ene;

public:
	virtual void execute(ParameterSet* param);
	virtual void executeNewThread(ParameterSet* param);
};