
#define _CRTDBG_MAP_ALLOC

/*#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif
#endif  // _DEBUG*/

#include <stdlib.h>
#include <crtdbg.h>

#include "Application.h"

int main(int argc,char** argv){
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	Application* appli = new Application(argc,argv);
	appli->start();
	delete appli;
	_CrtDumpMemoryLeaks();
	return 0;
}
