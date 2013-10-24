
#include "Simulator.h"

// 実行全体
void Simulator::execute(int unit,int id,ParameterSet* par){
	_unit=unit;
	_id=id;
	this->init(par);
	this->run();
	this->close();
}


// パラメータの初期化
void Simulator::init(ParameterSet* par){
	loadParameter(par);
	initParameter();
	initLogging();
}

// 計算ループ
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



// メモリの解放
void Simulator::close(){
	loggingSave();
	releaseParameter();
}