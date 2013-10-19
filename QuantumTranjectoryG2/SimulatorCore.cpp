
#include "Simulator.h"
#include "Random.h"

// パルス形状を計算
void Simulator::calcPulseSize(){

	pulse=pulsePump*(complex<double>(cos(step*dt*pulseDetune),sin(step*dt*pulseDetune)));

	if(usePulse){
		if(dt*step<pulseWidth){
			pulse*=(-cos(dt*step/pulseWidth*acos(0)*4)+1)/2;
		}else{
			pulse=0;
		}
	}
}

// ユニタリな部分の計算
void Simulator::calcLiuville(){
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// pump
		if(ae<maxAE){
			dif[i]+=img*pulse*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i+indAE];
		}
		if(ae>0){
			dif[i]+=img*conj(pulse)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i-indAE];
		}

		// coh e-g
		if(pg>0 && ae<maxAE){
			dif[i]+=img*coherenceProbe*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i-indPG+indAE];
		}
		if(pg<maxPG && ae>0){
			dif[i]+=img*coherenceProbe*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i+indPG-indAE];
		}

		// coh e-f
		if(pf>0 && af>0 && ae<maxAE){
			dif[i]+=img*coherenceControl*sqrt(pf)*sqrt(af)*sqrt(ae+1)*state[i-indPF-indAF+indAE];
		}
		if(pf<maxPF && af<maxAF && ae>0){
			dif[i]+=img*coherenceControl*sqrt(pf+1)*sqrt(af+1)*sqrt(ae)*state[i+indPF+indAF-indAE];
		}
	}
	for(i=0;i<vecSize;i++)state[i]+=dif[i]*dt;
}

// ロスが起きる確率を計算して判定
void Simulator::calcProbabiliyOfLoss(){
	probLossControl=0;
	probLossProbe=0;
	probLossAtom=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);
		probLossAtom += norm(state[i])*sqrt(ae);
		probLossProbe += norm(state[i])*sqrt(pg);
		probLossControl += norm(state[i])*sqrt(pf);
	}
	probLossAtom*=dt*life;
	probLossProbe*=dt*lossProbe;
	probLossControl*=dt*lossControl;

	flagLossAtom = (useLossAtom)&(r->next()<probLossAtom);
	flagLossProbe = (useLossProbe)&(r->next()<probLossProbe);
	flagLossControl = (useLossControl)&(r->next()<probLossControl);

	if(flagLossAtom)lossTimeAtom.push_back(dt*step);
	if(flagLossProbe)lossTimeProbe.push_back(dt*step);
	if(flagLossControl)lossTimeControl.push_back(dt*step);
}

// ロスなどの混合状態になるケースを計算
void Simulator::calcLindblad(){

	calcProbabiliyOfLoss();

	calcProjection();
}

// 測定の結果に応じて射影
void Simulator::calcProjection(){

	for(i=0;i<vecSize;i++)dif[i]=0;

	if(flagLossAtom){
		for(i=0;i<vecSize;i++){
			ae = getIdToAE(i);
			if(ae<maxAE){
				state[i]=state[i+indAE]*sqrt(ae+1);
			}else{
				state[i]=0;
			}
		}
	}
	if(flagLossProbe){
		for(i=0;i<vecSize;i++){
			pg = getIdToPG(i);
			if(pg<maxPG){
				state[i]=state[i+indPG]*sqrt(pg+1);
			}else{
				state[i]=0;
			}
		}
	}
	if(flagLossControl){
		for(i=0;i<vecSize;i++){
			pf = getIdToPF(i);
			if(pf<maxPF){
				state[i]=state[i+indPF]*sqrt(pf+1);
			}else{
				state[i]=0;
			}
		}
	}

	trace=0;
	energy=0;
	for(i=0;i<vecSize;i++){
		ae = getIdToAE(i);
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		if(!flagLossAtom){
			state[i]-=0.5*ae*life*dt*state[i];
		}
		if(!flagLossProbe){
			state[i]-=0.5*pg*lossProbe*dt*state[i];
		}
		if(!flagLossControl){
			state[i]-=0.5*pf*lossControl*dt*state[i];
		}
		trace+=norm(state[i]);
		energy+=norm(state[i])*(ae+pg+pf);
	}
	trace=sqrt(trace);
	for(i=0;i<vecSize;i++)state[i]/=trace;
}
