

#pragma once

#include "InteractiveInterpreter.h"

class Simulator;

class Application{
private:
	int _argv;
	char** _argc;
	Simulator* simulator;
public:
	Application(int argv,char** argc){
		_argv=argv;
		_argc=argc;
	}
	virtual void start();
};