
#include "Simulator.h"
#include "Random.h"


#include <iostream>
#include <fstream>
#include <conio.h>

// その時刻でのパルスの複素振幅を計算
void Simulator::calcPulseSize(){
	pulse=pulsePump*(complex<double>(cos(step*dt*pulseDetune),sin(step*dt*pulseDetune)));

	// usePulse=trueならパルスの形状が三角関数になるように変形
	if(usePulse){
		double pos = dt*step-pulseWidth*pulseCut/2;
		double val;

		if(pulseShape==0)val = exp(-pos*pos/(2*pulseWidth*pulseWidth))/sqrt(2*3.14159)/pulseWidth;
		else if(pulseShape==1)val = (dt*step<pulseWidth*pulseCut)?(1-abs(pos)/(pulseWidth*pulseCut/2))/(pulseWidth*pulseCut/2):0;
		else if(pulseShape==2)val = exp(-((abs(pos)<pulseWidth*5)?pulseWidth*5:abs(pos))*pulseExp);
		else if(pulseShape==3)val = pulseExp*exp(-dt*step/pulseWidth);
		else val=0;

		pulse*=sqrt(val);

/*		ofstream ofs("pulse.txt",ios::app);
		ofs << dt*step << " " << abs(pulse) << endl;
		ofs.close();*/

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


// ユニタリな時間発展の計算
void Simulator::calcLiuville(){
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// 励起
		if(pg<maxPG){
			dif[i]+=img*pulse*sqrt(pg+1)*state[i+indPG];
		}
		if(pg>0){
			dif[i]+=img*conj(pulse)*sqrt(pg)*state[i-indPG];
		}

		// probe側の相互作用
		if(pg>0 && ae<maxAE){
			dif[i]+=img*coherenceProbe*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i-indPG+indAE];
//			dif[i]+=img*coherenceProbe*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom)*state[i-indPG+indAE];
		}
		if(pg<maxPG && ae>0){
			dif[i]+=img*coherenceProbe*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i+indPG-indAE];
//			dif[i]+=img*coherenceProbe*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom)*state[i+indPG-indAE];
		}

		// control側の相互作用
		if(pf>0 && af>0 && ae<maxAE){
			dif[i]+=img*coherenceControl*sqrt(pf)*sqrt(af)*sqrt(ae+1)*state[i-indPF-indAF+indAE];
//			dif[i]+=img*coherenceControl*sqrt(pf)*sqrt(ae+1)*state[i-indPF-indAF+indAE];
		}
		if(pf<maxPF && af<maxAF && ae>0){
			dif[i]+=img*coherenceControl*sqrt(pf+1)*sqrt(af+1)*sqrt(ae)*state[i+indPF+indAF-indAE];
//			dif[i]+=img*coherenceControl*sqrt(pf+1)*sqrt(ae)*state[i+indPF+indAF-indAE];
		}
	}
	for(i=0;i<vecSize;i++)state[i]+=dif[i]*dt;
}

// ロスなどの混合状態になるケースを計算
void Simulator::calcLindblad(){

	calcProbabiliyOfLoss();

	calcProjection();
}

// ロスが起きる確率を計算して判定
void Simulator::calcProbabiliyOfLoss(){
	probLossControl=0;
	probLossProbe=0;
	probLossAtom=0;

	//ロスが起きる確率を計算
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		pf = getIdToPF(i);
		ae = getIdToAE(i);
		probLossAtom += norm(state[i])*ae;
		probLossProbe += norm(state[i])*pg;
		probLossControl += norm(state[i])*pf;
	}

	// ロスは消滅演算子に加えて微小時間と寿命の定数に比例
	probLossAtom*=dt*lossAtom;
	probLossProbe*=dt*lossProbe;
	probLossControl*=dt*lossControl;

	totalProbLoss+= probLossAtom + probLossProbe+probLossControl;

	// 乱数を利用してロスが起きるかどうかを判定
	flagLossAtom = (useLossAtom)&(r->next()<probLossAtom)&(!postselAtom);
	flagLossProbe = (useLossProbe)&(r->next()<probLossProbe)&(!postselProbe);
	flagLossControl = (useLossControl)&(r->next()<probLossControl)&(!postselControl);

	//強制的にロスさせる場合はそのようにする
	if(forceLossProbeTime==step)flagLossProbe=true;

}

