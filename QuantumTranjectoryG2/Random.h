
#pragma once

#include <random>

class Random{
public:
	std::random_device rd;
	std::mt19937_64 mt;
	std::uniform_real_distribution<double> dis;
	Random(){
		mt=std::mt19937_64(rd());
		dis=std::uniform_real_distribution<double>(0.0,1.0);
	}
	Random(int s){
		mt=std::mt19937_64(s);
		dis=std::uniform_real_distribution<double>(0.0,1.0);
	}
	virtual double next(){
		return dis(mt);
	}
};
