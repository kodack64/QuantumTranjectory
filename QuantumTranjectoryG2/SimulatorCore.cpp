
#include "Simulator.h"
#include "Random.h"


#include <iostream>
#include <fstream>
#include <conio.h>

// ���̎����ł̃p���X�̕��f�U�����v�Z
void Simulator::calcPulseSize(){
	pulse=pulsePump*(complex<double>(cos(step*dt*pulseDetune),sin(step*dt*pulseDetune)));

	// usePulse=true�Ȃ�p���X�̌`�󂪎O�p�֐��ɂȂ�悤�ɕό`
	if(usePulse){
		double pos = dt*step-pulseWidth*pulseCut/2;
//		if(dt*step<pulseWidth) pos = pulseWidth - pulseWidth*pulseCut/2;
		double val = exp(-pos*pos/(2*pulseWidth*pulseWidth))/sqrt(2*3.14159)/pulseWidth;
		pulse*=sqrt(val);

//		ofstream ofs("pulse2.txt",ios::app);
//		ofs << val << endl;
//		ofs.close();

/*
		if(dt*step<pulseWidth){
//			pulse*=(-cos(dt*step/pulseWidth*acos(0)*4)+1)/2;
			pulse*=sqrt((-cos(dt*step/pulseWidth*acos(0)*4)+1)/2);
		}else{
			pulse=0;
		}
		*/
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
		totAtomLoss++;
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
		totProbeLoss++;
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
		totControlLoss++;
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
	}

	//���K��
	double g2atoma=0;
	double g2probea=0;
	double g2controla=0;
	g2Probe=g2Atom=g2Control=0;
	double enepf[20];
	for(i=0;i<20;i++)enepf[i]=0;
	trace=sqrt(trace);

	for(i=0;i<=maxEne;i++)energyValueLogAll[i].push_back(0);
	for(i=0;i<=maxPG;i++)energyValueLogProbe[i].push_back(0);
	for(i=0;i<=maxPF;i++)energyValueLogControl[i].push_back(0);
	for(i=0;i<=maxAE;i++)energyValueLogAtom[i].push_back(0);

	for(i=0;i<vecSize;i++){
		state[i]/=trace;

		ae = getIdToAE(i);
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);
		g2Atom += ae*(ae-1)*norm(state[i]);
		g2Control += pf*(pf-1)*norm(state[i]);
		g2Probe += pg*(pg-1)*norm(state[i]);
		g2atoma+=ae*norm(state[i]);
		g2controla+=pf*norm(state[i]);
		g2probea+=pg*norm(state[i]);

		enepf[pf]+=norm(state[i]);
		if(loggingProb){
			if(step%loggingUnit==0){
				energyValueLogAll[pg+ae+pf][energyValueLogAll[pg+ae+pf].size()-1] +=norm(state[i]);
				energyValueLogProbe[pg][energyValueLogProbe[pg].size()-1] +=norm(state[i]);
				energyValueLogControl[pf][energyValueLogControl[pf].size()-1] +=norm(state[i]);
				energyValueLogAtom[ae][energyValueLogAtom[ae].size()-1] +=norm(state[i]);
			}
		}

		if(ae==maxAE)edgeAE+=norm(state[i])*ae;
		if(af==maxAF)edgeAF+=norm(state[i])*af;
		if(pg==maxPG)edgePG+=norm(state[i])*pg;
		if(pf==maxPF)edgePF+=norm(state[i])*pf;
		if(ae==maxAE || af==maxAF || pg==maxPG || pf==maxPF) edgeAll += norm(state[i])*(ae+af+pg+pf);
	}
	if(edgeAll>maxEdge)maxEdge=edgeAll;
	g2Atom/=pow(g2atoma,2);
	g2Control/=pow(g2controla,2);
	g2Probe/=pow(g2probea,2);

/*	if(step%1000 == 0){
		for(int i=0;i<10;i++){
			cout << i << ":" << enepf[i] << endl;
		}
	}*/

	if(flagLossProbe){
		if(forceLossProbeTime==step){
			cout << "# probe loss forced" << endl;
		}else{
//			cout << "# probe loss" << endl;
		}
	}
}
