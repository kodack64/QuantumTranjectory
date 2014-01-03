
#include <fstream>
#include <iostream>
using namespace std;

#include "InteractiveInterpreter.h"
#include "CommandFactory.h"

class Command;

// コマンドを読んで正常なコマンドならリストに追加
void InteractiveInterpreter::load(queue<Command*>& coms,string command){
	Command* com = CommandFactory::createCommand(command);
	if(com!=NULL){
		coms.push(com);
	}
}

//コマンドファイルを読んで行ごとに正常なコマンドならリストに追加
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
