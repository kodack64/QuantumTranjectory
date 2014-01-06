
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
	if(usePulse && energy<eps && dt*step>pulseWidth)endFlag=true;

	//�L�[�{�[�h�������ꂽ�ꍇ�A�r���Œ��f�����蒆�ԏ�Ԃ��o�͂����肷��
#ifdef _MSC_VER
#ifndef _OPENMP
	if(_kbhit()){
		char c = _getch();
		if(c==VK_ESCAPE){
			cout << "# aborted" << endl;
			endFlag = true;
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
		}
		if(c==VK_RETURN){
			cout << "# log saved" << endl;
			loggingSave();
			isLogged=true;
		}
	}
#endif
#endif

	if(!isLogged && flagLossProbe){
//		cout << "# log saved" << endl;
//		loggingSave();
	}
}

// ���O�ۑ�
void Simulator::logging(){

	//���X���N�������Ԃ��������Ă���
	if(loggingLossTimeFlag){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
	//���X���N����m�����������Ă����i���X���N����m�������U������̌��q����V�X�e���̓��ߗ��ɔ�Ⴗ��j
	if(step%loggingUnit==0){
//		if(loggingLossTimeFlag){
			lossProbabilityLogAtom.push_back(probLossAtom);
			lossProbabilityLogProbe.push_back(probLossProbe);
			lossProbabilityLogControl.push_back(probLossControl);
//		}
//		cout << norm(state[0]) << endl;
	}
}

// ���O��������
void Simulator::loggingSave(){

	cout << "begin log" << endl;

//	auto iomode = loggingOffset==0?ios::out:ios::app;
	auto iomode = ios::out;
	int newOffset = max(max(lossProbabilityLogAtom.size(),lossProbabilityLogProbe.size()),lossProbabilityLogControl.size());

	stringstream ss;
	ss << "data\\log_" << _unit << "_" << _id  << "_";
	ofstream ofs;
	unsigned int i;

	// ���q�̃��X���o��
	if(useLossAtom){
		ofs.open(ss.str()+"time_atom.txt",iomode);
		for(i=0;i<lossTimeLogAtom.size();i++){
			ofs << lossTimeLogAtom[i] << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();
		lossTimeLogAtom.clear();

		ofs.open(ss.str()+"prob_atom.txt",iomode);
		for(i=0;i<lossProbabilityLogAtom.size();i++){
			ofs << (loggingOffset+i)*loggingUnit*dt << " " << lossProbabilityLogAtom[i]/dt/lossAtom << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();
		lossProbabilityLogAtom.clear();
	}

	// probe�̃��X���o��
	if(useLossProbe){
		ofs.open(ss.str()+"time_probe.txt",iomode);
		for(i=0;i<lossTimeLogProbe.size();i++){
			ofs << lossTimeLogProbe[i] << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();

		ofs.open(ss.str()+"prob_probe.txt",iomode);
		for(i=0;i<lossProbabilityLogProbe.size();i++){
			ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogProbe[i]/dt/lossProbe << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();

		lossTimeLogProbe.clear();
		lossProbabilityLogProbe.clear();
	}

	// control�̃��X���o��
	if(useLossControl){
		ofs.open(ss.str()+"time_control.txt",iomode);
		for(i=0;i<lossTimeLogControl.size();i++){
			ofs << lossTimeLogControl[i] << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();
		lossTimeLogControl.clear();

		ofs.open(ss.str()+"prob_control.txt",iomode);
		for(i=0;i<lossProbabilityLogControl.size();i++){
			ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogControl[i]/dt/lossControl << endl;
		}
		if(ofs.bad()){cout << "bad alloc " << endl;}
		ofs.close();
		lossProbabilityLogControl.clear();
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
