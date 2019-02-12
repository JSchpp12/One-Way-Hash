// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <bitset>

struct block
{
	int num_m = 0; 
	unsigned long w[8]; //store message schedule for each m block? 
	unsigned long long m[8]; //store m blocks
	block *next;
};

void addBinary_M(bool binaryVal); 
void addM_Block(block *target, long long newM);
void printBinaryRep(unsigned long long target);
int getFileSize(std::string fileName);

block* returnedBlock; //will be used to pass a block pointer back from methods 

block* firstBlock; 
block* currentBlock; 
block* lastBlock; 

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 

	int messageLength = 0; 

	List< unsigned long long> storage;  
	int counter = 0; 

	//MIGHT NEED TO SET FIRST BLOCK AT THE BEGINNING TO CURRENT BLOCK 
	firstBlock = nullptr; //first block, beginning of the file 
	currentBlock = new block;
	lastBlock = nullptr; //keep track of last block made, end of the file 

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
			//read in chars and construct a bitset by 8 byte chunks 
			//FIX THIS SECTION THERE IS SOMETHIG WEIRD GOING ON  ------ (not reading correctly, dont want to take up more bits than is necessary)
			unsigned long long temp; 
			myFile.read(reinterpret_cast<char *> (&temp), sizeof(unsigned long long));

			std::bitset<64> testset(temp); 

			//check if current message is full, if so - create new one
			if (currentBlock->num_m >= 8)
			{
				block *previousBlock = currentBlock;
				currentBlock = new block;
				lastBlock = currentBlock;
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
				numZeroBits = 896 - (fileSize * 8 + 1); //number of zero bits that will be needed to pad message
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

						if (currentBlock->num_m >= 8)
						{
							//current block is full need a new one 
							block *previousBlock = currentBlock;
							currentBlock = new block;
							lastBlock = currentBlock;
							previousBlock->next = currentBlock; //set newly created block as next in chain 
						}
						currentBlock->m[currentBlock->num_m] = bitSetBack;
						currentBlock->num_m++;

						//just restart using the same bitset 
						firstBitLoc = 0; 
					}
					currentBlock_Bit.set(firstBitLoc, 0);
					firstBitLoc++; //increment counter 
				}
				//need to set l 
				//need to form a bitset of 128
				//split into 2 seperate bitsets of 64 
				//convert those into long longs and store in message blocks

				std::bitset<128> sizeSet(fileSize * 8); //this will store the last bits of the padding process 
				std::bitset<64> mostSignificant; 
				std::bitset<64> leastSignificant; 
				 
				int subCounter = 0; 

				//split the 128 bitset apart by testing each bit and copying output to designated sets
				for (int i = 0; i < 128; i++)
				{
					if (sizeSet.test(i) == true)
						if (i < 64)
							leastSignificant[i] = true;
						else
							mostSignificant[i - 64] = true;
					else
						if (i < 64)
							leastSignificant[i] = false;
						else
							mostSignificant[i - 64] = false; 
				}
				long long mostSigLong = mostSignificant.to_ullong(); 
				long long leastSigLong = leastSignificant.to_ullong();

				currentBlock->m[currentBlock->num_m] = mostSigLong; 
				currentBlock->num_m++; 

				//add long long to blocks 
				addM_Block(currentBlock, mostSigLong); 

				if (currentBlock != returnedBlock)
					lastBlock = returnedBlock;

				currentBlock = returnedBlock; 
				
				addM_Block(currentBlock, leastSigLong); 

				if (currentBlock != returnedBlock)
					lastBlock = returnedBlock;

				currentBlock = returnedBlock; 
			
				//currentBlock->m[currentBlock->num_m] = fileSize * 8; 
				//currentBlock->num_m++; 
			}
		}
		std::cout << "complete"; 
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
	static int bitSetCounter = 63; 
	static std::bitset<64> binarySet; 

	if (bitSetCounter >= 0)
	{
		binarySet.set(bitSetCounter, binaryVal);
		bitSetCounter--; 
	}
	else
	{
		//create long long and add to chain 
		long long newLong = binarySet.to_ullong(); 
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
}