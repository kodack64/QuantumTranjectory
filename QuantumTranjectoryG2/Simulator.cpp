

#pragma warning(disable : 4819)

#include "Simulator.h"
#include "ParameterSet.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;

#include "Random.h"

#define _NAME(s) #s

void Simulator::execute(ParameterSet* par){
	this->init(par);
	this->run();
	this->close();
}

void Simulator::executeNewThread(ParameterSet* par){
}

void Simulator::init(ParameterSet* par){

	loadParameter(par);
	initParameter();
	cout.precision(3);
}

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
	useLossPG=true;
	useLossAE=true;
	useLossPF=true;
	useLossAF=true;
	usePulse=true;
	useLossPG = par->getParamBoolean(_NAME(useLossPG));
	useLossAE = par->getParamBoolean(_NAME(useLossAE));
	useLossPF = par->getParamBoolean(_NAME(useLossPF));
	useLossAF = par->getParamBoolean(_NAME(useLossAF));
	usePulse = par->getParamBoolean(_NAME(usePulse));

	//	time
	dt=1e-4;
	div=1000;
	maxstep=100000000;
	dt = par->getParamDouble(_NAME(dt));
	div = par->getParamInt(_NAME(div));
	maxstep = par->getParamInt(_NAME(maxstep));

	//	input
	pump=1.5e-1;
	detune=0e-1;
	width=1e1;
	pump = par->getParamDouble(_NAME(pump));
	detune = par->getParamDouble(_NAME(detune));
	width = par->getParamDouble(_NAME(width));

	//	coherence
	cohg=1e-1;
	cohf=0e-0;
	cohg = par->getParamDouble(_NAME(cohg));
	cohf = par->getParamDouble(_NAME(cohf));

	//	lifetime
	lossPG=3e-1;
	lossPF=3e-1;
	life=6e-3;
	lossPG = par->getParamDouble(_NAME(lossPG));
	lossPF = par->getParamDouble(_NAME(lossPF));
	life = par->getParamDouble(_NAME(life));

	// end condition
	eps=1e-3;
	lossPG = par->getParamDouble(_NAME(eps));

	// predetection
	noDetect=0;
	baseout=0;
	noDetect = par->getParamInt(_NAME(noDetect));
	baseout = par->getParamInt(_NAME(baseout));
}

void Simulator::initParameter(){
	// const val
	seed=(int)time(NULL);
	indPG=1;
	indAE=indPG*(maxPG+1);
	indPF=indAE*(maxAE+1);
	indAF=indPF*(maxPF+1);
	vecSize=indAF*(maxAF+1);
	maxEne=maxPG+maxAE+maxPF;
	img=complex<double>(0,1);
	step=0;

	// memory allocate
	r=new Random(seed);
	state= new complex<double>[vecSize];
	dif= new complex<double>[vecSize];
	ene = new double[maxEne+1];
}

void Simulator::run(){
	calcPulseSize();
	calcLiuville();
	calcLindblad();
}

void Simulator::calcPulseSize(){
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

void Simulator::calcLiuville(){
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// pump
		if(ae<maxAE){
			dif[i]+=img*pump*pulse*rot*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i+indAE];
		}
		if(ae>0){
			dif[i]+=img*pump*pulse*conj(rot)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i-indAE];
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
	for(i=0;i<vecSize;i++)state[i]+=dif[i]*dt;
}

void Simulator::calcLindblad(){

	// loss g photon
	if(noDetect>0){
		sum=0;
		for(i=0;i<vecSize;i++)dif[i]=0;
		for(i=0;i<vecSize;i++){
			ae = getIdToAE(i);
			sum+=norm(state[i])*sqrt(ae);
		}
		poslosg=sum*dt*life;
		flaghit=false;
	}else{
		if(step<hitstep){
			flaghit=false;
		}else if(step==hitstep){
			flaghit=true;
		}else{
			sum=0;
			for(i=0;i<vecSize;i++)dif[i]=0;
			for(i=0;i<vecSize;i++){
				ae = getIdToAE(i);
				sum+=norm(state[i])*sqrt(ae);
			}
			poslosg=sum*dt*life;
			flaghit = r->next()<poslosg;
		}
	}

	if(flaghit){
		totalLossAE++;
		lossTime.push_back(dt*step);
	}
	sum=0;

	if(flaghit){
		for(i=0;i<vecSize;i++)dif[i]=0;
		for(i=0;i<vecSize;i++){
			ae = getIdToAE(i);
			if(ae<maxAE){
				dif[i]=state[i+indAE]*sqrt(ae+1);
			}
		}
		for(i=0;i<vecSize;i++)state[i]=dif[i];
		for(i=0;i<vecSize;i++){
			pg = getIdToPG(i);
			pf = getIdToPF(i);
			af = getIdToAF(i);
			state[i]-=0.5*(af*life+pg*lossPG+pf*lossPF)*dt*state[i];
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
	for(i=0;i<vecSize;i++)state[i]/=sum;

	if(noDetect>0)nodOut << step << " " <<  poslosg << endl;

}

void Simulator::close(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}