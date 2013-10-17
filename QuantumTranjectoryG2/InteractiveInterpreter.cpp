
#include "InteractiveInterpreter.h"
#include "CommandFactory.h"
#include "Command.h"
#include <fstream>

void InteractiveInterpreter::load(string command){
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