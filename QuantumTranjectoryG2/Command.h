
#pragma once


#include <string>
#include <queue>
using namespace std;

class ParameterSet;

class Command{
private:
public:
	virtual void execute(ParameterSet*,queue<Command*>&)=0;
};

class CommandSetParameterFile : public Command{
private:
	string _fileName;
public:
	CommandSetParameterFile(string fileName)
		:_fileName(fileName)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandSetParameter : public Command{
private:
	string _paramName;
	string _value;
public:
	CommandSetParameter(string paramName,string value)
		:_paramName(paramName)
		,_value(value)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};
class CommandExecute : public Command{
private:
	string _executeNum;
public:
	CommandExecute(string executeNum)
	:_executeNum(executeNum)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandScript : public Command{
private:
	string _repeatNum;
	string _scriptFile;
public:
	CommandScript(string scriptFile,string repeatNum)
	:_repeatNum(repeatNum)
	,_scriptFile(scriptFile){}

	virtual void execute(ParameterSet*,queue<Command*>&) override;
};


class CommandSetDifParameter : public Command{
private:
	string _paramName;
	string _value;
	string _type;
public:
	CommandSetDifParameter(string paramName,string value,string type)
		:_paramName(paramName)
		,_value(value)
		,_type(type)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandDescParameter : public Command{
private:
public:
	CommandDescParameter()
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandCalcHistgram : public Command{
private:
	string unit;
	string name;
	string sdt;
	string ssig;
public:
	CommandCalcHistgram(string _unit , string _dt,string _sig)
	:unit(_unit)
	,sdt(_dt)
	,ssig(_sig)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandCalcPower : public Command{
private:
	string unit;
public:
	CommandCalcPower(string _unit)
	:unit(_unit)
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};

class CommandCalcG2 : public Command{
private:
	virtual void calcG2(string base,string jump,string out,int jumpStep,double logDiv,int i_unit,string name);
public:
	CommandCalcG2()
	{}
	virtual void execute(ParameterSet*,queue<Command*>&) override;
};


