
#pragma once

#include <queue>
#include <string>
using namespace std;

class FileStream;
class Command;

class InteractiveInterpreter{
private:
	virtual void execute(){};
	queue<Command*> commandArray;
public:
	virtual void loadFile(string fs);
	virtual void load(string command);
	virtual Command* pop();
};