// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <fstream>

using namespace std; 

int main(int argc, char* argv[])
{
	ofstream myFile;

	uint8_t m[128]; //place to store chunks
	if (argc == 2)
	{
		int m_counter = 0;
		//just a file name 
		myFile.open(argv[0], ios::binary | ios::in);

		if (myFile.fail()) //check if file failed to open
		{
			cout << "Unable to open file...\n";
			cin.get();
			exit(1);
		}

		/*
		while (!myFile.eof())
		{
			if (m_counter < 128)
			{
				//read byte from file and place it in the array
				myFile << m[m_counter];
				m_counter++;
			}
			else
			{
				//ran out of space in block, need a new block
				m_counter = 0; //reset counter
			}
		}
		*/
	}
	else
	{
		std::cout << "Invalid Argument. Need only filename...\n";
		std::cin.get(); 
		exit(1); 
	}

	std::cout << "Hello World!\n";
	//need to read in file as chunks of 8 byte groups 

}