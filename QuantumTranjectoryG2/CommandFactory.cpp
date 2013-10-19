
#pragma warning(disable : 4290)

#include "CommandFactory.h"
#include <functional>
#include <boost\algorithm\string.hpp>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;

class BadCommand{
public:
	BadCommand(string str):message(str){
	}
	string message;
};

Command* CommandFactory::createCommand(string line) throw (BadCommand){
	vector <string> words;

	map<string,function<Command*(vector<string>)> > funcmap;

	funcmap.insert(make_pair("set",[](vector<string> words){
		return new CommandSetParameter(words.at(1),words.at(2));
	}));
	funcmap.insert(make_pair("load",[](vector<string> words){
		return new CommandSetParameterFile(words.at(1));
	}));
	funcmap.insert(make_pair("execute",[](vector<string> words){
		return new CommandExecute(words.at(1));
	}));
	funcmap.insert(make_pair("repeat",[](vector<string> words){
		return new CommandRepeat(words.at(1),words.at(2));
	}));

	boost::algorithm::split(words,line,boost::is_space());

	if(words.size()==0)return NULL;
	else{
		if(funcmap.find(words[0])!= funcmap.end()){
			try{
				return funcmap[words[0]](words);
			}catch(BadCommand& bd){
				throw bd;
			}
		}else{
			throw new BadCommand("message is empty");
		}
	}
}