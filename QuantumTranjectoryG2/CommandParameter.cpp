
#pragma warning(disable : 4819)

#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"
#include "CommandFactory.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <Windows.h>
using namespace std;

// 値の設定
void CommandSetParameter::execute(ParameterSet* par,queue<Command*>& coms){
	par->setParam(_paramName,_value);
	cout << "# set " << _paramName << " -> " << _value << endl;
}

// 値の加減
void CommandSetDifParameter::execute(ParameterSet* par,queue<Command*>& coms){
	try{
		if(_type=="int"){
			int val = boost::lexical_cast<int>(par->getParamInt(_paramName,0));
			int dif = boost::lexical_cast<int>(_value);
			par->setParam(_paramName,boost::lexical_cast<string>(val+dif));
			cout << "# difset " << _paramName << " " << val << " -> " << val+dif << endl;
		}else if(_type=="double"){
			double val = boost::lexical_cast<double>(par->getParamDouble(_paramName,0));
			double dif = boost::lexical_cast<double>(_value);
			par->setParam(_paramName,boost::lexical_cast<string>(val+dif));
			cout << "# difset " << _paramName << " " << val << " -> " << val+dif << endl;
		}else{
			cout << "# unknown type " << _type <<endl;
		}
	}catch(boost::bad_lexical_cast e){
		cout << "# cannot cast param : " << _paramName << " or " << _value << endl;
	}
}

// 設定値の表示
void CommandDescParameter::execute(ParameterSet* par,queue<Command*>& coms){
	cout << par->toString();
}

// パラメータファイルのロード
void CommandSetParameterFile::execute(ParameterSet* par,queue<Command*>& coms){
	ifstream ifs(_fileName,ios::in);
	if(!ifs){
		cout << ">> file not found" << endl;
	}
	while(ifs && !ifs.eof()){
		string line;
		vector<string> stringArray;
		getline(ifs,line);
		boost::algorithm::split(stringArray,line,boost::is_space());
		if(stringArray.size()<=1)continue;
		if(stringArray[0][0]=='#')continue;
		par->setParam(stringArray[0],stringArray[1]);
		cout << "# set " << stringArray[0] << " " << stringArray[1] << " @" << _fileName << endl; 
	}
}


// コマンドファイルのロード
void CommandScript::execute(ParameterSet* par,queue<Command*>& coms){
	int i_repeatNum;
	try{
		i_repeatNum = boost::lexical_cast<int>(_repeatNum);
	}catch(boost::bad_lexical_cast e){
		i_repeatNum=1;
	}
	ifstream ifs(_scriptFile,ios::in);
	if(!ifs || ifs.eof()){
		cout << "# file not found" << endl;
		return;
	}
	ifs.close();
	for(int i=0;i<i_repeatNum;i++){
		ifs.open(_scriptFile,ios::in);
		string line;
		while(ifs && !ifs.eof()){
			getline(ifs,line);
			if(line.length()>0){
				Command* com =CommandFactory::createCommand(line);
				if(com!=NULL){
					cout << "#   add queue : " << line << endl;
					coms.push(com);
				}
			}
		}
		ifs.close();
	}
}

