
#include <string>
#include <iostream>
#include "DistributionToTime.h"
#include "TimeToG2.h"

using namespace std;

int main(int  argc,char** argv){
	string file;
	if(argc>=2){
		file = argv[1];
	}else{
		cout << "file>" ;
		cin >> file;
	}

	DistributionToTime* dtt = new DistributionToTime();
	dtt->setPower(0.1,1,100000);
	dtt->make();
	dtt->output(file+"_org.txt");
	dtt->timeDif(10,20);
	dtt->output(file+"_dif.txt");
	delete dtt;

	TimeToG2* ttg = new TimeToG2();
	ttg->setInput(file+"_org.txt");
	ttg->calc();
	ttg->output(file+"_org_g2.txt");
	delete ttg;
	ttg = new TimeToG2();
	ttg->setInput(file+"_dif.txt");
	ttg->calc();
	ttg->output(file+"_dif_g2.txt");

	ttg->plot(file+"_org_g2.txt",file+"_dif_g2.txt");
	delete ttg;

	return 0;
}