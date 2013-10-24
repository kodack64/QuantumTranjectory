

#pragma once

#include <queue>

using namespace std;

class InteractiveInterpreter;
class ParameterSet;
class Command;

class Application{
private:
	int _argv;
	char** _argc;
	bool endFlag;
	ParameterSet* parameter;
	InteractiveInterpreter* inter;
	queue<Command*> commandArray;

	virtual void loadCommand();
	virtual void consumeCommand();
	virtual void waitCommand();

public:
	Application(int argv,char** argc){
		_argv=argv;
		_argc=argc;
	}
	virtual ~Application();
	virtual void start();
};