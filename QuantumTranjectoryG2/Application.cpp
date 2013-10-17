
#include "Application.h"

#include <iostream>
#include "InteractiveInterpreter.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "Command.h"

void Application::start(){
	inter = new InteractiveInterpreter();
	simulator = new Simulator();
	parameter = new ParameterSet();
	loadCommand();
	while(true){
		consumeCommand();
		waitCommand();
	}
	delete inter;
}

void Application::loadCommand(){
	for(int i=1;i<_argv;i++){
		inter->loadFile(_argc[i]);
	}
}
void Application::consumeCommand(){
	Command* com = inter->pop();
	com->execute(simulator,parameter);
}
void Application::waitCommand(){
	string command;
	getline(cin,command);
	inter->load(command);
}