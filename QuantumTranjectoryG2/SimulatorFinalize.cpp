
#include "Simulator.h"
#include "Random.h"

#include <iostream>
#include <iomanip>
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

	bool isLogged=false;

	//ステップ数の終了チェック
	if(step>maxstep)endFlag=true;
	//パルスを入射した場合、パルスが入りきった後にシステム内のエネルギーが一定以下になったら終了
	if(usePulse && energy<eps && dt*step>pulseWidth)endFlag=true;

	//キーボードが押された場合、途中で中断したり中間状態を出力したりする
#ifdef _MSC_VER
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

	if(!isLogged && flagLossProbe && loggingJump){
		cout << "# jump log" << endl;
		loggingSave();
	}
}

// ログ保存
void Simulator::logging(){

	//ロスが起きた時間をメモしておく
	if(loggingTime){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
	//ロスが起きる確率をメモしておく（ロスが起きる確率が共振器内部の光子数やシステムの透過率に比例する）
	if(loggingProb){
		if(step%loggingUnit==0){
			lossProbabilityLogAtom.push_back(probLossAtom);
			lossProbabilityLogProbe.push_back(probLossProbe);
			lossProbabilityLogControl.push_back(probLossControl);
		}
	}
}

// ログ書き込み
void Simulator::loggingSave(){

	auto iomode = loggingOffset==0?ios::out:ios::app;
	int newOffset = max(max(lossProbabilityLogAtom.size(),lossProbabilityLogProbe.size()),lossProbabilityLogControl.size());

	stringstream ss;
	ss << "data\\log_" << _unit << "_" << _id  << "_";
	ofstream ofs;
	unsigned int i;

	// 原子のロスを出力
	if(useLossAtom){

		if(loggingTime && !postselAtom){
			ofs.open(ss.str()+"time_atom.txt",iomode);
			for(i=0;i<lossTimeLogAtom.size();i++){
				ofs << lossTimeLogAtom[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_atom.txt",iomode);
			for(i=0;i<lossProbabilityLogAtom.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " << lossProbabilityLogAtom[i]/dt/lossAtom << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		lossTimeLogAtom.clear();
		lossProbabilityLogAtom.clear();
	}

	// probeのロスを出力
	if(useLossProbe){

		if(loggingTime && !postselProbe){
			ofs.open(ss.str()+"time_probe.txt",iomode);
			for(i=0;i<lossTimeLogProbe.size();i++){
				ofs << lossTimeLogProbe[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_probe.txt",iomode);
			for(i=0;i<lossProbabilityLogProbe.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogProbe[i]/dt/lossProbe << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		lossTimeLogProbe.clear();
		lossProbabilityLogProbe.clear();
	}

	// controlのロスを出力
	if(useLossControl){
		if(loggingTime && !postselControl){
			ofs.open(ss.str()+"time_control.txt",iomode);
			for(i=0;i<lossTimeLogControl.size();i++){
				ofs << lossTimeLogControl[i] << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		if(loggingProb){
			ofs.open(ss.str()+"prob_control.txt",iomode);
			for(i=0;i<lossProbabilityLogControl.size();i++){
				ofs << (loggingOffset+i)*loggingUnit*dt << " " <<  lossProbabilityLogControl[i]/dt/lossControl << endl;
			}
			if(ofs.bad()){cout << "bad alloc " << endl;}
			ofs.close();
		}

		lossTimeLogControl.clear();
		lossProbabilityLogControl.clear();
	}

	loggingOffset +=newOffset;

}

//確保したオブジェクトを解放
void Simulator::releaseParameter(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}
