// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <bitset>

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
	block *firstBlock = nullptr; //first block, beginning of the file 
	block *currentBlock = new block;
	block *lastBlock = nullptr; //keep track of last block made, end of the file 

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
		
		fileSize = getFileSize(argv[1]); //this will be the overall length of the final message (BYTES)


		while (!myFile.eof()) //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		{
			//worried about pointers vs standard variables and memory storage 
			unsigned long long temp; 
			myFile.read(reinterpret_cast<char *> (&temp), sizeof(unsigned long long));

			//check if current message is full, if so - create new one
			if (currentBlock->num_m >= 8)
			{
				block *previousBlock = currentBlock;
				lastBlock = currentBlock;
				currentBlock = new block;
				previousBlock->next = currentBlock; //set newly created block as next in chain 
			}

			currentBlock->m[currentBlock->num_m] = temp;
			currentBlock->num_m++; //increment counter

			if (firstBlock == nullptr)
			{
				firstBlock = currentBlock;
				lastBlock = currentBlock; 
			}
		}

		int numZeroBits = 0; 
		int firstBitLoc = 0; 

		//check if the length of the final message is a multiple of 512 
		if (((fileSize*8)% 256) != 0)
		{
			//append a 1 onto the end of the message 
			//then follow directions of padding (dont understand yet) 
			
			//find block that contains the last bit and travers to that block 
			if (((fileSize*8) % 8) != 0)
			{
				int lastBit = fileSize % 256; 
				//last m block written was not complete, needs padding added 
				
				std::bitset<64> currentBlock(lastBlock->m[lastBlock->num_m - 1]); //grab the last m in the last block 

			}
			else
			{
				numZeroBits = 448 - (fileSize * 8 + 1); //number of zero bits that will be needed to pad message
				std::bitset<64> currentBlock_Bit(lastBlock->m[lastBlock->num_m + 1]); 
				
				//insert last 1 into message 
				int firstBitLoc = ((fileSize * 8) % 8);
				currentBlock_Bit.set(firstBitLoc, 1); 
				firstBitLoc++; 

				//set 0 padding bits 
				for (int i = 0; i < numZeroBits; i++)
				{
					if (firstBitLoc > 63)
					{ 
						//input bitset into structures
						long long bitSetBack = currentBlock_Bit.to_ullong();
						currentBlock->m[currentBlock->num_m] = bitSetBack;
						currentBlock->num_m++;

						//just restart using the same bitset 
						firstBitLoc = 0; 
					}
					currentBlock_Bit.set(firstBitLoc, 0);
					firstBitLoc++; //increment counter 
				}
				std::cout << "wow"; 
				//need to set l 
				currentBlock->m[currentBlock->num_m] = fileSize * 8; 
				currentBlock->num_m++; 
			}
		}
	}
	else
	{
		std::cout << "Invalid Argument. Need only filename...\n";
		std::cin.get(); 
		exit(1); 
	} 
}

int getFileSize(std::string fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	return file.tellg();
}

void printBinaryRep(unsigned long long int target)
{
	unsigned long long tester = 1; 
	std::bitset<64> test(target); 

	std::cout << test; 

	target = test.to_ullong(); 

	delete(&test); 
	/*
	for (int i = 63; i >= 0; i--)
	{
		std::cout << (target >> i) & 1; 
	}
	*/ 
}