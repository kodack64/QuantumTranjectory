
#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "CommandFactory.h"
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

void CommandSetParameter::execute(Simulator* sim,ParameterSet* par,queue<Command*> coms){
	par->setParam(_paramName,_value);
}

void CommandSetParameterFile::execute(Simulator* sim,ParameterSet* par,queue<Command*> coms){
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

void CommandExecute::execute(Simulator* sim,ParameterSet* par,queue<Command*> coms){
	sim->execute(par);
}

void CommandRepeat::execute(Simulator* sim,ParameterSet* par,queue<Command*> coms){
	int i_repeatNum = boost::lexical_cast<int>(_repeatNum);
	for(int i=0;i<i_repeatNum;i++){
		string command = "load " + _scriptFile ;
		CommandFactory::createCommand(command);
	}
}