
#pragma warning(disable : 4819)

#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "CommandFactory.h"
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

void CommandSetParameter::execute(ParameterSet* par,queue<Command*> coms){
	par->setParam(_paramName,_value);
}

void CommandSetParameterFile::execute(ParameterSet* par,queue<Command*> coms){
	ifstream ifs(_fileName,ios::in);
	if(!ifs || ifs.eof()){
		return;
	}else{
		string line;
		vector<string> stringArray;
		getline(ifs,line);
		boost::algorithm::split(stringArray,line,boost::is_space());
		if(stringArray.size()<=1)return;
		if(stringArray[0][0]=='#')return;
		par->setParam(stringArray[0],stringArray[1]);
	}
}

void CommandExecute::execute(ParameterSet* par,queue<Command*> coms){

	int i_repeatNum = boost::lexical_cast<int>(_executeNum);

#ifdef _OPENMP
	int count=0;
#pragma omp parallel
	{
		Simulator* sim = new Simulator();
		while(true){
#pragma omp atomic
			{
				if(count>=i_repeatNum)break;
				else count++;
			}
			sim->execute(par);
		}
		delete sim;
	}

#else

	Simulator* sim = new Simulator();
	for(int i=0;i<i_repeatNum;i++){
		sim->execute(par);
	}
	delete sim;

#endif
}

void CommandRepeat::execute(ParameterSet* par,queue<Command*> coms){
	int i_repeatNum = boost::lexical_cast<int>(_repeatNum);
	for(int i=0;i<i_repeatNum;i++){
		string command = "load " + _scriptFile ;
		CommandFactory::createCommand(command);
	}
}