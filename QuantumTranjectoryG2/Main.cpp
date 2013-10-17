
#pragma warning(disable : 4996)

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <omp.h>
#include <conio.h>

using namespace std;
using namespace boost;

#include "Application.h"

int main(int argc,char** argv){
	Application* appli = new Application(argc,argv);
	appli->start();
	delete appli;
	return 0;
}
