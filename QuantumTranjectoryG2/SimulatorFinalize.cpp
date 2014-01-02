
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#ifdef _MSC_VER
#include <conio.h>
#include <Windows.h>
#endif

#include "Simulator.h"
#include "Random.h"


// 終了条件のチェック
void Simulator::checkCondition(){
	if(step>maxstep)endFlag=true;
	if(usePulse && energy<eps && dt*step>pulseWidth)endFlag=true;
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
	if(loggingLossTimeFlag){
		if(flagLossAtom)lossTimeLogAtom.push_back(dt*step);
		if(flagLossProbe)lossTimeLogProbe.push_back(dt*step);
		if(flagLossControl)lossTimeLogControl.push_back(dt*step);
	}
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

void Simulator::releaseParameter(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}