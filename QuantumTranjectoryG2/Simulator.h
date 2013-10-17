
#pragma once

class ParameterSet;

class Simulator{
public:
	virtual void execute(ParameterSet* param);
	virtual void executeNewThread(ParameterSet* param);
};