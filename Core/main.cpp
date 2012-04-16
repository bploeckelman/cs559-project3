/************************************************************************/
/* Game Testbed
/* ------------
/* Program entry point
/************************************************************************/
#include "MainWindow.h"

#include <iostream>
#include <conio.h>

using namespace std;

int main()
{
	try {
		MainWindow app;
	} catch(std::exception& e) {
		cerr << e.what() << endl
			<< "press a key to exit..." << endl;
		_getch();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
