// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>

int getFileSize(std::string fileName); 

struct block
{
	int num_m = 0; 
	long long m[8]; 
	block *next;
};

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 
	bool needPadding = false; 
	int numPaddingBits = 0; 
	int numReadsNeeded = 0; 

	char test; 
	List<long long> storage;  
	int counter = 0; 
	block *firstBlock = nullptr; 
	block *currentBlock = new block;

	//stack <char> storage; 
	
	if (argc == 2)
	{
		std::fstream myFile(argv[1], std::ios::binary | std::ios::in);

		if (myFile.fail()) //check if file failed to open
		{
			std::cout << "Unable to open file...\n";
			std::cin.get();
			exit(1);	
		}
		
		fileSize = getFileSize(argv[1]); 

		/*
		//check if the last chunk will need to be padded 
		if ((fileSize % 64) != 0)
		{
			numPaddingBits = fileSize % 64;
			numReadsNeeded = fileSize / 64;
			needPadding = true;
		}
		else
			numReadsNeeded = fileSize / 64;

		//if there are less than 64 bits in the file, only need to read once
		if (numReadsNeeded == 0) numReadsNeeded++; 

		*/ 


		while (!myFile.eof()) //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		//for (int i = 0; i < numReadsNeeded; i++)
		{
			//worried about pointers vs standard variables and memory storage 
			long long temp; 
			myFile.read(reinterpret_cast<char *> (&temp), sizeof(long long));

			//check if current message is full, if so - create new one
			if (currentBlock->num_m >= 8)
			{
				block *previousBlock = currentBlock;
				currentBlock = new block;
				previousBlock->next = currentBlock; //set newly created block as next in chain 
			}

			currentBlock->m[currentBlock->num_m] = temp;
			currentBlock->num_m++; //increment counter


			if (firstBlock == nullptr)
			{
				firstBlock = currentBlock;
			}
		}

		if (currentBlock->num_m != 8)
		{
			//this block is not full, need padding bits

		}

		/*
		long long v = reinterpret_cast<long long int> (firstBlock->m[0]); 
		for (int i = 63; i >= 0; i--)
		{
			std::cout << (v >> i) & 1)
		}
		*/ 

	}
	else
	{
		std::cout << "Invalid Argument. Need only filename...\n";
		std::cin.get(); 
		exit(1); 
	}


	
	/*
	for (int m = 0; m < 1; m++)
	{
		//test bits for testing 
		for (int i = 0; i < 8; i++)
		{
			if (test & (1 << (i - 1)))
				std::cout << 1;
			else
				std::cout << 0;
		}
	}
	 */ 
	//need to read in file as chunks of 8 byte groups 
}

int getFileSize(std::string fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	return file.tellg();
}