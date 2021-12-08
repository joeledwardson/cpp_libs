#include "Format.h"

bool GetFileText( const char * fileName, char ** msg )
{
	FILE * f = fopen(fileName,"r");	//open file in read mode
	if( !f )
		return false;

	fseek( f, 0, SEEK_END );	//seek to end
	int len = ftell( f );		//get current position in file (equal to the length)
	assert( len >= 0 );
	fseek( f, 0, SEEK_SET);	//seek back to start
	len++;	//increment length (for NULL character)
	*msg = (char*)malloc( sizeof(char) * len);	//allocate memory to pointer
	memset( *msg, '\0',sizeof(char) * len );	//set to NULL
	fread( *msg,sizeof(char),(len-1),f);	//read in characters from file
	fclose(f);		//close file
	return true;

}
void NumToDigPtr(  int no, uint8_t * pDigits,int base,int ptrLen)
{
	memset( pDigits,0,ptrLen * sizeof(uint8_t));
	float maxPower_f = log( (float)no ) / log( (float)base );
	for( int i = 0; i <= (int)maxPower_f; i++ )
	{
		
		int x = no / (int)pow( (float)base,i);
		x %= base;
		pDigits[ptrLen - i - 1] = (uint8_t)x;
	
	}
}
void PrintNumbers( uint8_t * pDigits, int nDigits)
{
	
	if( nDigits )
	{
		printf( "%d",pDigits[0] );
		for( int i = 1; i < nDigits; i++ )
		{
			printf( " %d",pDigits[i]);
		}

	}

	
}
bool IsNumber( const char * p )
{
	for(const char * c = p ; *c; c++ )
	{
		if( !isdigit( *c ) && !(c==p && *c=='-' && *(c+1)))
			return false;
	}
	return true;
}
void FormatInput( char * in_out  )
{
	//if newline character found replace with NULL character
	char * c;
	if( (c  = strchr( in_out,'\n')))
		*c = '\0';
}
bool FormatToNumbers( const char * p_sz, uint8_t * noList, int nExpected, int rangeMin, int rangeMax, bool (*extCheck)(uint8_t*))
{
	assert( strlen(p_sz) < 255 - 1 );
	char tmp[MAXINPUTLEN];
	strcpy(tmp,p_sz);

	int number;
	char * p = strtok( tmp," ");

	for( int i = 0; i < nExpected; i++ )
	{
		if( !p )
		{
			printf("You did not enter enough numbers\n");
			return false;
		}
		else if( !IsNumber(p))
		{
			printf("One of the characters entered was not a digit \n");
			return false;
		}

		number = atoi(p);

		if( number < rangeMin )
		{
			printf( "One of the entered numbers was less than %d\n",rangeMin);
			return false;
		}
		else if( number > rangeMax )
		{
			printf( "One of the entered numbers was greater than %d\n",rangeMax);
			return false;
		}



		
		noList[i] = (uint8_t)number;
			

		p = strtok( NULL," ");
	
	}

	if(p )
	{
		printf("Too many numbers entered\n");
		return false;
	}

	if( extCheck )
		return extCheck( noList );
	else
		return true;
}