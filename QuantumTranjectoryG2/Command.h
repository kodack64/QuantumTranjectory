
#pragma once


#include <string>
using namespace std;

class Simulator;
class ParameterSet;

class Command{
private:
public:
	virtual void execute(Simulator*,ParameterSet*)=0;
};

class CommandSetParameterFile : public Command{
private:
	string _fileName;
public:
	CommandSetParameterFile(string fileName)
		:_fileName(fileName)
	{}
	virtual void execute(Simulator*,ParameterSet*);
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
	virtual void execute(Simulator*,ParameterSet*);
};
class CommandExecute : public Command{
private:
public:
	CommandExecute()
	{}
	virtual void execute(Simulator*,ParameterSet*);
};