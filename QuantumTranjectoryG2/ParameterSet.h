
#pragma once

#include <map>
#include <string>
using namespace std;

class ParameterNotFound{};

class ParameterSet{
private:
	map <string,string> param;
public:
	virtual string getParam(string str){
		if(param.find(str)!=param.end()){
			return param[str];
		}else{
			throw new ParameterNotFound();
		}
	}
	virtual void setParam(string str,string val){
		param[str]=val;
	}
};