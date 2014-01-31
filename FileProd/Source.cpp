
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

int main(){
	ifstream ifs("gather.txt");
	ifstream ifs2("wave.txt");
	ofstream ofs("gat.txt");
	while(ifs && !ifs.eof()){
		double time,val,val2,val3;
		ifs >> time >> val >> val2;
		while(1){
			double t2;
			ifs2 >> t2 >> val3;
			if(t2>=time)break;
		}
		ofs << time << " " <<val << " " <<val2 <<" " << val3 << endl;
	}
	ofs.close();
	ifs.close();
	ifs2.close();
	return 0;
}