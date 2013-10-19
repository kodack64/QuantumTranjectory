
#include "Simulator.h"
#include "Random.h"

// ���s�S��
void Simulator::execute(ParameterSet* par){
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
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}