
#include "Simulator.h"
#include "Random.h"

#include <iostream>
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

	//�X�e�b�v���̏I���`�F�b�N
	if(step>maxstep)endFlag=true;
	//�p���X����˂����ꍇ�A�p���X�����肫������ɃV�X�e�����̃G�l���M�[�����ȉ��ɂȂ�����I��
	if(usePulse && energy<eps && dt*step>pulseWidth)endFlag=true;

	//�L�[�{�[�h�������ꂽ�ꍇ�A�r���Œ��f�����蒆�ԏ�Ԃ��o�͂����肷��
#ifdef _MSC_VER
	if(_kbhit()){
		char c = _getch();
		if(c==VK_RETURN){
			cout << "execution aborted" << endl;
			endFlag = true;
		}
		if(c==VK_SPACE){
			cout << "energy : " << energy << endl;
			cout << "step : " << step << " / " << maxstep << endl;
			cout << "prob : " << probLossProbe << endl;
		}
	}
#endif
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
		if(loggingLossTimeFlag){
			lossProbabilityLogAtom.push_back(probLossAtom);
			lossProbabilityLogProbe.push_back(probLossProbe);
			lossProbabilityLogControl.push_back(probLossControl);
		}
//		cout << norm(state[0]) << endl;
	}
}

// ���O��������
void Simulator::loggingSave(){

	stringstream ss;
	ss << "log_" << _unit << "_" << _id  << "_";
	ofstream ofs;
	unsigned int i;

	// ���q�̃��X���o��
	if(useLossAtom){
		ofs.open(ss.str()+"time_atom.txt",ios::out);
		for(i=0;i<lossTimeLogAtom.size();i++){
			ofs << lossTimeLogAtom[i] << endl;
		}
		ofs.close();
		ofs.open(ss.str()+"prob_atom.txt",ios::out);
		for(i=0;i<lossProbabilityLogAtom.size();i++){
			ofs << i*loggingUnit*dt << " " << lossProbabilityLogAtom[i] << endl;
		}
		ofs.close();
	}

	// probe�̃��X���o��
	if(useLossProbe){
		ofs.open(ss.str()+"time_probe.txt",ios::out);
		for(i=0;i<lossTimeLogProbe.size();i++){
			ofs << lossTimeLogProbe[i] << endl;
		}
		ofs.close();
		ofs.open(ss.str()+"prob_probe.txt",ios::out);
		for(i=0;i<lossProbabilityLogProbe.size();i++){
			ofs << i*loggingUnit*dt << " " <<  lossProbabilityLogProbe[i] << endl;
		}
		ofs.close();
	}

	// control�̃��X���o��
	if(useLossControl){
		ofs.open(ss.str()+"time_control.txt",ios::out);
		for(i=0;i<lossTimeLogControl.size();i++){
			ofs << lossTimeLogControl[i] << endl;
		}
		ofs.close();
		ofs.open(ss.str()+"prob_control.txt",ios::out);
		for(i=0;i<lossProbabilityLogControl.size();i++){
			ofs << i*loggingUnit*dt << " " <<  lossProbabilityLogControl[i] << endl;
		}
		ofs.close();
	}
}

//�m�ۂ����I�u�W�F�N�g�����
void Simulator::releaseParameter(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}
