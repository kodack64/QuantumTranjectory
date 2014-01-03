
#pragma warning(disable:4819)

#include "Application.h"

#include <iostream>
using namespace std;

#include "InteractiveInterpreter.h"
#include "ParameterSet.h"
#include "Command.h"

// コマンドインタプリタの初期化と入力の待機
void Application::start(){
	inter = new InteractiveInterpreter();
	parameter = new ParameterSet();
	loadCommand();
	endFlag=false;
	while(!endFlag){
		consumeCommand();
		waitCommand();
	}
	delete inter;
	delete parameter;
}

//ドロップされたコマンドファイルを読む
void Application::loadCommand(){
	for(int i=1;i<_argv;i++){
		inter->loadFile(commandArray,_argc[i]);
	}
}

//積まれたコマンドを全て実行
void Application::consumeCommand(){
	Command* com;
	while(!commandArray.empty()){
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

//コンソールからのコマンド入力を待機
void Application::waitCommand(){
	string command;
	cout << ">";
	getline(cin,command);
	if(command=="exit")endFlag=true;
	else inter->load(commandArray,command);
}
