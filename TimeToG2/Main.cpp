
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <conio.h>

using namespace std;

int main(int  argc,char** argv){
	string file;
	double div;
	if(argc>=2){
		file = argv[1];
	}else{
		cout << "file>" ;
		cin >> file;
	}
	ifstream ifs;
	ofstream ofs;
	ifs.open(file);
	vector <double> times;
	vector <int> hist;
	vector<double> g2;
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

	ofs.open((file+"out.txt").c_str(),ios::out);
	for(int i=0;i<g2.size();i++){
		ofs << div*i << " " << g2[i] << endl;
	}
	ofs.close();

	FILE* fp = _popen("gnuplot.exe","w");
	fprintf(fp,"plot \"");
	fprintf(fp,(file+"out.txt").c_str());
	fprintf(fp,"\" w l \n");
	fflush(fp);
	_getch();
	fclose(fp);
	return 0;
}