#pragma warning(disable:4018)
#define _USE_MATH_DEFINES
#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <Windows.h>
#include <cmath>
using namespace std;

// ÉqÉXÉgÉOÉâÉÄÇÃê∂ê¨
void CommandCalcHistgram::execute(ParameterSet* par,queue<Command*>& coms){
	int i_unit;
	try{
		i_unit = boost::lexical_cast<int>(par->getParamInt("unit",0));
	}catch(boost::bad_lexical_cast e){
		i_unit=1;
	}
	vector<string> files;
	WIN32_FIND_DATA fd;
	name = string("./data/log_")+boost::lexical_cast<string>(i_unit)+string("_*_time_probe.txt");
	HANDLE h = FindFirstFile(name.c_str(),&fd);
	if(h!=INVALID_HANDLE_VALUE){
		files.push_back(fd.cFileName);
		while(FindNextFile(h,&fd)){
			files.push_back(fd.cFileName);
		}
	}
	cout << files.size() << " files found" << endl;

	double maxtime=0;
	vector< vector<double> > histgram;
	for(int i=0;i<files.size();i++){
		vector<double> time;
		ifstream ifs("./data/"+files[i]);
		while(ifs && !ifs.eof()){
			string line;
			getline(ifs,line);
			if(line.length()>0){
				double val = boost::lexical_cast<double>(line);
				if(maxtime<val)maxtime=val;
				time.push_back(val);
			}
		}
		ifs.close();
		if(time.size()>0){
			if(time.size()>histgram.size()){
				histgram.resize(time.size());
			}
			for(int i=0;i<time.size();i++){
				histgram[time.size()-1].push_back(time[i]);
			}
		}
	}
	cout << "load finish" << endl;
	for(int i=0;i<histgram.size();i++){
		cout << i+1 << " " << histgram[i].size()/(i+1) << "sample" << endl;
	}

	const double dt = boost::lexical_cast<double>(sdt);
	const double sig = boost::lexical_cast<double>(ssig);
	const int width = (int)sig*100+1;
	const int length = (int)(maxtime/dt)+width;
	ofstream ofs;
	double normsum;
	for(int i=0;i<histgram.size();i++){
		if(histgram[i].size()==0)continue;
		vector<double> wave(length,0);
		for(int j=0;j<histgram[i].size();j++){
			int pos = (int)(histgram[i][j]/dt);
			for(int k=-width/2;k<width/2;k++){
				if(pos+k>=0 && pos+k<wave.size()){
					wave[pos+k]+=exp(-pow(k*dt,2)/(2*pow(sig,2)))/sqrt(2*M_PI)/sig;
				}
			}
		}
		ofs.open("./analyze/log_0_sample_"+boost::lexical_cast<string>(i+1)+".txt");
		normsum=0;
		for(int j=0;j<wave.size();j++){
			ofs << j*dt << " " << wave[j] << endl;
			normsum += wave[j]*dt;
		}
		ofs.close();

		ofs.open("./analyze/log_0_sample_"+boost::lexical_cast<string>(i+1)+"_norm.txt");
		for(int j=0;j<wave.size();j++){
			ofs << j*dt << " " << wave[j]/normsum << endl;
		}
		ofs.close();
	}

	vector<double> allwave(length,0);
	for(int i=0;i<histgram.size();i++){
		for(int j=0;j<histgram[i].size();j++){
			int pos = (int)(histgram[i][j]/dt);
			for(int k=-width/2;k<width/2;k++){
				if(pos+k>=0 && pos+k<allwave.size()){
					allwave[pos+k]+=exp(-pow(k*dt,2)/(2*pow(sig,2)))/sqrt(2*M_PI)/sig;
				}
			}
		}
	}
	ofs.open("./analyze/log_0_sample_all.txt");
	normsum=0;
	for(int j=0;j<allwave.size();j++){
		ofs << j*dt << " " << allwave[j] << endl;
		normsum+=allwave[j];
	}
	ofs.close();
	ofs.open("./analyze/log_0_sample_all_norm.txt");
	for(int j=0;j<allwave.size();j++){
		ofs << j*dt << " " << allwave[j]/normsum << endl;
	}
	ofs.close();
}

void CommandCalcPower::execute(ParameterSet* par,queue<Command*>& coms){
}
