
#include "Simulator.h"

// �I�������̃`�F�b�N
void Simulator::checkCondition(){
	if(step>maxstep)endFlag=true;
	if(energy<eps)endFlag=true;
}

// ���O�o��
void Simulator::logging(){
}
