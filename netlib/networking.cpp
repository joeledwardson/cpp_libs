
#include "networking.h"

void BNP::ZeroCore( PacketCore core ) {
	memset( core,'\0',byte_START );
}
char * BNP::GetIPAddress( sockaddr s )
{
	in_addr i;
#if PLATFORM == PLATFORM_WINDOWS
	i.S_un.S_addr = PSOCKADDR_IN( &s )->sin_addr.S_un.S_addr;
#else
	i.s_addr = PSOCKADDR_IN( &s )->sin_addr.s_addr;
#endif
	return inet_ntoa( i );
}
ULONG BNP::GetIpFromHostName( const char * hostName )
{
	
	sockaddr_in theIp;

	hostent * theHost;

	theHost =  	gethostbyname( hostName );
	if( theHost )
	{
		memcpy( (void*) &theIp.sin_addr,theHost->h_addr_list[0],theHost->h_length );
#if PLATFORM == PLATFORM_WINDOWS
		return theIp.sin_addr.S_un.S_addr;
#else
		return theIp.sin_addr.s_addr;
#endif
	}
	else {
		SetCritErr( GetSysError() );
		return 0;
	}
	
}
#if PLATFORM == PLATFORM_WINDOWS
bool BNP::GetExternalIP(ULONG& externalIP)
{

	

	struct IPWebsite
	{
		//szUniquePreIPText - CAN appear more than once. first appearance must directly preceed IP address string
		//afterIPChar - character directly proceeding ip address string
		char  * szUniquePreIPText, * afterIPChar;
		//szUrl - url of site to download
		char * szURL;
		IPWebsite( char * szURL, char * szUniquePreIPText, char * afterIPChar )
		{
			this->szURL = szURL;
			this->szUniquePreIPText = szUniquePreIPText;
			this->afterIPChar = afterIPChar;
			assert( strlen(afterIPChar) == 1 );
		}
		bool GetIPFromSite(ULONG& ipAddress)
		{
			char szPath[256];		//path of temp file contaning site HTML
			GetTempPath(256, szPath);	//store temporary path
			strcat(szPath, "my_ip.txt");	//add file name to path
			
			//function URLDOwnloadToFIle() if succeeds stores HTML in given path
			if( URLDownloadToFile( 0,szURL,szPath,0,0 ) == S_OK )
			{
				const int len = 3000;
				char szRead[len];		//buffer to store html
				ZeroMemory(szRead,len );

				char * Token;

				FILE * file = fopen( szPath,"r" );	//open file in read mode
				if( file )
				{
					int preLen = strlen(szUniquePreIPText);	//length of pre string

					fread(szRead,len,1,file );	//move HTML into read buffer

					fclose(file);	//close file
					remove(szPath);	//delete temp file containing html

					//function finds first appearance of pre-string in szRead and returns pointer to store in Token
					Token = strstr(szRead,szUniquePreIPText);	
					if( !Token )
						return false;

					Token = Token + preLen;	//token is incremented to after the pre-string (at the place of the ip address)
					//strtok() splits string where afterIPChar is found. therefore it should return a pointer to just the IP address
					Token = strtok( Token,afterIPChar );	
					if( !Token )
						return false;

					ULONG tempIP = inet_addr( Token );
					if( tempIP == INADDR_NONE || tempIP == 0 )	//if string is not a valid IP address then function has failed
						return false;
					else
					{
						ipAddress = tempIP;
						return true;
					}
				
				}
			}

			return false;

		}
	};


	// pointer to list of sites. 
	IPWebsite * sites;
	const int nSites = 3;
	sites = (IPWebsite*) malloc( sizeof( IPWebsite ) * nSites );

	sites[0] = IPWebsite(		"http://checkip.dyndns.org/Current IP Check.htm",
								": ",
								"<");
	sites[1] = IPWebsite(		"http://www.myip.ru/",
								"<TR><TD bgcolor=white align=center valign=middle>",
								"<" );
	sites[2] = IPWebsite(		"http://www.whatsmyip.us/",
								"class=\"ip\" onclick=\"copyClip()\" onmouseover=\"copyClip()\">\n",
								"<" );
	
	bool success = false;
	//loop through each site and try to get IP address
	for( int index = 0; index < nSites; index++ )
	{
		if( sites[index].GetIPFromSite( externalIP ))
		{
			success = true;
			break;	
			
		}
	}
	//free sites pointer
	free( sites );
	return success;
}
#endif

