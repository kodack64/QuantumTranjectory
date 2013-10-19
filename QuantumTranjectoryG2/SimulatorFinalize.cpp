
#include "Simulator.h"

// 終了条件のチェック
void Simulator::checkCondition(){
	if(step>maxstep)endFlag=true;
	if(energy<eps)endFlag=true;
}

// ログ出力
void Simulator::logging(){
}
