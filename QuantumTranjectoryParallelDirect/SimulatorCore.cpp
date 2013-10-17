
#include <iostream>
#include <fstream>
#include "Simulator.h"
#include "Random.h"

//#define DEBUG_OUT

void Simulator::runPrepareDebugOut(){
	fstream fcount;
	stringstream datafile;
	if(debug){
#pragma omp critical
		{
			int cc;
			fcount.open("count.txt",ios::in);
			if(!fcount){
				fcount.open("count.txt",ios::out);
				fcount << 0 << endl;
				fcount.close();
				cc=0;
			}else{
				fcount >> cc;
				fcount.close();
			}

			datafile << "./data/data" << cc << ".txt";

			fcount.open("count.txt",ios::out);
			fcount << cc+1;
			fcount.close();
		}

		fout.open(datafile.str(),ios::out);
		fout.precision(8);
		fout << "# 1-time(micro sec)  2-Energy 3-PhotonG 4-AtomE 5-PhotonF 6-AtomF 7-NonZeroProb 8-DetectProb 9-PhotonLoss"  << endl;
	}
}
void Simulator::runInitLoop(){
	// initialize

	if(noDetect==0){
		hitstep=maxstep+1;
		double ran = r->next();
		double psum=0;
		if(ran>=possum){
			hitstep=-1;
		}
		for(i=0;i<(signed)poslist.size();i++){
			psum+=poslist[i];
			if(ran<psum){
				hitstep=i;
				break;
			}
		}
		stringstream ss;
		ss << "./base/" << hitstep << ".txt";
		fstream baseRead(ss.str(),ios::in);
		if(baseRead){
			for(i=0;i<vecSize;i++){
				double re,im;
				baseRead >> re >> im;
				state[i]=complex<double>(re,im);
			}
			baseRead.close();
			step=hitstep-1;
		}else{
			for(i=0;i<vecSize;i++)state[i]=0;
			state[0]=1;
			step=-1;
		}
	}else{
		hitstep=maxstep+1;
		for(i=0;i<vecSize;i++)state[i]=0;
		state[0]=1;
		step=-1;
	}

	totalLossAE=0;
	lossTime.clear();

	poslosg=0;
	sum=totPG=totAE=totPF=totAF=totEN=trace=0;
}

void Simulator::runLoopLiouville(){
	rot = complex<double>(cos(step*dt*detune),sin(step*dt*detune));
	if(usePulse){
		if(dt*step<width){
			pulse=(-cos(dt*step/width*acos(0)*4)+1)/2;
		}else{
			pulse=0;
		}
	}else{
		pulse=1;
	}
	for(i=0;i<vecSize;i++)dif[i]=0;
	for(i=0;i<vecSize;i++){
		pg = getIdToPG(i);
		ae = getIdToAE(i);
		pf = getIdToPF(i);
		af = getIdToAF(i);

		// pump
		if(ae<maxAE){
			dif[i]+=img*pump*pulse*rot*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i+indAE];
		}
		if(ae>0){
			dif[i]+=img*pump*pulse*conj(rot)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i-indAE];
		}

		// coh e-g
		if(pg>0 && ae<maxAE){
			dif[i]+=img*cohg*sqrt(pg)*sqrt(ae+1)*sqrt(totAtom-ae-af)*state[i-indPG+indAE];
		}
		if(pg<maxPG && ae>0){
			dif[i]+=img*cohg*sqrt(pg+1)*sqrt(ae)*sqrt(totAtom-ae-af+1)*state[i+indPG-indAE];
		}

		// coh e-f
		if(pf>0 && af>0 && ae<maxAE){
			dif[i]+=img*cohf*sqrt(pf)*sqrt(af)*sqrt(ae+1)*state[i-indPF-indAF+indAE];
		}
		if(pf<maxPF && af<maxAF && ae>0){
			dif[i]+=img*cohf*sqrt(pf+1)*sqrt(af+1)*sqrt(ae)*state[i+indPF+indAF-indAE];
		}

	}
	for(i=0;i<vecSize;i++)state[i]+=dif[i]*dt;
}

