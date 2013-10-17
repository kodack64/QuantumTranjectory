

#pragma once

class InteractiveInterpreter;
class Simulator;
class ParameterSet;

class Application{
private:
	int _argv;
	char** _argc;
	Simulator* simulator;
	ParameterSet* parameter;
	InteractiveInterpreter* inter;

	virtual void loadCommand();
	virtual void consumeCommand();
	virtual void waitCommand();

public:
	Application(int argv,char** argc){
		_argv=argv;
		_argc=argc;
	}
	virtual void start();
};