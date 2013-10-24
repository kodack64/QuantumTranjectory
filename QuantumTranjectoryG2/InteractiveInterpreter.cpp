
#include <fstream>
#include <iostream>
using namespace std;

#include "InteractiveInterpreter.h"
#include "CommandFactory.h"

class Command;

void InteractiveInterpreter::load(queue<Command*>& coms,string command){
	Command* com = CommandFactory::createCommand(command);
	if(com!=NULL){
		coms.push(com);
	}
}

void InteractiveInterpreter::loadFile(queue<Command*>& coms,string file){
	ifstream ifs(file,ios::in);
	if(!ifs || ifs.eof()){
		//error
	}
	while(ifs && !ifs.eof()){
		string line;
		getline(ifs,line);
		cout << ">" << line << " @" << file << endl;
		this->load(coms,line);
	}
}
