
#pragma once

#include <string>
#include <vector>

using namespace std;

class TimeToG2{
private:
	string inputFile;
	double div;
	vector <double> times;
	vector <int> hist;
	vector<double> g2;

public:
	virtual void setInput(string file);
	virtual void calc();
	virtual void output(string out);
	virtual void plot(string out);
	virtual void plot(string out,string dif);
};