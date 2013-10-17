
#pragma warning(disable : 4996) // disable stdio safety
#pragma warning(disable : 4819) // disable boost is not unicode
#pragma warning(disable : 4290) // disable VC++ cannot specify error type


#include "Application.h"

int main(int argc,char** argv){
	Application* appli = new Application(argc,argv);
	appli->start();
	delete appli;
	return 0;
}
