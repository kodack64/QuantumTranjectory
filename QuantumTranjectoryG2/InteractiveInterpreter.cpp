
#include "InteractiveInterpreter.h"
#include "CommandFactory.h"
#include <fstream>

class Command;

void InteractiveInterpreter::load(string command){
	Command* com = CommandFactory::createCommand(command);
	if(com!=NULL){
		commandArray.push(com);
	}
}

void InteractiveInterpreter::loadFile(string file){
	ifstream ifs(file,ios::in);
	if(!ifs || ifs.eof()){
		//error
	}
	while(ifs && !ifs.eof()){
		string line;
		getline(ifs,line);
		this->load(line);
	}
}

Command* InteractiveInterpreter::pop(){
	Command* rtn = commandArray.front();
	commandArray.pop();
	return rtn;
}