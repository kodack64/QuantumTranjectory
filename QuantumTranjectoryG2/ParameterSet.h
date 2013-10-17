
#pragma once

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost;

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
	virtual int getParamInt(string str){
		return boost::lexical_cast<int>(param[str]);
	}
	virtual double getParamDouble(string str){
		return boost::lexical_cast<double>(param[str]);
	}
	virtual bool getParamBoolean(string str){
		return (param[str]=="1" || param[str]=="true");
	}
	virtual void setParam(string str,string val){
		param[str]=val;
	}
};