
#pragma warning(disable:4819)

#include "Application.h"

#include <iostream>
using namespace std;

#include "InteractiveInterpreter.h"
#include "ParameterSet.h"
#include "Command.h"

// �R�}���h�C���^�v���^�̏������Ɠ��͂̑ҋ@
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

//�h���b�v���ꂽ�R�}���h�t�@�C����ǂ�
void Application::loadCommand(){
	for(int i=1;i<_argv;i++){
		inter->loadFile(commandArray,_argc[i]);
	}
}

//�ς܂ꂽ�R�}���h��S�Ď��s
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

//�R���\�[������̃R�}���h���͂�ҋ@
void Application::waitCommand(){
	string command;
	cout << ">";
	getline(cin,command);
	if(command=="exit")endFlag=true;
	else inter->load(commandArray,command);
}
