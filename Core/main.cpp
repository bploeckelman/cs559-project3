/************************************************************************/
/* Game Testbed
/* ------------
/* Program entry point
/************************************************************************/
#include "MainWindow.h"

#include <exception>
#include <iostream>
#include <conio.h>

using namespace std;

int main()
{
	try {
		MainWindow app;
	} catch(std::runtime_error& e) {
		cout << e.what() << endl
			<< "press a key to exit..." << endl;
		_getch();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
