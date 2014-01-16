
#pragma once

#include <string>
#include <vector>

using namespace std;

class DistributionToTime{
private:
	double power;
	double div;
	vector<double> counts;
	int loop;
public:
	virtual void setPower(double _power,double _div,double _loop){power=_power;div=_div;loop=_loop;};
	virtual void make();
	virtual void timeDif(double dif,double coh);
	virtual void output(string file);
};