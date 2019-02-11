// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>

void printBinaryRep(unsigned long long target); 
int getFileSize(std::string fileName); 

struct block
{
	int num_m = 0; 
	unsigned long long m[8]; 
	block *next;
};

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 

	char test; 
	List< unsigned long long> storage;  
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
		
		fileSize = getFileSize(argv[1]); //this will be the overall length of the final message 


		while (!myFile.eof()) //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		{
			//worried about pointers vs standard variables and memory storage 
			unsigned long long temp; 
			myFile.read(reinterpret_cast<char *> (&temp), sizeof(unsigned long long));

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

		/*
		if (currentBlock->num_m != 8)
		{
			//this block is not full, need padding bits

		}
		*/

		//check if the length of the final message is a multiple of 512 
		if ((fileSize % 512) != 0)
		{
			//append a 1 onto the end of the message 
			//then follow directions of padding (dont understand yet) 
			
			//find block that contains the last bit and travers to that block 
			
		}

		 

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

void printBinaryRep(unsigned long long int target)
{
	unsigned long long tester = 1; 

	for (int i = 63; i >= 0; i--)
	{
		std::cout << (target >> i) & 
	}

}