
#pragma warning(disable : 4290)

#include "CommandFactory.h"
#include <boost\algorithm\string.hpp>
#include <iostream>

using namespace std;

map<string,function<Command*(vector<string>)> > CommandFactory::funcmap;

// コマンドと必要な引数のリストを設定
void CommandFactory::setCommandFunc(){
	funcmap.insert(make_pair("set",[](vector<string> words){
		if(words.size()==3){
			return (Command*)(new CommandSetParameter(words.at(1),words.at(2)));
		}else{
			cout << "# format : set <parameter name> <parameter value>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("setdif",[](vector<string> words){
		if(words.size()==4){
			return (Command*)(new CommandSetDifParameter(words.at(1),words.at(2),words.at(3)));
		}else{
			cout << "# format : setdif <parameter name> <parameter dif value> <parameter type>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("desc",[](vector<string> words){
		return (Command*)(new CommandDescParameter());
	}));
	funcmap.insert(make_pair("load",[](vector<string> words){
		if(words.size()==2){
			return (Command*)(new CommandSetParameterFile(words.at(1)));
		}else{
			cout << "# format : load <parameter file>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("execute",[](vector<string> words){
		if(words.size()==2){
			return (Command*)(new CommandExecute(words.at(1)));
		}else{
			cout << "# format : execute <repeat number>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("script",[](vector<string> words){
		if(words.size()==3){
			return (Command*)(new CommandScript(words.at(1),words.at(2)));
		}else{
			cout << "# format : script <script file> <repeat number>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("histgram",[](vector<string> words){
		if(words.size()==4){
			return (Command*)(new CommandCalcHistgram(words.at(1),words.at(2),words.at(3)));
		}else{
			cout << "# format : histgram <log unit number> <histgram delta time> <photon dist sigma>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("power",[](vector<string> words){
		if(words.size()==2){
			return (Command*)(new CommandCalcPower(words.at(1)));
		}else{
			cout << "# format : power <log unit number>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("g2",[](vector<string> words){
		if(words.size()==1){
			return (Command*)(new CommandCalcG2());
		}else{
			cout << "# format : g2" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("1pulse", [](vector<string> words){
		if (words.size() == 2){
			return (Command*)(new CommandCalc1Pulse(words.at(1)));
		}
		else{
			cout << "# format : 1pulse <time>" << endl;
			return (Command*)NULL;
		}
	}));
	funcmap.insert(make_pair("2pulse", [](vector<string> words){
		if (words.size() == 3){
			return (Command*)(new CommandCalc2Pulse(words.at(1),words.at(2)));
		}
		else{
			cout << "# format : 2pulse <time1> <time2>" << endl;
			return (Command*)NULL;
		}
	}));
}

// コマンドの生成
Command* CommandFactory::createCommand(string line){

	vector <string> words;

	if(funcmap.empty())setCommandFunc();

	// コマンドの分割
	boost::algorithm::split(words,line,boost::is_space());

	//空行でなければマッチするコマンドを返す
	if(words.size()==0)return NULL;
	if(funcmap.find(words[0])!= funcmap.end()){
		return funcmap[words[0]](words);
	}else{
		cout << "# invalid command" << endl;
		cout << "# commands list:" << endl;
		auto ite = funcmap.begin();
		while(ite!=funcmap.end()){
			cout << "#  " << ite->first << endl;
			ite++;
		}
		cout << "#  exit" << endl;
		return NULL;
	}
}