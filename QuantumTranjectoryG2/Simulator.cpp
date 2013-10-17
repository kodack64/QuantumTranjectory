#include "Simulator.h"

#include <iostream>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;

#include "Random.h"

#define TOSTRING(s) #s

void Simulator::execute(ParameterSet* par){
	this->init();
	this->run();
	this->close();
}

void Simulator::executeNewThread(ParameterSet* par){
}

void Simulator::init(){
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

	// predetection
	noDetect=0;
	baseout=0;

	string str = TOSTRING(life);




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
}
void Simulator::run(){
}
void Simulator::close(){
}