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

	

	//int N, G, V;

	//cout << "Enter the number of Normal cooks, Vegan cooks, VIP cooks: ";
	//cin >> N;
	//cin >> G;
	//cin >> V;


	//int SN, SG, SV;

	//cout << "Enter Speed of Normal cooks: "; cin >> SN; cout << endl;
	//cout << "Enter Speed of Vegan cooks: "; cin >> SG; cout << endl;
	//cout << "Enter Speed of VIP cooks: "; cin >> SV; cout << endl;

	//int BO, BN, BG, BV;

	//cout << "Enter number of orders a cook must prepare before taking a break: "; cin >> BO; cout << endl;
	//cout << "Enter break duration for Normal cooks: "; cin >> BN; cout << endl;
	//cout << "Enter break duration for Vegan cooks: "; cin >> BG; cout << endl;
	//cout << "Enter break duration for VIP cooks: "; cin >> BV; cout << endl;








	return 0;
}
