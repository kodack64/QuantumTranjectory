
#pragma once

#include <queue>
#include <string>
using namespace std;

class Command;

class InteractiveInterpreter{
private:
public:
	virtual void loadFile(queue<Command*>& com,string fileName);
	virtual void load(queue<Command*>& com,string command);
};