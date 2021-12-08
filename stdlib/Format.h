#include <stdint.h>	//integer types - uint8_t etc
#include <stdio.h>	//FILE, fopen()...
#include <ctype.h>	//isdigit()
#include <string.h>	//strchr(), strtok()
#include <stdlib.h>	//malloc(), free()
#include <assert.h>	//assert()
#include <math.h>	//pow(), log()
#include <stdlib.h>	//full declaration of malloc(), free() etc..
#include "platform.h"	//platform/compiler definitions
#include <vector>
#ifdef C99
#include <limits.h>	//INT_MAX
#endif


#define MAXINPUTLEN 255	//maximum size of input

template <typename T> void DelVec( std::vector<T*>& container ) {
	for( unsigned int i=0;i<container.size();i++ ) {
		if( container[i] ) {
			delete container[i];
			container[i]=NULL;
		}
	}
}

//allocates memory to msg and stores file contest in it
bool GetFileText( const char * fileName, char ** msg );
//takes a number, and stores digits of number for a given base in the pDigits pointer
void NumToDigPtr( int no, uint8_t * pDigits,int base,int ptrLen);
//prints a list of numbers
void PrintNumbers( uint8_t * pDigits, int nDigits);
//removes '\n'
void FormatInput( char * in_out  );
//checks if a string contains an UNSIGNED number
bool IsNumber( const char * p );
//takes a given string and stores in a number list - extCheck = optional function pointer that checks number list
bool FormatToNumbers( const char * p_sz, uint8_t * noList, int nExpected, int rangeMin, int rangeMax, bool (*extCheck)(uint8_t*) );