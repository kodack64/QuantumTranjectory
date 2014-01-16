
#include "TimeToG2.h"
#include <boost\lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <conio.h>

void TimeToG2::setInput(string _file){
	inputFile = _file;
}

void TimeToG2::calc(){
	ifstream ifs;
	ifs.open(inputFile);

	double maxTime=0;
	while(ifs){
		string line;
		getline(ifs,line);
		if(line.length()>0){
			double val = boost::lexical_cast<double>(line);
			times.push_back(val);
			if(maxTime<val)maxTime=val;
		}
	}

	cout << "maxtime:" << maxTime << endl;
	cout << "size:" << times.size() << endl;

	cout << "div>";
	cin >> div;

	ifs.close();
	hist.resize((int)(maxTime/div)+1);
	g2.resize((int)(maxTime/div)+1);
	for(int i=0;i<hist.size();i++)hist[i]=g2[i]=0;
	for(int i=0;i<times.size();i++){
		hist[(int)(times[i]/div)]++;
	}
	for(int i=0;i<g2.size();i++){
		double sumr=0;
		double suml=0;
		int count = hist.size()-i;
		for(int j=0;j<count;j++){
			if(i==0){
				g2[i]+=hist[j]*(hist[j]-1);
			}else{
				g2[i]+=hist[j]*hist[j+i];
			}
			sumr+=hist[j];
			suml+=hist[j+i];
		}
		sumr/=count;
		suml/=count;
		g2[i]/=count;
		g2[i] = g2[i]/(sumr*suml);
	}
}
void TimeToG2::output(string outFile){
	ofstream ofs(outFile,ios::out);
	if(ofs){
		for(int i=0;i<g2.size();i++){
			ofs << div*i << " " << g2[i] << endl;
		}
		ofs.close();
	}
}

void TimeToG2::plot(string outFile){
	FILE* fp = _popen("gnuplot.exe","w");
	fprintf(fp,"plot \"");
	fprintf(fp,outFile.c_str());
	fprintf(fp,"\" w l \n");
	fflush(fp);
	_getch();
	fclose(fp);
}
void TimeToG2::plot(string outFile,string difFile){
	FILE* fp = _popen("gnuplot.exe","w");
	fprintf(fp,"set yrange[0:2]\n ");

	fprintf(fp,"plot \"");
	fprintf(fp,outFile.c_str());
	fprintf(fp,"\" w l \n");

	fprintf(fp,"replot \"");
	fprintf(fp,difFile.c_str());
	fprintf(fp,"\" w l \n");

	fflush(fp);
	_getch();
	fclose(fp);
}
