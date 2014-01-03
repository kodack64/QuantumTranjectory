
#include "Simulator.h"
#include "Random.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#ifdef _MSC_VER
//マクロの再定義の警告を無視
#pragma warning(disable: 4005)
#include <conio.h>
#include <Windows.h>
#endif

// 終了条件のチェック
void Simulator::checkCondition(){

	//ステップ数の終了チェック
	if(step>maxstep)endFlag=true;
	//パルスを入射した場合、パルスが入りきった後にシステム内のエネルギーが一定以下になったら終了
	if(usePulse && energy<eps && dt*step>pulseWidth)endFlag=true;

	//キーボードが押された場合、途中で中断したり中間状態を出力したりする
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

// ログ保存
void Simulator::logging(){

	//ロスが起きた時間をメモしておく
	if(loggingLossTimeFlag){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
	//ロスが起きる確率をメモしておく（ロスが起きる確率が共振器内部の光子数やシステムの透過率に比例する）
	if(step%loggingUnit==0){
		if(loggingLossTimeFlag){
			lossProbabilityLogAtom.push_back(probLossAtom);
			lossProbabilityLogProbe.push_back(probLossProbe);
			lossProbabilityLogControl.push_back(probLossControl);
		}
//		cout << norm(state[0]) << endl;
	}
}

// ログ書き込み
void Simulator::loggingSave(){

	stringstream ss;
	ss << "log_" << _unit << "_" << _id  << "_";
	ofstream ofs;
	unsigned int i;

	// 原子のロスを出力
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

	// probeのロスを出力
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

	// controlのロスを出力
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

//確保したオブジェクトを解放
void Simulator::releaseParameter(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}
