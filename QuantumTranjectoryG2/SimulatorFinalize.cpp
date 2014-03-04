
#include "Simulator.h"
#include "Random.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

#ifdef _MSC_VER
//�}�N���̍Ē�`�̌x���𖳎�
#pragma warning(disable: 4005)
#include <conio.h>
#include <Windows.h>
#endif

// �I�������̃`�F�b�N
void Simulator::checkCondition(){

	bool isLogged=false;

	//�X�e�b�v���̏I���`�F�b�N
	if(step>maxstep)endFlag=true;
	//�p���X����˂����ꍇ�A�p���X�����肫������ɃV�X�e�����̃G�l���M�[�����ȉ��ɂȂ�����I��
//	if(usePulse && energy<eps && dt*step>pulseWidth*pulseCut)endFlag=true;

	//�L�[�{�[�h�������ꂽ�ꍇ�A�r���Œ��f�����蒆�ԏ�Ԃ��o�͂����肷��
#ifdef _MSC_VER
	if(_kbhit()){
		char c = _getch();
		if(c==VK_ESCAPE){
			cout << "# aborted" << endl;
			endFlag = true;

			ofstream ofs("analyze\\g2exp.txt",ios::app);
			ofs << -pulseExp << " " << g2ValueLogProbe[g2ValueLogProbe.size()-1] << endl;
			ofs.close();

		}
		if(c==VK_SPACE){
			cout << "# status" << endl;
			cout << "#  energy : " << energy << endl;
			cout << "#  step : " << step << " / " << maxstep << endl;
			cout << "#  prob : " << probLossProbe << endl;
			cout << "#  edgepg : " << edgePG << endl;
			cout << "#  edgeae : " << edgeAE << endl;
			cout << "#  edgepf : " << edgePF << endl;
			cout << "#  edgeaf : " << edgeAF << endl;
			cout << "#  edgeall : " << edgeAll << endl;
			cout << "#  AllEne ";
			for(int i=0;i<min(maxEne,4);i++){
				cout << energyValueLogAll[i][energyValueLogAll[i].size()-1] << " ";
			}
			cout << endl;
			cout << "#  AllProbe ";
			for(int i=0;i<min(maxPG,4);i++){
				cout << energyValueLogProbe[i][energyValueLogProbe[i].size()-1] << " ";
			}
			cout << endl;
			cout << "#  AllControl ";
			for(int i=0;i<min(maxPF,4);i++){
				cout << energyValueLogControl[i][energyValueLogControl[i].size()-1] << " ";
			}
			cout << endl;
			cout << "#  AllAtom ";
			for(int i=0;i<min(maxAE,4);i++){
				cout << energyValueLogAtom[i][energyValueLogAtom[i].size()-1] << " ";
			}
			cout << endl;
		}
		if(c==VK_RETURN){
			cout << "# log saved" << endl;
			loggingSave();
			isLogged=true;
		}
	}
#endif

	if(!isLogged && flagLossProbe && loggingJump){
		cout << "# jump log" << endl;
		loggingSave();
	}
}

