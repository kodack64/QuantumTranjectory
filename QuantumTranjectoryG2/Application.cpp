
#include "Application.h"
#include <iostream>

void Application::start(){
	InteractiveInterpreter* inter = new InteractiveInterpreter();
	for(int i=1;i<_argv;i++){
		inter->loadFile(_argc[i]);
	}
	while(true){
		string command;
		getline(cin,command);
		inter->load(command);
	}
	delete inter;
}