// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>

using namespace std; 

int main(int argc, char* argv[])
{
	fstream myFile(argv[1], ios::binary | ios::in); 
	stack <long long> storage; 
	long long temp = 0; 
	std::cout << sizeof(long long); 
	//char temp; 
	
	if (argc == 2)
	{
		int m_counter = 0;
		//just a file name 
		//myFile.open();

		if (myFile.fail()) //check if file failed to open
		{
			cout << "Unable to open file...\n";
			cin.get();
			exit(1);	
		}
		
		
		while (!myFile.eof()) //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		{
			myFile.read(reinterpret_cast<char *> (&temp), sizeof(long long)); 
			storage.push(temp); 

			//myFile.read((&temp), sizeof(char));
			//std::cout << temp << "\n"; 
		}
		
	}
	else
	{
		std::cout << "Invalid Argument. Need only filename...\n";
		std::cin.get(); 
		exit(1); 
	}

	std::cout << "Hello World!\n";
	std::cout << storage.top(); 

	//need to read in file as chunks of 8 byte groups 

}