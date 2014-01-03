

#include "Application.h"

int main(int argc,char** argv){
	Application* appli = new Application(argc,argv);
	appli->start();
	delete appli;
	return 0;
}
