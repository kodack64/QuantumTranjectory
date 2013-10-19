
#pragma warning(disable:4819)

#include "Application.h"

#include <iostream>
using namespace std;

#include "InteractiveInterpreter.h"
#include "ParameterSet.h"
#include "Command.h"

Application::~Application(){
	delete inter;
	delete parameter;
}

void Application::start(){
	inter = new InteractiveInterpreter();
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
			com->execute(parameter,commandArray);
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