// ���O�ۑ�
void Simulator::logging(){

	//���X���N�������Ԃ��������Ă���
	if(loggingTime){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
	//���X���N����m�����������Ă����i���X���N����m�������U������̌��q����V�X�e���̓��ߗ��ɔ�Ⴗ��j
	if(loggingProb){
		if(step%loggingUnit==0){
			lossProbabilityLogAtom.push_back(probLossAtom);
			lossProbabilityLogProbe.push_back(probLossProbe);
			lossProbabilityLogControl.push_back(probLossControl);
			g2ValueLogAtom.push_back(g2Atom);
			g2ValueLogProbe.push_back(g2Probe);
			g2ValueLogControl.push_back(g2Control);
		}
	}
	if(pulseShape==2){
		if(-pulseWidth*5.1 < dt*step-pulseWidth*pulseCut/2 && dt*step-pulseWidth*pulseCut/2 < -pulseWidth*5.1+dt*1.1){
			ofstream ofs("analyze\\g2exp.txt",ios::app);
			ofs << pulseExp << " " << g2ValueLogProbe[g2ValueLogProbe.size()-1] << endl;
			ofs.close();
		}
	}
}

// ���O��������
void Simulator::loggingSave(){

	auto iomode = loggingOffset==0?ios::out:ios::app;
	int newOffset = max(max(lossProbabilityLogAtom.size(),lossProbabilityLogProbe.size()),lossProbabilityLogControl.size());

	stringstream ss;
	ss << "data\\log_" << _unit << "_" << _id  << "_";
	ofstream ofs;
	unsigned int i;

	// ���q�̃��X���o��
	if(useLossAtom){

		if(loggingTime && !postselAtom){
			ofs.open(ss.str()+"time_atom.txt",iomode);
			for(i=0;i<lossTimeLogAtom.size();i++){
				ofs << lossTimeLogAtom[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_atom.txt",iomode);
			for(i=0;i<lossProbabilityLogAtom.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " << lossProbabilityLogAtom[i]/dt/lossAtom << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"g2_atom.txt",iomode);
			for(i=0;i<g2ValueLogAtom.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " << g2ValueLogAtom[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"ene_atom.txt",iomode);
			for(i=0;i<energyValueLogAtom[0].size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " ";
				for(int j=0;j<maxAE;j++){
					ofs << energyValueLogAtom[j][i] << " ";
				}
				ofs << endl;
			}
			ofs.close();
		}

		lossTimeLogAtom.clear();
		lossProbabilityLogAtom.clear();
		g2ValueLogAtom.clear();
		for(int j=0;j<maxAE;j++)energyValueLogAtom[j].clear();
	}

	// probe�̃��X���o��
	if(useLossProbe){

		if(loggingTime && !postselProbe){
			ofs.open(ss.str()+"time_probe.txt",iomode);
			for(i=0;i<lossTimeLogProbe.size();i++){
				ofs << lossTimeLogProbe[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_probe.txt",iomode);
			for(i=0;i<lossProbabilityLogProbe.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogProbe[i]/dt/lossProbe << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"g2_probe.txt",iomode);
			for(i=0;i<g2ValueLogProbe.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " << g2ValueLogProbe[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"ene_probe.txt",iomode);
			for(i=0;i<energyValueLogProbe[0].size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " ";
				for(int j=0;j<maxPG;j++){
 					ofs << energyValueLogProbe[j][i] << " ";
				}
				ofs << endl;
			}
			ofs.close();

			ofs.open(ss.str()+"ene_all.txt",iomode);
			for(i=0;i<energyValueLogAll[0].size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " ";
				for(int j=0;j<maxEne;j++){
					ofs << energyValueLogAll[j][i] << " ";
				}
				ofs << endl;
			}
			ofs.close();

			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		lossTimeLogProbe.clear();
		lossProbabilityLogProbe.clear();
		g2ValueLogProbe.clear();
		for(int j=0;j<maxPG;j++)energyValueLogProbe[j].clear();
		for(int j=0;j<maxEne;j++)energyValueLogAll[j].clear();
	}

	// control�̃��X���o��
	if(useLossControl){
		if(loggingTime && !postselControl){
			ofs.open(ss.str()+"time_control.txt",iomode);
			for(i=0;i<lossTimeLogControl.size();i++){
				ofs << lossTimeLogControl[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_control.txt",iomode);
			for(i=0;i<lossProbabilityLogControl.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogControl[i]/dt/lossControl << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"g2_control.txt",iomode);
			for(i=0;i<g2ValueLogControl.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " << g2ValueLogControl[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();

			ofs.open(ss.str()+"ene_control.txt",iomode);
			for(i=0;i<energyValueLogControl[0].size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " ";
				for(int j=0;j<maxPF;j++){
					ofs << energyValueLogControl[j][i] << " ";
				}
				ofs << endl;
			}
			ofs.close();
		}

		lossTimeLogControl.clear();
		lossProbabilityLogControl.clear();
		g2ValueLogControl.clear();
		for(int j=0;j<maxPF;j++)energyValueLogControl[j].clear();
	}

	loggingOffset +=newOffset;

}

//�m�ۂ����I�u�W�F�N�g�����
void Simulator::releaseParameter(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}
