//#include "Drawing.h"
#include "Rest\Restaurant.h"
#include "GUI\GUI.h"
#include "ArrayStack.h"
#include "Pri_Queue.h"
#include <iostream>
#include <fstream>


string getInputFileName(GUI* pGUI)
{
	pGUI->PrintMessage("Enter input file name: ");
	string filename = pGUI->GetString();
	return filename;
}

int main()
{
	Restaurant* pRest = new Restaurant;
	pRest->RunSimulation();
	
	delete pRest;

	

	return 0;
}
