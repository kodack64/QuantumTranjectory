
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <conio.h>

using namespace std;

int main(int argc,char**argv){
	ofstream ofs("CalcAverage.txt",ios::out);
	ofs << "# i average max" << endl;
	for(int i=1;i<argc;i++){
		string file = argv[i];
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
		ofs << i+1 << " " << ave/sum << " " << maxt << endl;
	}
	ofs.close();
	return 0;
}