
#pragma warning(disable : 4996)

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <omp.h>
#include <conio.h>

using namespace std;
using namespace boost;

#include "Simulator.h"


int main(int argc,char** argv){

	int corenum=0;
	int totalTask=1;
	string baseText="";
	vector< pair<string,string> > arg;
	for(int cc=1;cc<argc;cc++){
		string temp = argv[cc];
		vector<string> keyval;
		split(keyval,temp,is_any_of("="));
		if(keyval.size()>=2){
			if(keyval[0]=="base"){
				baseText = keyval[1];
				fstream bs(baseText,ios::in);
				if(!bs){
					cout << keyval[1] << "not found!" << endl;
				}else{
					cout << "load <- " << keyval[1] << endl;
				}
				while(bs && !bs.eof()){
					string str;
					vector<string> vstr;
					getline(bs,str);
					split(vstr,str,is_any_of("="));
					if(vstr.size()>=2){
						arg.push_back(make_pair(vstr[0],vstr[1]));
					}
				}
				if(bs)bs.close();
			}
			else{
				arg.push_back(make_pair(keyval[0],keyval[1]));
			}
		}
	}
	auto ite = arg.begin();
	while(ite!=arg.end()){
		if((*ite).first=="core"){corenum = lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="task"){totalTask = lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else ite++;
	}

	for(unsigned int cc=0;cc<arg.size();cc++){
		if(arg[cc].first=="core"){
			corenum = lexical_cast<int>(arg[cc].second);
		}
		else if(arg[cc].first=="task"){
			totalTask = lexical_cast<int>(arg[cc].second);
		}
	}

	int totalEndTask=0;
	int totalStartTask=0;

	if(corenum!=0){
		omp_set_num_threads(corenum);
	}

	ifstream filecheck("detectpos.txt");
	if(!filecheck){
		cout << "	!precalc!" << endl;
		Simulator* psim = new Simulator();
		arg.push_back(make_pair("noDetect","1"));
		psim->init(arg);
		arg.push_back(make_pair("noDetect","0"));
		psim->run();
		delete psim;
		cout << "	detection possibility -> detectpos.txt" << endl;
	}else{
		cout << "	!precalc ok" << endl;
	}

	auto totalStartPoint = chrono::high_resolution_clock::now();
	cout << "***start : #core = " << omp_get_max_threads() 
		<< " #task = " << totalTask << endl;


#pragma omp parallel
	{
		cout << "#core = " << omp_get_thread_num() << " init" << endl;
		int myOwnTaskId;
		int myTotalTaskId=0;
		chrono::system_clock::time_point myStart,myEnd;
		long long myTime,myTotalTime;

		Simulator* sim = new Simulator();
		sim->init(arg);
		if(omp_get_thread_num()==0){
			sim->outputProperty();
		}

#pragma omp critical (output)
		{
			myOwnTaskId=0;
			myTotalTaskId=totalStartTask;
			totalStartTask++;
		}

		cout << "#core = " << omp_get_thread_num() << " start" << endl;
		while(myTotalTaskId<totalTask){

			myStart = chrono::high_resolution_clock::now();
			sim->run();
			myEnd = chrono::high_resolution_clock::now();
			myTime = chrono::duration_cast<chrono::milliseconds>(myEnd-myStart).count();
			myTotalTime = chrono::duration_cast<chrono::seconds>(myEnd-totalStartPoint).count();

#pragma omp critical (output)
			{
				totalEndTask++;

				sim->output();
				cout << omp_get_thread_num() << " : " << totalEndTask << "/" << totalTask << " count = " << sim->getTotalLossPG() << " time = " << myTime << "ms  : totalTime = " << myTotalTime << "s" << endl;

				myTotalTaskId=totalStartTask;
				totalStartTask++;
				myOwnTaskId++;
			}
		}
		sim->close();
		delete sim;
	}
	auto totalEndPoint = chrono::high_resolution_clock::now();
	long long totalTime = chrono::duration_cast<chrono::milliseconds>(totalEndPoint-totalStartPoint).count(); 
	cout << "***end	time = " << totalTime << "ms" << " ( " << totalTime/totalTask << " ms per task )" << endl;

	return 0;
}