// 測定の結果に応じて射影
void Simulator::calcProjection(){

	for(i=0;i<vecSize;i++)dif[i]=0;

	// 原子の自然放出が起きた場合の計算
	if(flagLossAtom){
		totAtomLoss++;
		if(r->next()<0.5){
			// eからg
			for(i=0;i<vecSize;i++){
				ae = getIdToAE(i);
				if(ae<maxAE){
					state[i]=state[i+indAE]*sqrt(ae+1);
				}else{
					state[i]=0;
				}
			}
		}else{
			// eからf
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
	//probeの共振器から光子が抜けた場合
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
	//controlの共振器から光子が抜けた場合
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


	//ロスがなかった時の期待値の補正
	//あと対角和とシステム中のエネルギー量の計算
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
			state[i]*=(1-0.5*ae*lossAtom*dt);
		}
		if(!flagLossProbe){
			state[i]*=(1-0.5*pg*lossProbe*dt);
		}
		if(!flagLossControl){
			state[i]*=(1-0.5*pf*lossControl*dt);
		}
		trace+=norm(state[i]);
		energy+=norm(state[i])*(ae+pg+pf);
	}
//	totalShrink += tracedec;
//	cout << tracedec << " " << totalShrink << " " << totalProbLoss << " " << 1-trace << endl;

	//正規化
	double g2atoma=0;
	double g2probea=0;
	double g2controla=0;
	g2Probe=g2Atom=g2Control=0;
	energy1=energy2=0;
	double enepf[20];
	for(i=0;i<20;i++)enepf[i]=0;
	trace=sqrt(trace);

	for(i=0;i<=maxEne;i++)energyValueLogAll[i].push_back(0);
	for(i=0;i<=maxPG;i++)energyValueLogProbe[i].push_back(0);
	for(i=0;i<=maxPF;i++)energyValueLogControl[i].push_back(0);
	for(i=0;i<=maxAE;i++)energyValueLogAtom[i].push_back(0);

	if(flagLossAtom || flagLossProbe || flagLossControl) totalShrink=0;
	else totalShrink += (1-trace*trace);

	if(!flagLossProbe)trajectoryWeight *=(1.0-probLossProbe);
	else trajectoryWeight*=probLossProbe;

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

		if(ae+pg+pf==1 && pf==af) energy1 += norm(state[i]);
		if(ae+pg+pf==2 && pf==af) energy2 += norm(state[i]);

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

	double gp=coherenceProbe*sqrt(1.0*totAtom);
	double gc=coherenceControl;

	darkfidelity1 = norm((state[indPG]*gc - state[indAF+indPF]*gp) / sqrt(pow(gp,2)+pow(gc,2))) / energy1; 

	darkfidelity2 = 
		norm(
		(gc*gc*state[2*indPG]/sqrt(2) - gp*gc*state[indPG+indPF+indAF] + 0.5*gp*gp*state[2*(indPF+indAF)])
		/sqrt(0.5*pow(gc,4) + pow(gc*gp,2) + 0.25*pow(gp,4))
		)/energy2;

	//darkmatch1 = norm((eigenvec(mg10)*gc-eigenvec(mf01)*gp)/sqrt(gp*gp+gc*gc))/pos1;
	//darkmatch2 = norm((eigenvec(mg20)*gc*gc/sqrt(2.0)-eigenvec(mf11)*gp*gc+eigenvec(m2f02)*gp*gp*0.5)/sqrt(0.5*pow(gc,4)+gp*gp*gc*gc+0.25*pow(gp,4)))/pos2;


/*	if(step%1000 == 0){
		for(int i=0;i<10;i++){
			cout << i << ":" << enepf[i] << endl;
		}
	}*/

/*	trace=0;
	for(i=0;i<vecSize;i++){
		trace+=norm(state[i]);
	}
	cout << "trace : " << trace << endl;*/

	if(flagLossProbe){
		if(forceLossProbeTime==step){
			cout << "# probe loss forced" << endl;
		}else{
//			cout << "# probe loss" << endl;
		}
	}
}
