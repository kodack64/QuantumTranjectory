
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <conio.h>

using namespace std;

int main(int argc,char**argv){
	if(argc<=1)return 0;
	string file = argv[1];
	ifstream ifs(file);
	double ave=0;
	double sum=0;
	double dt=0;
	double time,val,ltime;
	double maximum=0,maxt=0;
	ifs >> ltime >> val;
	while(!ifs.eof() && ifs){
		ifs >> time;
		if(dt==0)dt=time-ltime;
		if((ifs >> val)!=0){
			ave+=time*val*dt;
			sum+=val*dt;
			if(val>maximum){
				maximum=val;
				maxt=time;
			}
//			cout << time*val*dt << endl;
		}
	}
	cout << "average:" << ave/sum << endl;
	cout << "max:" << maxt << endl;
	_getch();
	return 0;
}