#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <cctype>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

class Key {
public:
	virtual bool ThisKey( int inputKey ) = 0;
};
class StdKey : public Key{
public:
	StdKey( int _keyID );
	bool ThisKey( int inputKey );
private:
	bool isChr;
	int keyID;
};
class KeyList {
public:
	virtual std::vector< Key* > GetKeyList();
	static void DelKeyList( std::vector<Key* > key_List );
};

std::string GenerateRandName();

class Player_Base {
public:
	
	Player_Base( std::string nameOfPlayer );
	Player_Base();
	virtual ~Player_Base();

	std::string name;
};


#endif
