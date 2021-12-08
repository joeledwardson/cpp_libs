#include "error.h"
#include <vector>

//critical error
std::string critError;
class Expn_Critical : public std::exception {
public:
	virtual const char* what() const throw();
};
const char* Expn_Critical::what() const throw() {
	return critError.c_str();
}
void SetCritErr( std::string err ) {
	critError = err;
	throw Expn_Critical();
}


//non critical errors
std::vector< JSTRING > errorList;

JSTRING GetLastErr(  ) {
	if( errorList.size() )
		return errorList.back();
	else
#ifdef WIDESTRINGS
		return JSTRING( L"no error to retrieve!" );
#else
		return JSTRING( "no error to retrieve!" );
#endif
}
void RmLastErr() {
	if( errorList.size() )
		errorList.erase( errorList.end()-1 );
}
void AddErr( JSTRING err ) {
	errorList.push_back( err );
}
bool IsErr() {
	return errorList.size() > 0;
}

std::string GetSysError() {
	

#if PLATFORM==PLATFORM_WINDOWS
	int errNum = GetLastError();
	std::string err;	//MUST be created after getlasterror() is called or error number will be reset to 0
	char* msg;
	HMODULE lib = NULL;
	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					0,
					(LPCVOID)lib,
					errNum,
					0, // language ID
					(LPSTR)&msg,
					0, // size ignored
					NULL); // arglist


	err =  std::string(msg);
#else
	std::string err;
	err = std::string( strerror( errno ) );
#endif

	return err;
}