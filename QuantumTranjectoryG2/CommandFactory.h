
#pragma once

#include "Command.h"
#include <map>
#include <string>
#include <vector>
#include <functional>
using namespace std;

class CommandFactory{
private:
	static void setCommandFunc();
	static map<string,std::function<Command*(vector<string>)> > CommandFactory::funcmap;
public:
	static Command* createCommand(string line);
};