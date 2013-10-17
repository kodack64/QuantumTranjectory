
#pragma once

#include <vector>
#include <string>
using namespace std;

class FileStream;

class InteractiveInterpreter{
private:
	virtual void execute(){};
public:
	virtual void loadFile(string fs);
	virtual void load(string command);
};