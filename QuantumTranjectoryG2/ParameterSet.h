
#pragma once

#include <map>
#include <string>
using namespace std;

class ParameterSet{
private:
	map <string,int> intParam;
	map <string,double> doubleParam;
public:
	virtual int getIntParam(string str){
		if(intParam.find(str)!=intParam.end()){
			return intParam[str];
		}
		return -1;
	}
	virtual double getDoubleParam(string str){
		if(doubleParam.find(str)!=doubleParam.end()){
			return doubleParam[str];
		}
		return -1;
	}
	virtual void setParam(string str,double val){
		doubleParam[str]=val;
	}
	virtual void setParam(string str,int val){
		intParam[str]=val;
	}
};