// SHA256.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <bitset>
#include <sstream>

/*
PROBLEM LIST
1. Might be a problem with signed/unsigned 
	-when converting and testing, the first 0 was missing when it shouldnt be
*/ 
struct block
{
	int num_m = 0; 
	uint64_t  m[16]; //store m blocks
	block *next = nullptr; 
};

struct schedule
{
	uint64_t  W[80]; 
};

struct hashValue
{
	uint64_t  H[8]; 
	hashValue *nextHash = nullptr; 
};

#pragma region Methods
uint64_t  rightRotate(uint64_t  word, int n); 
uint64_t  leftRotate(uint64_t  word, int n); 
uint64_t  rightShift(uint64_t  word, int n); 
uint64_t  Wt_bottomCalc(int t); 
uint64_t  calc_seriesZero(uint64_t  word);
uint64_t  calc_seriesOne(uint64_t  word); 
uint64_t  calc_Ch(uint64_t  x, uint64_t  y, uint64_t  z); 
uint64_t  calc_Maj(uint64_t  x, uint64_t  y, uint64_t  z); 
void addBinary_M(bool binaryVal);
void addM_Block(block *target, uint64_t  newM);
void forceWrite_M();
uint64_t  sigma_ZeroCalc(uint64_t  word); 
uint64_t  sigma_UnoCalc(uint64_t  word); 
uint64_t  takeCompliment(uint64_t  target); 
int getLeastSigBit(std::bitset<64> *target); 
void printBinaryRep(uint64_t  target);
int getFileSize(std::string fileName);
#pragma endregion

block* returnedBlock; //will be used to pass a block pointer back from methods 

block* firstBlock; 
block* currentBlock; 
block* lastBlock; 

schedule AlgSchedule; 

int modVal = 0; 

//hashValue *initialHash = new hashValue; 

int numBits = 0; 
int numBlocks = 1; 

int bitSetCounter = 63;
std::bitset<64> binarySet;

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 

	int messageLength = 0; 

	//List< uint64_t > storage;  
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
	
		modVal = 2 ^ 64; //set the modVal for use in algorithm addition 

		 //this might not be reading it quite correctly, cant tell yet it difficult to tell if the long long representation is binary accurate
		do
		{
			char container;
			myFile.read(&container, sizeof(char));
			if (myFile.eof()) break;

			std::bitset<8> charBitSet(container);
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
		if ((numBits % 1024) != 0)
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

			for (int i = 127; i >=0 ; i--)
			{
				if (sizeSet.test(i) == true)
					addBinary_M(true);
				else
					addBinary_M(false);
			}
		}
		else if (numBits == 0)
		{
			for (int i = 0; i < 16; i++)
			{
				if (i == 0)
				{
					addBinary_M(true); 
					for (int j = 0; j < 64; j++)
					{
						addBinary_M(false); 
					}
				}
				else
				{
					for (int j = 0; j < 64; j++)
					{
						addBinary_M(false); 
					}
				}
			}
		}
		//padding is complete data is ready 
		//forceWrite_M(); //dump what is in the set into the structure
		
		//set initial hash value 
		hashValue hash; 
		for (int i = 0; i < 8; i++)
			hash.H[i] = initialHash[i]; 
		
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
					AlgSchedule.W[j] = algBlock->m[j]; 
				}
				else
				{
					AlgSchedule.W[j] = Wt_bottomCalc(j); //UNTESTED ----------------
				}
			}

			//initilize the 8 working variables (all 64 bits each) 
			uint64_t  T_one, T_two; 
			uint64_t  a = hash.H[0]; 
			uint64_t  b = hash.H[1]; 
			uint64_t  c = hash.H[2]; 
			uint64_t  d = hash.H[3]; 
			uint64_t  e = hash.H[4]; 
			uint64_t  f = hash.H[5]; 
			uint64_t  g = hash.H[6]; 
			uint64_t  h = hash.H[7]; 

			//part 3
			for (int k = 0; k < 80; k++)
			{
				/*
				T_one = (h + calc_seriesOne(e) + calc_Ch(e, f, g) + constants[k] + AlgSchedule.W[k]) % modVal;
				T_two = (calc_seriesZero(a) + calc_Maj(a, b, c)) % modVal;
				h = g;
				g = f;
				f = e;
				e = (d + T_one) % modVal;
				d = c;
				c = b;
				b = a;
				a = (T_one + T_two) % modVal;
				*/
				uint64_t calcResp_seriesOne = calc_seriesOne(e); 
				uint64_t calcResp_Ch = calc_Ch(e, f, g); 
				uint64_t consta = constants[k]; 
				uint64_t sch = AlgSchedule.W[k]; 

				T_one = h + calc_seriesOne(e) + calc_Ch(e, f, g) + constants[k] + AlgSchedule.W[k]; 
				T_two = calc_seriesZero(a) + calc_Maj(a, b, c); 
				h = g;
				g = f;
				f = e;
				e = (d + T_one); 
				d = c;
				c = b;
				b = a;
				a = (T_one + T_two); 
					
			}

			//compute the ith intermediate hash value H[I] 
			
			hash.H[0] = (a + hash.H[0]); 
			hash.H[1] = (b + hash.H[1]); 
			hash.H[2] = (c + hash.H[2]); 
			hash.H[3] = (d + hash.H[3]); 
			hash.H[4] = (e + hash.H[4]); 
			hash.H[5] = (f + hash.H[5]); 
			hash.H[6] = (g + hash.H[6]); 
			hash.H[7] = (h + hash.H[7]); 
			
			/*
			hash.H[0] = (a + hash.H[0]) % modVal;
			hash.H[1] = (b + hash.H[1]) % modVal;
			hash.H[2] = (c + hash.H[2]) % modVal;
			hash.H[3] = (d + hash.H[3]) % modVal;
			hash.H[4] = (e + hash.H[4]) % modVal;
			hash.H[5] = (f + hash.H[5]) % modVal;
			hash.H[6] = (g + hash.H[6]) % modVal;
			hash.H[7] = (h + hash.H[7]) % modVal;
			*/ 
		}
		//compute final hash value -- print out 
		std::cout << "Hash Value of input file : \n"; 
		for (int i = 0; i < 8; i++)
		{
			std::bitset<64> temp(hash.H[i]);
			//std::cout << std::hex << temp.to_ullong() << std::endl; 
			std::cout << std::hex << temp.to_ullong(); 
			//std::cout << temp.to_string(); 
		}
		//std::cout << "complete \n"; 
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
	//write and then check to see if the word is full
	binarySet.set(bitSetCounter, binaryVal);
	bitSetCounter--;

	if (bitSetCounter < 0)
	{
		//create long long and add to chain 
		uint64_t  newLong = binarySet.to_ullong(); 
		addM_Block(currentBlock, newLong); 

		if (currentBlock != returnedBlock)
			lastBlock = returnedBlock; 
		currentBlock = returnedBlock; 

		bitSetCounter = 63;
	}
	numBits++;
}


