
#include "Simulator.h"
#include "Random.h"

// �p���X�`����v�Z
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

// ���j�^���ȕ����̌v�Z
void Simulator::calcLiuville(){
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// pump
		if(pg<maxPG){
			dif[i]+=img*pulse*sqrt(pg+1)*state[i+indPG];
		}
		if(pg>0){
			dif[i]+=img*conj(pulse)*sqrt(pg)*state[i-indPG];
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

// ���X�Ȃǂ̍�����ԂɂȂ�P�[�X���v�Z
void Simulator::calcLindblad(){

	calcProbabiliyOfLoss();

	calcProjection();
}

// ���X���N����m�����v�Z���Ĕ���
void Simulator::calcProbabiliyOfLoss(){
	probLossControl=0;
	probLossProbe=0;
	probLossAtom=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);
		probLossAtom += norm(state[i])*ae;
		probLossProbe += norm(state[i])*pg;
		probLossControl += norm(state[i])*pf;
	}
	probLossAtom*=dt*life;
	probLossProbe*=dt*lossProbe;
	probLossControl*=dt*lossControl;

	flagLossAtom = (useLossAtom)&(r->next()<probLossAtom);
	flagLossProbe = (useLossProbe)&(r->next()<probLossProbe);
	flagLossControl = (useLossControl)&(r->next()<probLossControl);
}

// ����̌��ʂɉ����Ďˉe
void Simulator::calcProjection(){

	for(i=0;i<vecSize;i++)dif[i]=0;

	// todo e����a��f�Ƀ����_���ɗ�����悤�ɂ���
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

	//���X���Ȃ��������̊��Ғl�̕␳
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

	//���K��
	trace=sqrt(trace);
	for(i=0;i<vecSize;i++)state[i]/=trace;
}
