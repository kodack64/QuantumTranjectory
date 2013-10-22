
#include "Simulator.h"

// 終了条件のチェック
void Simulator::checkCondition(){
	if(step>maxstep)endFlag=true;
	if(energy<eps)endFlag=true;
}

// ログ出力
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
