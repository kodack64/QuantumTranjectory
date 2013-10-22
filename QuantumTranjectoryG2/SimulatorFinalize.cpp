
#include "Simulator.h"

// �I�������̃`�F�b�N
void Simulator::checkCondition(){
	if(step>maxstep)endFlag=true;
	if(energy<eps)endFlag=true;
}

// ���O�o��
void Simulator::logging(){
	if(loggingLossTimeFlag){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
	if(step%loggingUnit==0){
		if(loggingLossTimeFlag){
			if(flagLossAtom)	lossProbabilityLogAtom.push_back(probLossAtom);
			if(flagLossProbe)	lossProbabilityLogProbe.push_back(probLossProbe);
			if(flagLossControl)	lossProbabilityLogControl.push_back(probLossControl);
		}
	}
}
