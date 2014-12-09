
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
	maxPG = par->getParamInt(_NAME(maxPG),maxPG);
	maxAE = par->getParamInt(_NAME(maxAE),maxAE);
	maxPF = par->getParamInt(_NAME(maxPF),maxPF);
	maxAF = par->getParamInt(_NAME(maxAF),maxAF);
	totAtom = par->getParamInt(_NAME(totAtom),totAtom);

	// use flag
	useLossProbe = par->getParamBoolean(_NAME(useLossProbe),useLossProbe);
	useLossAtom = par->getParamBoolean(_NAME(useLossAtom),useLossAtom);
	useLossControl = par->getParamBoolean(_NAME(useLossControl),useLossControl);
	postselAtom = par->getParamBoolean(_NAME(postselAtom),postselAtom);
	postselProbe = par->getParamBoolean(_NAME(postselProbe),postselProbe);
	postselControl = par->getParamBoolean(_NAME(postselControl),postselControl);

	usePulse = par->getParamBoolean(_NAME(usePulse),usePulse);

	//	time
	dt = par->getParamDouble(_NAME(dt),dt);
	maxstep = par->getParamInt(_NAME(maxstep),maxstep);

	// logging
	loggingUnit = par->getParamInt(_NAME(loggingUnit),loggingUnit);
	loggingTime = par->getParamBoolean(_NAME(loggingTime),loggingTime);
	loggingProb = par->getParamBoolean(_NAME(loggingProb),loggingProb);
	loggingJump = par->getParamBoolean(_NAME(loggingJump),loggingJump);


	//	input
	pulsePump = par->getParamDouble(_NAME(pulsePump),pulsePump);
	pulseDetune = par->getParamDouble(_NAME(pulseDetune),pulseDetune);
	pulseWidth = par->getParamDouble(_NAME(pulseWidth),pulseWidth);
	pulseCut = par->getParamDouble(_NAME(pulseCut),pulseCut);
	pulseShape = par->getParamInt(_NAME(pulseShape),pulseShape);
	pulseExp = par->getParamDouble(_NAME(pulseExp),pulseExp);

	//	coherence
	coherenceProbe = par->getParamDouble(_NAME(coherenceProbe),coherenceProbe);
	coherenceControl = par->getParamDouble(_NAME(coherenceControl),coherenceControl);

	//	lifetime
	lossProbe = par->getParamDouble(_NAME(lossProbe),lossProbe);
	lossControl = par->getParamDouble(_NAME(lossControl),lossControl);
	lossAtom = par->getParamDouble(_NAME(lossAtom),lossAtom);
	forceLossProbeTime = par->getParamInt(_NAME(forceLossProbeTime),forceLossProbeTime);
	forceLossProbeTimeSub = par->getParamInt(_NAME(forceLossProbeTimeSub),forceLossProbeTimeSub);

	// end condition
	eps = par->getParamDouble(_NAME(eps),eps);
}

// 読み込んだパラメータから使うパラメータを初期化
void Simulator::initParameter(){
	// ベクトルの添字と各量子の個数の対応を作成
	indPG=1;
	indAE=indPG*(maxPG+1);
	indPF=indAE*(maxAE+1);
	indAF=indPF*(maxPF+1);
	vecSize=indAF*(maxAF+1);
	maxEne=maxPG+maxAE+maxPF;

	// 定数の設定と配列の初期化
	seed=(int)time(NULL);
	img=complex<double>(0,1);
	step=0;
	lossTimeLogAtom.clear();
	lossTimeLogControl.clear();
	lossTimeLogProbe.clear();
	lossProbabilityLogAtom.clear();
	lossProbabilityLogControl.clear();
	lossProbabilityLogProbe.clear();
	logPulse.clear();
	g2ValueLogAtom.clear();
	g2ValueLogControl.clear();
	g2ValueLogProbe.clear();
	energyValueLogAll.resize(maxEne+1);
	energyValueLogProbe.resize(maxPG+1);
	energyValueLogControl.resize(maxPF+1);
	energyValueLogAtom.resize(maxAE+1);
	endFlag=false;
	loggingOffset=0;
	trace=0;
	energy=0;
	edgePG=0;
	edgePF=0;
	edgeAE=0;
	edgeAF=0;
	edgeAll=0;
	totProbeLoss=0;
	totControlLoss=0;
	totAtomLoss=0;
	maxEdge=0;
	totalShrink=0;
	totalProbLoss=0;
	trajectoryWeight=1.0;

	// メモリ領域の確保
//	r=new Random(seed);
	r = new Random();
	state= new complex<double>[vecSize];
	dif= new complex<double>[vecSize];
	ene = new double[maxEne+1];

	// 内部状態を真空に初期化
	state[0]=1;
}

//ログの初期化
void Simulator::initLogging(){
	cout.precision(8);
}