//add M to target block, will return pointer to final block in the chain
void addM_Block(block *target, uint64_t  newM)
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
	//if (target->num_m <= 16)
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


uint64_t  rightRotate(uint64_t  word, int n)
{
	//uint64_t  result = ((RBitWord >> n) | (RBitWord << 64 - n)).to_ullong();
	return (word >> n) | (word << (64 - n)); 
}

//void leftRotate(std::bitset<64> *currentSet, int n)
uint64_t  leftRotate(uint64_t  word, int n)
{
	std::bitset<64> LBitWord(word); 

	//std::bitset<64> LBit_SubAlpha = LBitWord.operator<<(n); 
	//std::bitset<64> LBit_SubBeta = LBitWord.operator>>(64 - n); 

	//uint64_t  result = ((LBitWord << n) |= (LBitWord >> 64 - n)).to_ullong();
	uint64_t  result = ((LBitWord << n) | (LBitWord >> 64 - n)).to_ullong();
	return result; 

	//return (LBit_SubAlpha |= LBit_SubBeta).to_ullong(); 
}


void forceWrite_M()
{
	uint64_t  data = binarySet.to_ullong(); 
	addM_Block(currentBlock, data); 
}

uint64_t  sigma_ZeroCalc(uint64_t  word)
{
	std::bitset<64> Zero_subAlpha = rightRotate(word, 1); 
	std::bitset<64> Zero_subBeta = rightRotate(word, 8);
	std::bitset<64> Zero_subThird = rightShift(word, 7);

	
	//uint64_t  result = (Zero_subAlpha ^= Zero_subBeta ^= Zero_subThird).to_ullong(); 
	uint64_t  result = (Zero_subAlpha ^ Zero_subBeta ^ Zero_subThird).to_ullong(); 
	return result; 
}

