
#pragma once

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
#include <fstream>
using namespace std;
using namespace boost;

class ParameterNotFound{};

class ParameterSet{
private:
	map <string,string> param;
public:
	virtual string getParam(string str,string def){
		if(param.count(str)>0){
			return param[str];
		}else{
			return def;
		}
	}
	virtual int getParamInt(string str,int def){
		int rtn=0;
		if(param.count(str)>0){
			try{
				rtn = boost::lexical_cast<int>(param[str]);
			}catch(boost::bad_lexical_cast e){
				rtn = def;
			}
		}else{
			rtn=def;
		}
		return rtn;
	}
	virtual double getParamDouble(string str,double def){
		double rtn=0;
		if(param.count(str)>0){
			try{
				rtn = boost::lexical_cast<double>(param[str]);
			}catch(boost::bad_lexical_cast e){
				rtn = def;
			}
		}else{
			rtn=def;
		}
		return rtn;
	}
	virtual bool getParamBoolean(string str,bool def){
		if(param.count(str)>0){
			return (param[str]=="1" || param[str]=="true");
		}else{
			return def;
		}
	}
	virtual void setParam(string str,string val){
		param[str]=val;
	}
	virtual void outputAllParameter(ostream &out){
		out << ">>> **********" << endl;
		auto ite = param.begin();
		while(ite!=param.end()){
			out << ">>> " << ite->first << " = " << ite->second << endl;
			ite++;
		}
		out << ">>> **********" << endl;
	}
};