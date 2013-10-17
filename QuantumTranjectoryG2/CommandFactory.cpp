
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
	function< Command* (vector<string>) > dot = [] ( vector<string> words ) { return NULL; };

	map<string,function<Command*(vector<string>)> > funcmap;
	funcmap.insert.make_pair("set",<>()());

	algorithm::split(words,line,boost::is_space());
	Command* com;
	if(words.size()==0)return __nullptr;
	switch(words[0]){
	case "set":
		break;
	case "load":
		break;
	case "execute":
		break;
	default:
		// error
	}
}