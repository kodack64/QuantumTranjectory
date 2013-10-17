
#include "Application.h"

#include <iostream>
#include "InteractiveInterpreter.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "Command.h"

using namespace std;

Application::~Application(){
	delete inter;
	delete simulator;
	delete parameter;
}

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
		inter->loadFile(commandArray,_argc[i]);
	}
}

void Application::consumeCommand(){
	Command* com;
	while(commandArray.empty()){
		com = commandArray.front();
		if(com!=NULL){
			com->execute(simulator,parameter,commandArray);
			delete com;
		}else{
			break;
		}
		commandArray.pop();
	}
}

void Application::waitCommand(){
	string command;
	getline(cin,command);
	inter->load(commandArray,command);
}
