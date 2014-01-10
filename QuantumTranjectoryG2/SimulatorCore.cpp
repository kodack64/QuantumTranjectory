
#include "Simulator.h"
#include "Random.h"


#include <iostream>
#include <conio.h>

// ���̎����ł̃p���X�̕��f�U�����v�Z
void Simulator::calcPulseSize(){
	pulse=pulsePump*(complex<double>(cos(step*dt*pulseDetune),sin(step*dt*pulseDetune)));

	// usePulse=true�Ȃ�p���X�̌`�󂪎O�p�֐��ɂȂ�悤�ɕό`
	if(usePulse){
		if(dt*step<pulseWidth){
			pulse*=(-cos(dt*step/pulseWidth*acos(0)*4)+1)/2;
		}else{
			pulse=0;
		}
	}
}


// ���j�^���Ȏ��Ԕ��W�̌v�Z
void Simulator::calcLiuville(){
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// ��N
		if(pg<maxPG){
			dif[i]+=img*pulse*sqrt(pg+1)*state[i+indPG];
		}
		if(pg>0){
			dif[i]+=img*conj(pulse)*sqrt(pg)*state[i-indPG];
		}

		// probe���̑��ݍ�p
		if(pg>0 && ae<maxAE){
			dif[i]+=img*coherenceProbe*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i-indPG+indAE];
		}
		if(pg<maxPG && ae>0){
			dif[i]+=img*coherenceProbe*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i+indPG-indAE];
		}

		// control���̑��ݍ�p
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

	//���X���N����m�����v�Z
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		ae = getIdToAE(i);
		probLossAtom += norm(state[i])*ae;
		probLossProbe += norm(state[i])*pg;
		probLossControl += norm(state[i])*pf;
	}

	// ���X�͏��ŉ��Z�q�ɉ����Ĕ������ԂƎ����̒萔�ɔ��
	probLossAtom*=dt*lossAtom;
	probLossProbe*=dt*lossProbe;
	probLossControl*=dt*lossControl;

	// �����𗘗p���ă��X���N���邩�ǂ����𔻒�
	flagLossAtom = (useLossAtom)&(r->next()<probLossAtom)&(!postselAtom);
	flagLossProbe = (useLossProbe)&(r->next()<probLossProbe)&(!postselProbe);
	flagLossControl = (useLossControl)&(r->next()<probLossControl)&(!postselControl);

	//�����I�Ƀ��X������ꍇ�͂��̂悤�ɂ���
	if(forceLossProbeTime==step)flagLossProbe=true;

}

// ����̌��ʂɉ����Ďˉe
void Simulator::calcProjection(){

	for(i=0;i<vecSize;i++)dif[i]=0;

	// ���q�̎��R���o���N�����ꍇ�̌v�Z
	if(flagLossAtom){
		if(r->next()<0.5){
			// e����g
			for(i=0;i<vecSize;i++){
				ae = getIdToAE(i);
				if(ae<maxAE){
					state[i]=state[i+indAE]*sqrt(ae+1);
				}else{
					state[i]=0;
				}
			}
		}else{
			// e����f
			for(i=0;i<vecSize;i++){
				ae = getIdToAE(i);
				af = getIdToAF(i);
				if(ae<maxAE && af>0){
					state[i]=state[i+indAE-indAF]*sqrt(ae+1)*sqrt(af);
				}else{
					state[i]=0;
				}
			}
		}
	}
	//probe�̋��U�킩����q���������ꍇ
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
	//control�̋��U�킩����q���������ꍇ
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
	//���ƑΊp�a�ƃV�X�e�����̃G�l���M�[�ʂ̌v�Z
	trace=0;
	energy=0;
	edgePG=0;
	edgePF=0;
	edgeAE=0;
	edgeAF=0;
	edgeAll=0;
	for(i=0;i<vecSize;i++){
		ae = getIdToAE(i);
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		if(!flagLossAtom){
			state[i]-=0.5*ae*lossAtom*dt*state[i];
		}
		if(!flagLossProbe){
			state[i]-=0.5*pg*lossProbe*dt*state[i];
		}
		if(!flagLossControl){
			state[i]-=0.5*pf*lossControl*dt*state[i];
		}
		trace+=norm(state[i]);
		energy+=norm(state[i])*(ae+pg+pf);
		if(ae==maxAE)edgeAE+=norm(state[i]);
	}

	//���K��
	trace=sqrt(trace);
	for(i=0;i<vecSize;i++){
		state[i]/=trace;

		ae = getIdToAE(i);
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);
		if(ae==maxAE)edgeAE+=norm(state[i]);
		if(af==maxAF)edgeAF+=norm(state[i]);
		if(pg==maxPG)edgePG+=norm(state[i]);
		if(pf==maxPF)edgePF+=norm(state[i]);
		if(ae==maxAE || af==maxAF || pg==maxPG || pf==maxPF) edgeAll += norm(state[i]);
	}

	if(flagLossProbe){
		if(forceLossProbeTime==step){
			cout << "# probe loss forced" << endl;
		}else{
//			cout << "# probe loss" << endl;
		}
	}
}
