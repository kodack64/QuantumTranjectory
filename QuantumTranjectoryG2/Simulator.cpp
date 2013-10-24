
#include "Simulator.h"

// ���s�S��
void Simulator::execute(int unit,int id,ParameterSet* par){
	_unit=unit;
	_id=id;
	this->init(par);
	this->run();
	this->close();
}


// �p�����[�^�̏�����
void Simulator::init(ParameterSet* par){
	loadParameter(par);
	initParameter();
	initLogging();
}

// �v�Z���[�v
void Simulator::run(){
	while(step<maxstep && !endFlag){
		calcPulseSize();
		calcLiuville();
		calcLindblad();
		logging();
		checkCondition();
		step++;
	}
}



// �������̉��
void Simulator::close(){
	loggingSave();
	releaseParameter();
}