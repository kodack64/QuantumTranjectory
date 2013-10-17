#include "Simulator.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Random.h"
#include <omp.h>

using namespace boost;


Simulator::Simulator()
:r(__nullptr)
,state(__nullptr)
,dif(__nullptr)
,ene(__nullptr)
{
}

int Simulator::init(vector<pair<string,string> > arg){
	// state limit
	maxPG=4;
	maxAE=4;
	maxPF=0;
	maxAF=0;
	totAtom=10;

	// use flag
	useLossPG=true;
	useLossAE=true;
	useLossPF=true;
	useLossAF=true;
	usePulse=true;

	//	time
	dt=1e-4;
	div=1000;
	maxstep=100000000;

	//	input
	pump=1.5e-1;
	detune=0e-1;
	width=1e1;

	//	coherence
	cohg=1e-1;
	cohf=0e-0;

	//	lifetime
	lossPG=3e-1;
	lossPF=3e-1;
	life=6e-3;

	// end condition
	eps=1e-3;
	seed=(int)time(NULL);

	// predetection
	noDetect=0;
	baseout=0;

	// setup
	auto ite = arg.begin();
	while(ite!=arg.end()){
		if((*ite).first=="maxPG"){maxPG=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="maxAE"){maxAE=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="maxPF"){maxPF=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="maxAF"){maxAF=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="totAtom"){totAtom=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="dt"){dt=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="div"){div=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="maxstep"){maxstep=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="pump"){pump=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="detune"){detune=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="width"){width=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="cohg"){cohg=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="cohf"){cohf=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="cohg"){cohg=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="lossPG"){lossPG=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="lossPF"){lossPF=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="life"){life=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="eps"){eps=lexical_cast<double>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="debug"){debug=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="noDetect"){noDetect=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else if((*ite).first=="baseout"){baseout=lexical_cast<int>((*ite).second);ite=arg.erase(ite);}
		else {restarg.push_back(*ite); ite++;}
	}

	// no change
	indPG=1;
	indAE=indPG*(maxPG+1);
	indPF=indAE*(maxAE+1);
	indAF=indPF*(maxPF+1);
	vecSize=indAF*(maxAF+1);
	maxEne=maxPG+maxAE+maxPF;
	img=complex<double>(0,1);

	// memory allocate
	r=new Random(seed);
	state= new complex<double>[vecSize];
	dif= new complex<double>[vecSize];
	ene = new double[maxEne+1];

	// setting
	cout.precision(3);

	if(noDetect>0)nodOut.open("detectpos.txt",ios::out);
	else{
		possum=0;
		double npo=1;
		nodOut.open("detectpos.txt",ios::in);
		while(nodOut && !nodOut.eof()){
			double st,po;
			nodOut >> st >> po;
			poslist.push_back(npo*po);
			possum+=npo*po;
			npo*=(1-po);
		}
	}
	return 0;
}