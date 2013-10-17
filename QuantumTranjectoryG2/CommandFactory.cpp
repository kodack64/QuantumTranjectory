
#include "CommandFactory.h"
#include <functional>
#include <boost\algorithm\string.hpp>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;

Command* CommandFactory::createCommand(string line){
	vector <string> words;
	map<string,function<Command*(vector<string>)> > funcmap;
	funcmap.insert(make_pair("set",[](vector<string> words){
		return new CommandSetParameter(words[1],words[2]);
	}));
	funcmap.insert(make_pair("load",[](vector<string> words){
		return new CommandSetParameterFile();
	}));
	funcmap.insert(make_pair("execute",[](vector<string> words){
		return new CommandExecute();
	}));

	boost::algorithm::split(words,line,boost::is_space());
	if(words.size()==0)return NULL;
	else{
		if(funcmap.find(words[0])!= funcmap.end()){
			return funcmap[words[0]](words);
		}else{
			return NULL;
		}
	}
}