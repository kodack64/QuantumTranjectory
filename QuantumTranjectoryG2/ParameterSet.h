
#pragma once

#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
using namespace std;
using namespace boost;

class ParameterNotFound{};

class ParameterSet{
private:
	map <string,string> param;
public:
	ParameterSet(){
		param.clear();

		param["unit"] = "0";

		param["maxPG"]="4";
		param["maxAE"]="4";
		param["maxPF"]="4";
		param["maxAF"]="4";
		param["totAtom"]="100";

		param["useLossProbe"]="true";
		param["useLossControl"]="true";
		param["useLossAtom"]="true";
		param["usePulse"]="false";

		param["dt"]="1e-2";
		param["maxstep"]="10000000";

		param["loggingUnit"] = "1000";
		param["loggingTime"] = "true";
		param["loggingProb"] = "true";
		param["loggingJump"] = "true";

		param["pulsePump"] = "1.5e-1";
		param["pulseDetune"] = "0e-1";
		param["pulseWidth"] = "1e1";

		param["coherenceProbe"] = "1e-1";
		param["coherenceControl"]  ="1e-0";

		param["lossProbe"] = "3e-2";
		param["lossControl"] ="3e-2";
		param["lossAtom"] = "3e-0";
		param["eps"] = "1e-3";

		param["postselProbe"]="false";
		param["postselControl"]="false";
		param["postselAtom"]="false";

		param["forceLossProbeTime"]="-1";

		param["core"] = "8";
		
	}

	virtual string getParam(string str,string def){
		if(param.count(str)>0){
			return param[str];
		}else{
			cout << " *** Undefined Parameter : " << str << endl;
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
			cout << " *** Undefined Parameter : " << str << endl;
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
			cout << " *** Undefined Parameter : " << str << endl;
			rtn=def;
		}
		return rtn;
	}
	virtual bool getParamBoolean(string str,bool def){
		if(param.count(str)>0){
			return (param[str]=="1" || param[str]=="true");
		}else{
			cout << " *** Undefined Parameter : " << str << endl;
			return def;
		}
	}
	virtual void setParam(string str,string val){
		if(param.count(str)==0){
			cout << " *** New Parameter : " << str << " <- " << val << endl;
		}else{
			cout << " *** Update Parameter : " << str << " : " << param[str] << " -> " << val << endl;
		}
		param[str]=val;
	}
	virtual void outputAllParameter(ostream &out){
		out << "#   **********" << endl;
		auto ite = param.begin();
		while(ite!=param.end()){
			out << "#   " << ite->first << " = " << ite->second << endl;
			ite++;
		}
		out << "#   **********" << endl;
	}
};