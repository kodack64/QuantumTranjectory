
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


class CommandDifSetParameter : public Command{
private:
	string _paramName;
	string _value;
	string _type;
public:
	CommandDifSetParameter(string paramName,string value,string type)
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



