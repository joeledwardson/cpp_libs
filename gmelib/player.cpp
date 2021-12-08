#include "player.h"

StdKey::StdKey( int _keyID ) {
	keyID = _keyID;
	isChr = isalpha(keyID)!=0;
	if( isChr )
		assert( isupper(keyID));
}
bool StdKey::ThisKey( int inputKey ) {
	return inputKey == keyID || (isChr && tolower(keyID)==inputKey);
}


Player_Base::Player_Base(std::string nameOfPlayer) {
	name = nameOfPlayer;
}
Player_Base::Player_Base() {
}
Player_Base::~Player_Base() {
}
	


std::string GenerateRandName( ) {
	srand( (unsigned int) time(NULL) );

	std::string nameList[] = {	
					"BECAUSE I'M BATMAN",
					"Overturned hedgehog",
					"Darth maul",
					"epic bacon",
					"A burnt crumpet",
					"ignorant pumpkin",
					"confused penguin",
					"bent.I.am",
					"bob",
					"general taboo",
					"the pootis man",
					"Robert F. Aggot" };

	int i = rand() % (sizeof(nameList)/sizeof(std::string));

	return nameList[i];
}
