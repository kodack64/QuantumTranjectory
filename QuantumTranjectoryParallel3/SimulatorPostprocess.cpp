
#include "Simulator.h"

#include <fstream>
#include <map>
#include "Random.h"

Simulator::~Simulator(){
}

void Simulator::close(){
	delete r;
	delete[] state;
	delete[] dif;
	delete[] ene;
}

int Simulator::output(){
	fstream fhit;
	map<int,int> hit;
	fhit.open("hit.txt",ios::in);
	while(fhit && !fhit.eof()){
		int num,c;
		fhit >> num >> c;
		if(!fhit.fail()){
			hit.insert(map<int,int>::value_type(num,c));
		}
	}
	if(hit.count(totalLossPG)) hit[totalLossPG]++;
	else hit[totalLossPG]=1;
	fhit.close();
	fhit.open("hit.txt",ios::out);
	map<int,int>::iterator ite = hit.begin();
	while(ite!=hit.end()){
		fhit << (*ite).first << " " << (*ite).second << endl;
		ite++;
	}
	fhit.close();

	stringstream ss;
	ss << "hit" << totalLossPG << ".txt";
	fhit.open(ss.str(),ios::app);
	vector<double>::iterator vite = lossTime.begin();
	while(vite!=lossTime.end()){
		fhit << (*vite) << " ";
		vite++;
	}
	fhit << endl;
	fhit.close();
	return 0;
}

int Simulator::getTotalLossPG(){
	return totalLossPG;
}

void Simulator::outputProperty(){
	fstream fprop("property.txt",ios::out);
	fprop << "*** Energy Limit ***" << endl;
	fprop << " maxPG	=" << maxPG << endl;
	fprop << " maxAE	=" << maxAE << endl;
	fprop << " maxPF	=" << maxPF << endl;
	fprop << " maxAF	=" << maxAF << endl;
	fprop << " totAtom	=" << totAtom << endl;
	fprop << endl;
	fprop << "*** Time ***" << endl;
	fprop << " dt	=" << dt << endl;
	fprop << " maxstep	=" << maxstep << endl;
	fprop << " eps	=" << eps << endl;
	fprop << " div	=" << div << endl;
	fprop << " debug	=" << debug << endl;
	fprop << endl;
	fprop << "*** Input ***" << endl;
	fprop << " pump	=" << pump << endl;
	fprop << " detune	=" << detune << endl;
	fprop << " width	=" << width << endl;
	fprop << endl;
	fprop << "*** Interaction ***" << endl;
	fprop << " cohg	=" << cohg << endl;
	fprop << " cohf	=" << cohf << endl;
	fprop << endl;
	fprop << "*** Loss ***" << endl;
	fprop << " lossPG	=" << lossPG << endl;
	fprop << " lossPF	=" << lossPF << endl;
	fprop << " life	=" << life << endl;
	if(restarg.size()>0){
		fprop << "!!! unknown arg !!!" << endl;
		auto ite = restarg.begin();
		while(ite!=restarg.end()){
			fprop << " " << (*ite).first << "	=" << (*ite).second << endl;
			ite++;
		}
	}
	fprop.close();
}