void Simulator::runLoopLindblad(){
	// loss g photon
	if(noDetect>0){
		sum=0;
		for(i=0;i<vecSize;i++)dif[i]=0;
		for(i=0;i<vecSize;i++){
			ae = getIdToAE(i);
			sum+=norm(state[i])*sqrt(ae);
		}
		poslosg=sum*dt*life;
		flaghit=false;
	}else{
		if(step<hitstep){
			flaghit=false;
		}else if(step==hitstep){
			flaghit=true;
		}else{
			sum=0;
			for(i=0;i<vecSize;i++)dif[i]=0;
			for(i=0;i<vecSize;i++){
				ae = getIdToAE(i);
				sum+=norm(state[i])*sqrt(ae);
			}
			poslosg=sum*dt*life;
			flaghit = r->next()<poslosg;
		}
	}

	if(flaghit){
		totalLossAE++;
		lossTime.push_back(dt*step);
	}
	sum=0;

	if(flaghit){
		for(i=0;i<vecSize;i++)dif[i]=0;
		for(i=0;i<vecSize;i++){
			ae = getIdToAE(i);
			if(ae<maxAE){
				dif[i]=state[i+indAE]*sqrt(ae+1);
			}
		}
		for(i=0;i<vecSize;i++)state[i]=dif[i];
		for(i=0;i<vecSize;i++){
			pg = getIdToPG(i);
			pf = getIdToPF(i);
			af = getIdToAF(i);
			state[i]-=0.5*(af*life+pg*lossPG+pf*lossPF)*dt*state[i];
			sum+=norm(state[i]);
		}

	}else{

		for(i=0;i<vecSize;i++){
			pg = getIdToPG(i);
			ae = getIdToAE(i);
			pf = getIdToPF(i);
			af = getIdToAF(i);
			state[i]-=0.5*((ae+af)*life+pf*lossPF+pg*lossPG)*dt*state[i];
			sum+=norm(state[i]);
		}
	}
	sum=sqrt(sum);
	for(i=0;i<vecSize;i++)state[i]/=sum;

	if(noDetect>0)nodOut << step << " " <<  poslosg << endl;
}

void Simulator::runLoopLogging(){
	if(noDetect>0 && baseout>0){
		stringstream ss;
		ss << step;
		fstream fs("base\\"+ss.str()+".txt",ios::out);
		for(int i=0;i<vecSize;i++){
			fs << state[i].real() << " " << state[i].imag() << endl;
		}
		fs.close();
	}
	if(step%div==0){
		totPG=totAE=totPF=totAF=totEN=trace=0;
		for(i=0;i<=maxEne;i++)ene[i]=0;
		for(i=0;i<vecSize;i++){
			pg=getIdToPG(i);
			ae=getIdToAE(i);
			pf=getIdToPF(i);
			af=getIdToAF(i);
			val = norm(state[i]);

			totPG+=val*pg;
			totAE+=val*ae;
			totPF+=val*pf;
			totAF+=val*af;
			totEN+=val*(pg+ae+pf);
			trace+=val;
			ene[pg+ae+pf]+=val;
		}
#ifdef DEBUG_OUT
		if(debug){
			fout << dt*step << " " << totEN << " " << totPG
			<< " " << totAE << " " << totPF << " " << totAF
			<< " " << 1-ene[0] << " " << poslosg << " " << totalLossPG << endl;
	//		cout << dt*step << " " << totEN << " " << totPG
	//		<< " " << totAE << " " << totPF << " " << totAF
	//		<< " " << 1-ene[0] << " " << poslosg << " " << totalLossPG << endl;

	//		for(i=0;i<vecSize;i++){
	//			if(norm(state[i])>0){
	//			fout << "(" << getIdToPG(i) << "," << getIdToAE(i) << "," << getIdToPF(i) << "," << getIdToAF(i) << ")" << "=" << state[i] << endl;
	//			}
	//		}
		}
#endif DEBUG_OUT
	}
}

void Simulator::runLoop(){
	while( (dt*step<width || (1-ene[0])>eps) && step<maxstep && hitstep>=0){
		step++;
		runLoopLiouville();
		runLoopLindblad();
		runLoopLogging();
	}
}

void Simulator::runCloseLoop(){
	if(noDetect>0)nodOut.close();
}

int Simulator::run(){

#ifdef DEBUG_OUT
	runPrepareDebugOut();
#endif DEBUG_OUT

	runInitLoop();

	runLoop();

#ifdef DEBUG_OUT
	if(debug)fout.close();
#endif DEBUG_OUT

	runCloseLoop();

	return 0;
}
