// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <bitset>
/*
PROBLEM LIST
1. Might be a problem with signed/unsigned 
	-when converting and testing, the first 0 was missing when it shouldnt be
*/ 
struct block
{
	int num_m = 0; 
	unsigned long w[8]; //store message schedule for each m block? 
	unsigned long long m[8]; //store m blocks
	block *next;
};

struct schedule
{
	unsigned long long W[80]; 
};

void addBinary_M(bool binaryVal); 
void addM_Block(block *target, long long newM);
void forceWrite_M(); 
void printBinaryRep(unsigned long long target);
int getFileSize(std::string fileName);

block* returnedBlock; //will be used to pass a block pointer back from methods 

block* firstBlock; 
block* currentBlock; 
block* lastBlock; 

schedule AlgSchedule; 

int numBits = 0; 

static int bitSetCounter = 63;
std::bitset<64> binarySet;

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 

	int messageLength = 0; 

	//List< unsigned long long> storage;  
	int counter = 0; 

	//MIGHT NEED TO SET FIRST BLOCK AT THE BEGINNING TO CURRENT BLOCK 
	currentBlock = new block;
	firstBlock = currentBlock; 
	lastBlock = nullptr; //keep track of last block made, end of the file 

	//stack <char> storage; 
	
	if (argc == 2)
	{
		std::fstream myFile(argv[1], std::ios::binary | std::ios::in | std::ios::beg);

		if (myFile.fail()) //check if file failed to open
		{
			std::cout << "Unable to open file...\n";
			std::cin.get();
			exit(1);	
		}
		
		fileSize = getFileSize(argv[1]); //this will be the overall length of the final message (BYTES) 
	
		 //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		do
		{
			char container;
			myFile.read(&container, sizeof(char));
			if (myFile.eof()) break;

			std::bitset<8> charBitSet(container);
			std::cout << charBitSet;
			//insert byte into storage
			for (int i = 7; i >= 0; i--)
			{
				if (charBitSet.test(i) == true)
					addBinary_M(true);
				else
					addBinary_M(false);
			}
		} while (!myFile.eof());

		int numZeroBits = 0; 
		int firstBitLoc = 0; 

		//check if the length of the final message is a multiple of 512 
		if (((fileSize * 8) % 256) != 0)
		{
			//append a 1 onto the end of the message 
			//then follow directions of padding (dont understand yet) 

			//find block that contains the last bit and travers to that block 

			numZeroBits = 896 - (fileSize * 8 + 1); //number of zero bits that will be needed to pad message
			addBinary_M(true); //padding starts with a 1 before 0 pads 


			//std::bitset<64> currentBlock_Bit(lastBlock->m[lastBlock->num_m + 1]); 

			//insert last 1 into message 
			//int firstBitLoc = ((fileSize * 8) % 8);
			//currentBlock_Bit.set(firstBitLoc, 1); 
			//firstBitLoc++; 

			//set 0 padding bits 
			for (int i = 0; i < numZeroBits; i++)
			{
				addBinary_M(false);
			}
			//need to set l 
			//need to form a bitset of 128
			//split into 2 seperate bitsets of 64 
			//convert those into long longs and store in message blocks

			std::bitset<128> sizeSet(fileSize * 8); //this will store the last bits of the padding process 

			for (int i = 0; i < 128; i++)
			{
				if (sizeSet.test(i) == true)
					addBinary_M(true);
				else
					addBinary_M(false);
			}
		}
		//padding is complete data is ready 
		//forceWrite_M(); //dump what is in the set into the structure
		//prepare message schedule 

	}
	else
	{
		std::cout << "Invalid Argument. Need only filename...\n";
		std::cin.get(); 
		exit(1); 
	} 
}

//add a binary value to the message via a bitset 
void addBinary_M(bool binaryVal)
{
	numBits++; 
	 
	
	if (bitSetCounter >= 0)
	{
		binarySet.set(bitSetCounter, binaryVal);
		bitSetCounter--; 
	}
	else
	{
		//create long long and add to chain 
		unsigned long long newLong = binarySet.to_ullong(); 
		addM_Block(currentBlock, newLong); 

		if (currentBlock != returnedBlock)
			lastBlock = returnedBlock; 
		currentBlock = returnedBlock; 

		bitSetCounter = 63; 
	}
}

void rightRotate(std::bitset<64> *currentSet, int n)
{
	// x - w bit word
	//
	// (x << n) V ( x >> w - n) 
}

void leftRotate(std::bitset<64> *currentSet, int n)
{

}

//add M to target block, will return pointer to final block in the chain
void addM_Block(block *target, long long newM)
{
	//check if the first block actually exsists 
	if (!currentBlock)
	{
		currentBlock = new block;
		firstBlock = currentBlock; 
		lastBlock = currentBlock; 
	}
	//check if the block has room for the new M
	if (target->num_m < 8)
	{
		target->m[target->num_m] = newM; 
		target->num_m++; 
		returnedBlock = target; //set the return block
	}
	else
	{
		block *newBlock = new block; 
		target->next = newBlock; 
		newBlock->m[newBlock->num_m] = newM; 
		newBlock->num_m++;
		returnedBlock = newBlock; //set the return block
		currentBlock = newBlock; 
	}
}

void forceWrite_M()
{
	long long data = binarySet.to_ullong(); 
	addM_Block(currentBlock, data); 

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
}