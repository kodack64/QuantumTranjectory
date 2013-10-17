
#pragma once

#include "Command.h"
#include <string>
using namespace std;

class CommandFactory{
private:
public:
	static Command* createCommand(string line);
};