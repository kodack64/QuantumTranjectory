
#include "DistributionToTime.h"
#include <ctime>
#include <random>
#include <iostream>
#include <fstream>
#include <conio.h>
using namespace std;

void DistributionToTime::make(){
	std::mt19937 engine;
	std::poisson_distribution<> poisson(power*div);
	for(int i=0;i<loop;i++){
		int count = poisson(engine);
		for(int j=0;j<count;j++){
			counts.push_back(div*i);
		}
	}
}

void DistributionToTime::timeDif(double dif,double coherent){
	vector<double> ncounts;
	for(int i=0;i<counts.size();i++){
		int sum=1;
		for(int j=i-1;j>=0;j--){
			if(counts[i]-coherent<= counts[j]){
				sum++;
			}else{
				break;
			}
		}
/*		for(int j=i+1;j<counts.size();j++){
			if(counts[i]-coherent<= counts[j] && counts[j] < counts[i]+coherent){
				sum++;
			}else{
				break;
			}
		}*/
		cout << sum << endl;
		ncounts.push_back(counts[i]-dif*(sum-1)/sum);
	}
	counts.swap(ncounts);
}

void DistributionToTime::output(string file){
	ofstream ofs(file,ios::out);
	for(int i=0;i<counts.size();i++){
		ofs << counts[i] << endl;
	}
	ofs.close();
}
