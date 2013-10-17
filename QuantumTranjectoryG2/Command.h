
#pragma once


#include <string>
using namespace std;

class Simulator;

class Command{
private:
public:
	virtual void execute(Simulator*)=0;
};

class ParameterFileSet : public Command{
private:
	string parameterFile;
public:
	virtual void execute(Simulator*);
};

class ParameterSet : public Command{
private:
	string targetName;
public:
	virtual void execute(Simulator*);
};
