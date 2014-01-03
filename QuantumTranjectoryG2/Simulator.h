
#pragma once

#include <complex>
#include <vector>
#include <string>
using namespace std;

class ParameterSet;
class Random;

class Simulator{
private:

	int _unit;
	int _id;

	virtual void init(ParameterSet*);
	virtual void loadParameter(ParameterSet*);
	virtual void initParameter();
	virtual void initLogging();

	virtual void run();
	virtual void calcPulseSize();
	virtual void calcLiuville();
	virtual void calcLindblad();
	virtual void calcProbabiliyOfLoss();
	virtual void calcProjection();
	virtual void logging();
	virtual void checkCondition();

	virtual void close();
	virtual void releaseParameter();
	virtual void loggingSave();

	// macro
	inline int getIdToPG(int i){return (i%indAE)/indPG;}
	inline int getIdToAE(int i){return (i%indPF)/indAE;}
	inline int getIdToPF(int i){return (i%indAF)/indPF;}
	inline int getIdToAF(int i){return (i%vecSize)/indAF;}

	// loop variable
	int pg;
	int ae;
	int pf;
	int af;
	double trace;
	double energy;
	double edgePG;
	double edgeAE;
	double edgePF;
	double edgeAF;
	double edgeAll;
	int step;

	// temp variable
	int i;
	double sum;

	// pulse variable
	complex<double> pulse;
	complex<double> pulseRot;

	// loss variable
	bool flagLossAtom;
	bool flagLossControl;
	bool flagLossProbe;

	double probLossProbe;
	double probLossControl;
	double probLossAtom;


	// constant variable
	int maxPG;
	int maxAE;
	int maxPF;
	int maxAF;
	int totAtom;

	//  given parameters
	// use flag
	bool useLossProbe;
	bool useLossAtom;
	bool useLossControl;
	bool usePulse;

	//	time
	double dt;
	long maxstep;

	//	input
	double pulsePump;
	double pulseDetune;
	double pulseWidth;

	//	coherence
	double coherenceProbe;
	double coherenceControl;

	//	lifetime
	double lossProbe;
	double lossControl;
	double life;

	// end condition
	bool endFlag;
	double eps;
	int seed;

	// logging
	int loggingUnit;
	bool loggingLossTimeFlag;
	bool loggingLossProbabilityFlag;
	string loggingLossTimeName;
	string loggingLossProbabilityName;
	vector<double> lossTimeLogAtom;
	vector<double> lossTimeLogControl;
	vector<double> lossTimeLogProbe;
	vector<double> lossProbabilityLogAtom;
	vector<double> lossProbabilityLogControl;
	vector<double> lossProbabilityLogProbe;
	int loggingOffset;

	// indirectry given parameters
	int indPG;
	int indAE;
	int indPF;
	int indAF;
	int vecSize;
	int maxEne;

	// const value
	complex<double> img;

	// memory allocate
	Random *r;
	complex<double>* state;
	complex<double>* dif;
	double* ene;

public:
	virtual void execute(int unit,int id,ParameterSet* param);
};