BNP::Timeout_Timer::Timeout_Timer( NID_BIG theTimeout )
	:timeout(theTimeout)
{

}
bool BNP::Timeout_Timer::HasTimedOut()
{
	if( timer.GetTimeMilli() < timeout)
		return false;
	else
	{
		timer.ResetWatch();
		return true;
	}
}
void BNP::Timeout_Timer::ResetWatch()
{
	timer.ResetWatch();
}


BNP::NetIPacket::~NetIPacket() {
}
BNP::NetIPacket::NetIPacket(const char * buf, unsigned int len)
	:StdIPacket(buf,len) {
	assert( len >= byte_START );
	place = byte_START;
}
void BNP::NetIPacket::GetCore(PacketCore core) const {
	assert( msgBuf.size() >= byte_START );
	memcpy( core, &msgBuf[0],byte_START );
}
BNP::NetIPacket::NetIPacket()
:StdIPacket( NULL,0 ) {
	place = 0;
}
void BNP::NetIPacket::Reset() {
	assert( len() >= byte_START );
	place = byte_START;
}
int64_t BNP::NetIPacket::GetTimeStamp() const{
	PacketCore core;
	GetCore(core);
	StdIPacket p( core+byte_TS_8,sizeof(uint64_t));
	uint64_t ts;
	p>>ts;
	return (int64_t)ts;
}

BNP::NetOPacket::~NetOPacket() {
}
BNP::NetOPacket::NetOPacket(const PacketCore core  )
	:StdOPacket( core, byte_START ) {
}
void BNP::NetOPacket::SetTimeStamp() {
	assert( msgBuf.size() >= byte_START );
	uint64_t ts=uint64_t( TimeStamp());
	StdOPacket p;
	p<<ts;
	memcpy( msgBuf.data() + byte_TS_8,p.GetBuf(),sizeof(uint64_t));
}
void BNP::NetOPacket::EditCore( unsigned int corePos, byte val ) {
	assert( corePos < byte_START );
	assert( len() >= byte_START );
	msgBuf[corePos] = val;
}

BNP::PacketNumber BNP::HeartBeat::GetPacketID() {
	return packetID;
}
void BNP::HeartBeat::Update() {
	packetID++;
	core[BNP::byte_PN] = packetID;
}
BNP::HeartBeat::HeartBeat( ) 
{
	ZeroCore(core);
	packetID = 0;
	core[BNP::byte_CM] = BNP::CM_HEARTBEAT;
	core[BNP::byte_PN] = packetID;
	
}
const char * BNP::HeartBeat::GetCore(){
	return core;
}

BNP::Update::Update(const  UpdateReference& IDOfUpdate,NID updateType , const StdOPacket& xDatPacket, bool isForceSend )
	:updateID( IDOfUpdate),
	 packLen( xDatPacket.len() + byte_START),
	forceSend( isForceSend ),
	p( GenCore(updateType))
{
	p << (StdOPacket&)xDatPacket;
	
}
BNP::Update::Update(const  UpdateReference& IDOfUpdate, const NetOPacket& tDatPacket, bool isForceSend )
	:updateID( IDOfUpdate ),
	packLen( tDatPacket.len() ),
	forceSend( isForceSend ),
	p(tDatPacket)
{

}
BNP::Update::~Update()
{

}
BNP::NetOPacket& BNP::Update::GetUpdate(UpdateNumber upID) {
	p.EditCore( byte_UP ,upID );
	return p;
}
BNP::NetOPacket BNP::Update::GenCore( NID updateType ) {
	PacketCore c;
	memset( c,'\0',byte_START );
	c[byte_CM] = CM_UPDATE_SND;
	//c[byte_UP] this is set when GetUpdate() is called
	c[byte_UT] = updateType;
	return NetOPacket(c);
}




BNP::ContPos::ContPos() {
	lIndex = 0;
	cIndex=0;
}
BNP::ContPos::ContPos( ListPos listIndex,ListPos contactIndex ) {
	lIndex = listIndex;
	cIndex = contactIndex;
}

BNP::RecvPacketProcess::~RecvPacketProcess() {
}