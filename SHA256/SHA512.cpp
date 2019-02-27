//Jacob Schaupp 
//EECS 3540: Systems and Systems Programming 
//SHA - 512
//This program contains an implementation of the SHA-512 algorithm, taking an argument of the file path that the algorithm is to compute the 
//HASH value of 

#include "pch.h"
#include "List.h"
#include "SHA256.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <bitset>
#include <sstream>

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
void padRestOfBlock(); 
void addM_Block(block *target, uint64_t  newM);
void forceWrite_M();
uint64_t  sigma_ZeroCalc(uint64_t  word); 
uint64_t  sigma_UnoCalc(uint64_t  word); 
void printBinaryRep(uint64_t  target);
int getFileSize(std::string fileName);
#pragma endregion

block* returnedBlock; //will be used to pass a block pointer back from methods 

block* firstBlock; 
block* currentBlock; 
block* lastBlock; 

schedule AlgSchedule; 

int modVal = 0; 

int numBits = 0; 
int numBlocks = 1; 

int bitSetCounter = 63;
std::bitset<64> binarySet;

int main(int argc, char* argv[])
{
	//information for padding and number of reads needed for whole file 
	int fileSize = 0; 

	int messageLength = 0; 

	int counter = 0; 

	currentBlock = new block;
	firstBlock = currentBlock; 
	lastBlock = nullptr; //keep track of last block made, end of the file 

	if (argc == 2)
	{
		std::fstream myFile(argv[1], std::ios::binary | std::ios::in | std::ios::beg);

		if (myFile.fail()) //check if file failed to open
		{
			std::cout << "Unable to open file...\n";
			std::cin.get();
			exit(1);	
		}
		
		fileSize = getFileSize(argv[1]);

		modVal = 2 ^ 64; //set the modVal for use in algorithm addition 
		do
		{
			char container;
			myFile.read(&container, sizeof(char));

			if (myFile.eof()) break; //check end of file flag
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

		//check if the current block has 2 slots left, if not ; fill the thing with 0s 
		//then pad new block with 0s and message length 
		if (currentBlock->num_m >= 13)
		{
			padRestOfBlock();

			//put zeros in block all except for last 2 M's
			for (int i = 0; i < 14; i++)
			{
				for (int j = 63; j >= 0; j--)
				{
					addBinary_M(false);
				}
			}

			//encode the file size 
			std::bitset<128> sizeSet(fileSize * 8);
			for (int i = 127; i >= 0; i--)
			{
				if (sizeSet.test(i) == true)
					addBinary_M(true);
				else
					addBinary_M(false);
			}
		}
		//check if the length of the final message is a multiple of 512 
		else if ((numBits % 1024) != 0)
		{ 
			numZeroBits = 896 - (fileSize * 8 + 1); //number of zero bits that will be needed to pad message
			//numZeroBits = 896 - numBits - (1024 * ((fileSize * 8) / 1024)) + 1; 
			addBinary_M(true); //padding starts with a 1 before 0 pads 

			//set 0 padding bits 
			for (int i = 0; i < numZeroBits; i++)
			{
				addBinary_M(false);
			}

			//encode the file size into the block chain 
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
		
		//set initial hash value 
		hashValue hash; 
		for (int i = 0; i < 8; i++)
			hash.H[i] = initialHash[i]; 
		
		for (int i = 0; i < numBlocks; i++)
		{
			block *algBlock = firstBlock;
			//set the correct block to work with 
			for (int z = 0; z < i; z++)
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
		}

		//compute final hash value -- print out 
		std::cout << "Hash Value of input file : \n"; 
		for (int i = 0; i < 8; i++)
		{
			std::bitset<64> temp(hash.H[i]);
			std::cout << std::hex << temp.to_ullong(); 
		}
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

void padRestOfBlock()
{
	addBinary_M(true); 
	for (int i = currentBlock->num_m; i< 16; i++)
	{
		for (int j = bitSetCounter; j >= 0; j--)
		{
			addBinary_M(false); 
		}
	}
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
	return (word >> n) | (word << (64 - n)); 
}

//void leftRotate(std::bitset<64> *currentSet, int n)
uint64_t  leftRotate(uint64_t  word, int n)
{
	std::bitset<64> LBitWord(word); 
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
	
	uint64_t  result = (Zero_subAlpha ^ Zero_subBeta ^ Zero_subThird).to_ullong(); 
	return result; 
}

uint64_t  sigma_UnoCalc(uint64_t  word)
{
	std::bitset<64> Uno_subAlpha = rightRotate(word, 19); 
	std::bitset<64> Uno_subBeta = rightRotate(word, 61); 
	std::bitset<64> Uno_subThird = rightShift(word, 6); 

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

	uint64_t  result = (partOne + partTwo + partThree + partFour); 
	return result; 
}

//computes the series 0 calculation in the algorithm 
uint64_t  calc_seriesZero(uint64_t  word)
{
	std::bitset<64> sA_first(rightRotate(word, 28)); 
	std::bitset<64> sA_second(rightRotate(word, 34)); 
	std::bitset<64> sA_third(rightRotate(word, 39)); 

	uint64_t  result = (sA_first ^ sA_second ^ sA_third).to_ullong(); 
	return result; 
}

//computes the series 1 calculation in the algorithm 
uint64_t  calc_seriesOne(uint64_t  word)
{
	std::bitset<64> sB_first(rightRotate(word, 14)); 
	std::bitset<64> sB_second(rightRotate(word, 18)); 
	std::bitset<64> sB_third(rightRotate(word, 41)); 

	uint64_t  result = (sB_first ^ sB_second ^ sB_third).to_ullong(); 
	return result; 
}

//runs the calculation Ch(x, y, z) required by the algorithm 
uint64_t  calc_Ch(uint64_t  x, uint64_t  y, uint64_t  z)
{
	uint64_t  result = ((x & y) ^ (~x & z));  
	return result; 
}

//runs the Maj(x,y,z) calculation required by the algorithm 
uint64_t  calc_Maj(uint64_t  x, uint64_t  y, uint64_t  z)
{
	uint64_t  result = ((x & y) ^ (x & z) ^ (y & z)); 
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