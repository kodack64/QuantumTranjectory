
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <conio.h>
using namespace std;

int main(){
	int current=300;
	int step=10;
	ifstream ifs("files.txt");
	ofstream ofs("gather.txt",ios::out);
	while(ifs && !ifs.eof()){
		string file;
		if((ifs >> file)==0)break;
		ifstream iifs(file);
		while(iifs && !iifs.eof()){
			double d,val,val2;
			cout << "get val" << endl;
			if((iifs >> d >> val >> val2)==0)break;
			ofs << current << " " << val << " " << val2 << endl;
			current+=step;
		}
		iifs.close();
	}
	ifs.close();
	ofs.close();
	_getch();
	return 0;
}
