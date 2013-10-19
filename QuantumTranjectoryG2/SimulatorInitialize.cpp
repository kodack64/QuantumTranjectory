
#pragma warning(disable : 4819)

#include "Simulator.h"
#include "Random.h"
#include "ParameterSet.h"

#include <iostream>
using namespace std;

// 変数名を取得
#define _NAME(s) #s

// ParameterSetからパラメータの読み込み
void Simulator::loadParameter(ParameterSet* par){
	// state limit
	maxPG=4;
	maxAE=4;
	maxPF=0;
	maxAF=0;
	totAtom=10;
	maxPG = par->getParamInt(_NAME(maxPG));
	maxAE = par->getParamInt(_NAME(maxAE));
	maxPF = par->getParamInt(_NAME(maxPF));
	maxAF = par->getParamInt(_NAME(maxAF));
	totAtom = par->getParamInt(_NAME(totAtom));

	// use flag
	useLossProbe=true;
	useLossAtom=true;
	useLossControl=true;
	usePulse=true;
	useLossProbe = par->getParamBoolean(_NAME(useLossProbe));
	useLossAtom = par->getParamBoolean(_NAME(useLossAtom));
	useLossControl = par->getParamBoolean(_NAME(useLossControl));
	usePulse = par->getParamBoolean(_NAME(usePulse));

	//	time
	dt=1e-4;
	maxstep=100000000;
	dt = par->getParamDouble(_NAME(dt));
	maxstep = par->getParamInt(_NAME(maxstep));

	// logging
	loggingUnit=1000;
	loggingUnit = par->getParamInt(_NAME(loggingUnit));


	//	input
	pulsePump=1.5e-1;
	pulseDetune=0e-1;
	pulseWidth=1e1;
	pulsePump = par->getParamDouble(_NAME(pulsePump));
	pulseDetune = par->getParamDouble(_NAME(pulseDetune));
	pulseWidth = par->getParamDouble(_NAME(pulseWidth));

	//	coherence
	coherenceProbe=1e-1;
	coherenceControl=0e-0;
	coherenceProbe = par->getParamDouble(_NAME(coherenceProbe));
	coherenceControl = par->getParamDouble(_NAME(coherenceControl));

	//	lifetime
	lossProbe=3e-1;
	lossControl=3e-1;
	life=6e-3;
	lossProbe = par->getParamDouble(_NAME(lossProbe));
	lossControl = par->getParamDouble(_NAME(lossControl));
	life = par->getParamDouble(_NAME(life));

	// end condition
	eps=1e-3;
	eps = par->getParamDouble(_NAME(eps));
}

// 読み込んだパラメータから使うパラメータを初期化
void Simulator::initParameter(){
	// array index
	indPG=1;
	indAE=indPG*(maxPG+1);
	indPF=indAE*(maxAE+1);
	indAF=indPF*(maxPF+1);
	vecSize=indAF*(maxAF+1);
	maxEne=maxPG+maxAE+maxPF;

	// const value
	seed=(int)time(NULL);
	img=complex<double>(0,1);
	step=0;
	lossTimeAtom.clear();
	lossTimeControl.clear();
	lossTimeProbe.clear();
	endFlag=false;

	// memory allocate
	r=new Random(seed);
	state= new complex<double>[vecSize];
	dif= new complex<double>[vecSize];
	ene = new double[maxEne+1];
}

void Simulator::initLogging(){
	cout.precision(3);
}
