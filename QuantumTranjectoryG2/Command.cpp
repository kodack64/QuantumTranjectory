
#include "Command.h"
#include "Simulator.h"
#include "ParameterSet.h"

void CommandSetParameter::execute(Simulator* sim,ParameterSet* par){
	sim->execute(par);
}

void CommandSetParameterFile::execute(Simulator* sim,ParameterSet* par){
}

void CommandExecute::execute(Simulator* sim,ParameterSet* par){
}