uint64_t  sigma_UnoCalc(uint64_t  word)
{
	std::bitset<64> Uno_subAlpha = rightRotate(word, 19); 
	std::bitset<64> Uno_subBeta = rightRotate(word, 61); 
	std::bitset<64> Uno_subThird = rightShift(word, 6); 

	//uint64_t  result = (Uno_subAlpha ^= Uno_subBeta ^= Uno_subThird).to_ullong(); 
	uint64_t  result = (Uno_subAlpha ^ Uno_subBeta ^ Uno_subThird).to_ullong(); 
	return result; 
}

uint64_t  rightShift(uint64_t  word, int n)
{
	std::bitset<64> rShift(word); 
	uint64_t  result = (rShift >> n).to_ullong();
	return result; 
}

uint64_t  Wt_bottomCalc(int t)
{
	//there are 4 calculations whose results need to be added to solve this calculation 
	uint64_t  partOne = sigma_UnoCalc(AlgSchedule.W[t - 2]); 

	uint64_t  partTwo = AlgSchedule.W[t - 7]; 

	uint64_t  partThree = sigma_ZeroCalc(AlgSchedule.W[t - 15]);

	uint64_t  partFour = AlgSchedule.W[t - 16]; 

	//uint64_t  result = (partOne + partTwo + partThree + partFour) % modVal; 
	uint64_t  result = (partOne + partTwo + partThree + partFour); 
	//read that using unsigned is the same as -- Addition (+) is performed modulo 2^64
	return result; 
}

//computes the series 0 calculation in the algorithm 
uint64_t  calc_seriesZero(uint64_t  word)
{
	std::bitset<64> sA_first(rightRotate(word, 28)); 
	std::bitset<64> sA_second(rightRotate(word, 34)); 
	std::bitset<64> sA_third(rightRotate(word, 39)); 

	//uint64_t  result = (sA_first ^= sA_second ^= sA_third).to_ullong(); 
	uint64_t  result = (sA_first ^ sA_second ^ sA_third).to_ullong(); 
	return result; 
}

//computes the series 1 calculation in the algorithm 
uint64_t  calc_seriesOne(uint64_t  word)
{
	std::bitset<64> sB_first(rightRotate(word, 14)); 
	std::bitset<64> sB_second(rightRotate(word, 18)); 
	std::bitset<64> sB_third(rightRotate(word, 41)); 

	//uint64_t  result = (sB_first ^= sB_second ^= sB_third).to_ullong(); 
	uint64_t  result = (sB_first ^ sB_second ^ sB_third).to_ullong(); 
	return result; 
}

//runs the calculation Ch(x, y, z) required by the algorithm 
uint64_t  calc_Ch(uint64_t  x, uint64_t  y, uint64_t  z)
{

	uint64_t  result = ((x & y) ^ (~x & z)); 
	
	//uint64_t  result = ((bit_x &= bit_y) ^= (bit_complX &= bit_z)).to_ullong(); 
	//uint64_t  result = ((bit_x & bit_y) ^= (bit_complX & bit_z)).to_ullong(); 
	return result; 
}

//runs the Maj(x,y,z) calculation required by the algorithm 
uint64_t  calc_Maj(uint64_t  x, uint64_t  y, uint64_t  z)
{
	std::bitset<64> bit_x(x); 
	std::bitset<64> bit_y(y); 
	std::bitset<64> bit_z(z); 

	uint64_t  result = ((x & y) ^ (x & z) ^ (y & z)); 
	//uint64_t  result = ((bit_x &= bit_y) ^= (bit_x &= bit_z) ^= (bit_y &= bit_z)).to_ullong(); 
	//uint64_t  result = ((bit_z & bit_y) ^= (bit_x & bit_z) ^= (bit_y & bit_z)).to_ullong(); 
	return result; 
}

uint64_t  takeCompliment(uint64_t  target)
{
	int posOfLeastSig = -1; 

	std::bitset<64> bit_raw(target);

	//find position of least significant 1 value in set 
	for (int i = 0; i < 64; i++)
	{
		if (bit_raw.test(i) == true)
		{
			posOfLeastSig = i;
			break; 
		}
	}

	//flip all more significant bits after the 1 found 
	for (int j = posOfLeastSig + 1; j < 64 ; j++)
		bit_raw.flip(j); 
	
	uint64_t  result = bit_raw.to_ullong(); 
	return result; 
}

int getFileSize(std::string fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	return file.tellg();
}

void printBinaryRep(uint64_t target)
{
	uint64_t  tester = 1; 
	std::bitset<64> test(target); 

	std::cout << test; 

	target = test.to_ullong(); 

	delete(&test); 
}