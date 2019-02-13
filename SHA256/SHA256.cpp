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
	unsigned long long m[16]; //store m blocks
	block *next = nullptr; 
};

struct schedule
{
	unsigned long long W[80]; 
};

struct hashValue
{
	unsigned long long H[8]; 
	hashValue *nextHash = nullptr; 
};

#pragma region Methods
long long rightRotate(long long word, int n); 
long long leftRotate(long long word, int n); 
long long rightShift(long long word, int n); 
unsigned long long Wt_bottomCalc(int t); 
void addBinary_M(bool binaryVal);
void addM_Block(block *target, long long newM);
void forceWrite_M();
long long sigma_ZeroCalc(long long word); 
long long sigma_UnoCalc(long long word); 
void printBinaryRep(unsigned long long target);
int getFileSize(std::string fileName);
#pragma endregion

block* returnedBlock; //will be used to pass a block pointer back from methods 

block* firstBlock; 
block* currentBlock; 
block* lastBlock; 

schedule *AlgSchedule; 

//hashValue *initialHash = new hashValue; 

int numBits = 0; 
int numBlocks = 1; 

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
		if (((fileSize * 8) % 1024) != 0)
		{ 
			numZeroBits = 896 - (fileSize * 8 + 1); //number of zero bits that will be needed to pad message
			addBinary_M(true); //padding starts with a 1 before 0 pads 

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
		
		//set initial hash value 
		hashValue *hash = new hashValue; 
		for (int i = 0; i < 8; i++)
			hash->H[i] = initialHash[i]; 
		
		block *algBlock = firstBlock; 

		
		for (int i = 0; i < numBlocks; i++)
		{
			//set the correct block to work with 
			for (int h = 0; h < i; h++)
			{
				algBlock = algBlock->next; 
			}

			//prepare message schedule 
			for (int j = 0; j < 80; j++)
			{
				if (j <= 15)
				{
					AlgSchedule->W[j] = algBlock->m[j]; 
				}
				else
				{
					
				}
			}
		}
		std::cout << "prepare message \n"; 
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

long long rightRotate(long long word, int n)
{
	std::bitset<64> RBitWord(word); 

	std::bitset<64> RBit_SubAlpha = RBitWord.operator>>(n); 
	std::bitset<64> RBit_SubBeta = RBitWord.operator<<(64 - n); 

	return (RBit_SubAlpha |= RBit_SubBeta).to_ullong();
}

//void leftRotate(std::bitset<64> *currentSet, int n)
long long leftRotate(long long word, int n)
{
	std::bitset<64> LBitWord(word); 

	std::bitset<64> LBit_SubAlpha = LBitWord.operator<<(n); 
	std::bitset<64> LBit_SubBeta = LBitWord.operator>>(64 - n); 

	return (LBit_SubAlpha |= LBit_SubBeta).to_ullong(); 
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
	if (target->num_m < 16)
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
		numBlocks++; 
	}
}

void forceWrite_M()
{
	long long data = binarySet.to_ullong(); 
	addM_Block(currentBlock, data); 

}

long long sigma_ZeroCalc(long long word)
{

	std::bitset<64> Zero_subAlpha = rightRotate(word, 1); 
	std::bitset<64> Zero_subBeta = rightRotate(word, 8);
	std::bitset<64> Zero_subThird = rightShift(word, 7);

	long long result = (Zero_subAlpha ^= Zero_subBeta ^= Zero_subThird).to_ullong(); 
	return result; 
}

long long sigma_UnoCalc(long long word)
{
	std::bitset<64> Uno_subAlpha = rightRotate(word, 19); 
	std::bitset<64> Uno_subBeta = rightRotate(word, 61); 
	std::bitset<64> Uno_subThird = rightShift(word, 6); 

	long long result = (Uno_subAlpha ^= Uno_subBeta ^= Uno_subThird).to_ullong(); 
	return result; 
}

long long rightShift(long long word, int n)
{
	std::bitset<64> rShift(word); 
	long long result = rShift.operator>>(n).to_ullong();
	return result; 
}

unsigned long long Wt_bottomCalc(int t)
{
	//there are 4 calculations whose results need to be added to solve this calculation 
	long long partOne = sigma_UnoCalc(AlgSchedule->W[t - 2]); 

	long long partTwo = AlgSchedule->W[t - 7]; 

	long long partThree = sigma_ZeroCalc(AlgSchedule->W[t - 15]);

	long long partFour = AlgSchedule->W[t - 16]; 

	unsigned long long result = partOne + partTwo + partThree + partFour; 

	//read that using unsigned is the same as -- Addition (+) is performed modulo 2^64
	return result; 
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