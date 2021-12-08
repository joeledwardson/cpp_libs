#ifndef NETWORK_H
#define NETWORK_H

#include "Platform.h"
#define _CRT_SECURE_NO_WARNINGS	//no warnings on using safe version of strcpy() etc
#include <assert.h>	//use of assert() in function calls
#include "Timer.h"	//used in BNP::Timeout_Timer
#include <ctype.h>	//isupper(), islower() BNP::Pack_Packet()
#include <vector>	//vector used for list of contacts
#include <stdio.h>	//usage of printf()
#include <stdint.h> //all intenger types e.g. uint16_t_t
#include <utility>	//std::pair
#include <string.h>	//strcpy(), memset()...
#include <stdlib.h>	//srand()..

#ifndef INVALID_SOCKET
#define INVALID_SOCKET ~0
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#if COMPILER == COMPILER_VS
#pragma comment(lib,"ws2_32.lib")
#include <winsock.h>
#include <urlmon.h>
#pragma comment( lib,"Urlmon.lib")

#else
#define NAMESIZE 20

#define closesocket close


#include <cstdarg>	//... used in param for JNP::Pack functions
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>	//exit()
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <cstring>

typedef sockaddr_in * PSOCKADDR_IN;
#endif




class BaseNetworking;

typedef const unsigned char		NID; //network ID  type
typedef const uint16_t	        NID_BIG;
typedef uint32_t PacketLen;

/*base networking class. used for game interaction for sending/recieving
data and controls address data for server. UDP NON BLOCKING SOCKETS ONLY */
class BaseNetworking
{
public:

	enum SocketType
	{
		SOCKET_HOST,
		SOCKET_CLIENT,
	};

	BaseNetworking(uint16_t nSockets);
	
	virtual ~BaseNetworking();
	
	/* sends data (buffer param) to a speficied address and port (toAddr param)
	with NULL char appended. If toAddr is NULL, function uses server
	structure for sending. INCLUDE 4 byte GAME INDENTIFYING byteS 
	returns > 0 on succes, -1 if failure. 
	*/
	int SendData( UINT sock, int size,const char* buffer,const sockaddr* toAddr = NULL);
	/* sends data for a bound socket. CANNOT BE HOST SOCKET. */
	
	/* recieves data from specified location (fromAddr). if fromAddr is NULL, function uses server address structure.
	returns NULL if no data	recieved. sock = index of socket to be used in socket list
	returns number of bytes revieced if all goes well, 0 if nothing recieved, and -1 if error 
	*/
	int RecieveData(UINT sock,int size,char * recvBuf, sockaddr* fromAddr = NULL );

	
	


	bool InitClientSocket(UINT sock, unsigned long IPAddress, UINT port );
	bool InitHostSocket( UINT sock, UINT port );
	bool DestroySocket( UINT sock );

	std::string GetError();

	std::wstring WGetError();

protected:
	
	bool IsErrorWait();
	
	void SetCustomErr( std::string err );

	std::string _GetSysError();
	void SetSysErr();

#if PLATFORM == PLATFORM_WINDOWS
	int sockAddrSize;
#else
    socklen_t sockAddrSize;
#endif

	UINT nSockets;
	int * sockets;
	SocketType * socketTypeList;
private:
	std::string customError;
	int sysErr;
};



//sample networking protocol
/*
namespace SNP
{
	NID_BIG port;

	enum UpdateTypes {..}

	//extra comm types (START AT 10 OR WILL CONFLICT WITH BASE CM TYPES FROM BNP)
}
*/

/******Joel's base networking protocol**********/
namespace BNP
{
	static const char * SVR_HOSTNAME = "joelyboy94.zapto.org";

	//see Networking.txt for detailed info on sending and receiving data protocols
		
	
	/************packet header*****/
	NID byte_CM						= 0; //Communications byte
	/**** heartbeat packets ***/
	NID byte_PN						= 1; //packet number byte. increments as program continues
	/*** update packets********/
	NID byte_UP						= 1; //ID of update
	NID byte_UT						= 2; //update type
	/****request packets******/
	NID byte_RQID					= 1; //id of request
	//sucess
	NID byte_RQHB					= 2; //heartbeat of sender (used in request AND reply)
	NID byte_RQUP					= 3; //current update ID of sender (used in request AND reply)
	//fail
	NID byte_RQDR					= 2; //deny reason
	/****************************/	
	NID byte_START					= 4; //position for start of packet after identifiers

	

	/******* timeouts  protocol (TO)*****/
	NID_BIG TO_SERVER					= 3500;
	NID_BIG	TO_CLIENT_CONNECT			= 2000;
	NID_BIG TO_GENERAL					= 3500;
	NID_BIG TO_SENDMILLI				= 50; //how often the program should send packets. set to 20ms so 50fps
	NID_BIG TO_HEARTBEAT				= 1000; //2fps send, does not require real time updating

	
	/* note that these are appended to sending and processed on receiving in
	SendData and RecieveData, do not attempt to use outside these functions */
	/************************/


	/******* communication protocols (CM). *********/
	/*subdivision of protocols
	standard protocols are CM_NORMREQUEST and CM_NORMCOMM. below are special replies
	format: (id protocol - see above. except replace ID with CM. proceeds with underscore and communication)
	eg. CM_HS_R_FULL: CM_HS_R = ID_HS_R for host server request (server reply to host in this example), but
	replacing ID with CM; this precedes _FULL, 	indicating server is full      */
	NID CM_HEARTBEAT				= 1; //Heartbeat packet

	NID CM_UPDATE_SND				= 2; //new update
	NID CM_UPDATE_RCV				= 3; //Acknowledgement of update

	NID CM_NORMREQUEST				= 4; //use for normal request, where above does not specify
	NID CM_REPLY_SUCCESS			= 5; //request accepted
	NID CM_REPLY_FAIL				= 6; //request denied


	/************************************************/
	 //gets external IP address and stores it in externalIP
	bool GetExternalIP(ULONG& externalIP);


	//gets IP address from host name, if it fails, returns 0
	ULONG GetIpFromHostName( const char * hostName );

	/*** packet destinations *****/
	
	/*****************************/

	template<class type>
	int FindItem( const type * firstEntry,type val,UINT memDif, UINT nItems)
	{
		const char * p = (const char*)firstEntry;
		for(UINT iIndex = 0; iIndex < nItems; iIndex++ )
		{
			if( !memcmp(p,&val,sizeof(type)))
			{
				return iIndex;
			}
			p += memDif;

		}
		return -1;
	}

	
	struct Timeout_Timer
	{
		Timeout_Timer( NID_BIG theTimeout );
		void ResetWatch();
		bool HasTimedOut();
	private:
		NID_BIG timeout;
		Timer timer;

	};

	struct HeartBeat
	{
		static const int size = byte_START;
		byte GetPacketID();
		void Update();
		HeartBeat( );
		const char * GetPacket();
	private:
		byte packetID;
		char sendBuffer[size];
	};

	

	struct Update
	{
		/*this ID is purely managed by IPComm, not sent to any contacts. it is a reference so that the udpateIDList
		contained by contacts contains IDs refering to this value in updates. the actual ID of the update is set when
		GetUpdate() is called*/
		const byte updateID;
		//update type MUST be <=255
		Update( byte& IDOfUpdate,NID updateType ,const char * szExData, UINT len, bool isForceSend);
		Update( byte& IDOfUpdate, const char * completeBuf, uint32_t len, bool isForceSend = false );
		~Update();
		Update& operator = ( const Update& u);
		Update( const Update& u );
		const char * GetUpdate( byte upID );
		const uint32_t packLen;
		const bool forceSend;
	protected:
		char * buf;
	};

	struct RcvUpdate
	{
		RcvUpdate( const char * data, PacketLen pLen );
		RcvUpdate( const RcvUpdate& s );
		RcvUpdate& operator=( const RcvUpdate& u );
		~RcvUpdate()
		{
			delete[] buf;
		}
		const PacketLen len;
		char * buf;

	};

	struct IPContact
	{
		enum UpdateList_Typ
		{
			UpdateList_Snd,
			UpdateList_Acpt
		};

		IPContact(const char * recvBuf , int sockID, bool isHostContact,sockaddr * s = NULL);

		~IPContact();
		
		IPContact& operator = ( const IPContact& c );

		const int socketID;
		
		const sockaddr * ContactIPData();

		//-1 = is valid but future packet
		//0 = invalid
		//1 = correct packet number =  next incremental ID
		int IsPacketValid( byte cmType, byte packetNumber );
		
		
		Timeout_Timer timeout;

		// upType = updateList_Snd: returns updateIDList size
		//upType = updateList_Acpt: returns updateAcceptList size
		uint32_t GetNUpdates( UpdateList_Typ upType );

		// upType = updateList_Snd: in list updateIDList - if isActual set to true returns ID of update to be sent
		//if isActual set to false returns reference ID of update to update List in IPCOMM
		//upType = updateList_Acpt: returns element at position index in updateAcceptList
		byte GetUpdateID( UpdateList_Typ upType, uint32_t index, bool isActual = false );
	

		//listType - UpdateList_Snd: if isActual is false, checks for reference update ID, if true checks for actual ID to be sent
		//UpdateList_Acpt: sees if updateID is already on the list of IDs to be sent as received
		int FindUpdate( byte updateID, UpdateList_Typ listType, bool isActual = false );

		//returns len if next update found. 0 if not
		uint32_t GetNxtUpdate( char * recvBuf );
	
		void ClearAcptList()
		{
			updateAcceptList.clear();
		}
		void AddUpdate( byte updateID )
		{
			slfUPID++;
			updateIDList.push_back( std::pair<byte,byte>( updateID, slfUPID) );
		}

		//returns true if processed. false if not
		bool ProcessRcvData( char * rcvBuf, int len ) ;
	private:
		std::vector<RcvUpdate>futureUpdates;	//received updates with ID too high, wait for expected ID before returning this to user
		std::vector<std::pair<byte,byte> >updateIDList;	//first - reference to ID of update in Update list, second - actual ID to be sent
		std::vector<byte>updateAcceptList;				//ID to be sent as received

		



		byte slfUPID; // current update ID for sending updates (increments when new ID for updateIDList is created)
		byte curUPID; // (contact's current Update ID)
		byte curHBID; // (contact's current heartbeat ID)
		sockaddr socketData;
		const bool hostContact;
		static const int nPacketLag = 20;
	};

	

	class IPComm
	{
	public:
		IPComm(UINT numberOfLists, int nConnections);
		
		virtual ~IPComm() = 0;

		void MoveContact( uint32_t srcL, uint32_t srcP, uint32_t dstL );

		//processes update: removes sent updates, and sends updates that have yet to be recieved
		bool Std_SndData();

		//return -1 for error. 0 for no data, >0 refers to length of packet
		int Std_RcvData( uint32_t lIndex, uint32_t pIndex, bool& processed, char * rcvBuf, int rcvLen);

		//returns current number of updates still to send
		int GetNUpdates()
		{
			return updateList.size();
		}

		void RemoveContact( UINT listIndex, UINT contactIndex );

		bool HasTimedOut( UINT listIndex, UINT contactIndex );

		int SendPacket(  UINT listIndex, UINT contactIndex , char * szBuffer, UINT len );

		void FormRequest( char * szMessage);

		//adds update to updates list. DOEST NOT set which contact to recieve update. InitCurUpdate() must be called to do this
		void AddUpdate( int updateType, const char * msg, UINT len , bool forceSend = false);

		//initialises current update to ALL contacts
		void InitCurUpdate();
		//initialises current update to all contacts in specified list
		void InitCurUpdate( UINT listIndex );
		//initialises current update to specific player
		void InitCurUpdate( UINT listIndex, UINT playerIndex );
		
		BaseNetworking networker;

		/*returns -1 if error
		returns 0 reply unsuccessful
		returns len if reply was successful

		pass index of socket for basenetworking list of sockets. protected because public
		access to this class should not be dealing with the socket list in basenetworking.

		pass buffer to send to server to rqstbuf, with rqstBufLen the length of the buffer
		pass buffer to recieve reply to rcvBuf, with max length for recieving (must be smaller than size of buffer) as rcvBufLen
		*/
		int Connect( bool& success,char& denyReason, int socketIndex, const char * rqstBuf, int rqstBufLen,int rcvBufLen, char * rcvBuf  );

		void AddList();
		void RmList(uint32_t listIndex);

	protected:
		UINT nLists;

		Timeout_Timer UPTimer;
		Timeout_Timer HBTimer;

		//current update ID, only used for Update ID rerence with contact clients, actual update id to be sent
		//is stored in the client itself
		byte curUpListID;

		HeartBeat heartbeat;

		
		/* add a contact with (acting as a client). listIndex = index of list to append with contact. recvBuf = buffer recieved
		from host. sockID = index of socket in basenetworking list. should be wrapped as public access should not
		be permitted to socket indexing within basenetworking*/
		void AddContact_Client(	UINT listIndex, const  char * recvBuf, int sockIndex );
		
		bool SendUpdates( std::vector< IPContact >& list ,bool forceSend);
		bool SendUpdateAccepts(  std::vector< IPContact >& list  );
		bool SendHeartBeats(  std::vector< IPContact >& list  );
		bool IsUpdateSent(  std::vector< IPContact >& list , UINT uIndex);

		std::vector<Update> updateList;
		std::vector< std::vector<IPContact> > contactLists;

		

		byte requestID;

	};

	class IPComm_HostType : public IPComm
	{
	public:

		IPComm_HostType(UINT nLists, int nConnections);

		virtual ~IPComm_HostType();
		
		PacketLen FormReplySuccess(  char * Buffer, const char * rqstBuf);
		PacketLen FormReplyFailure(	char * buffer, const char * rqstBuf, char denyReason );

		int SendPacket( char * buffer, int len, sockaddr socketData, int sockIndex );

	protected:
		/* add a contact with (acting as the host). listIndex = index of list to append with contact. recvBuf = buffer recieved
		when client sent reqest. sockID = index of socket in basenetworking list. should be wrapped as public access should not
		be permitted to socket indexing within basenetworking. socketData = IP data associated with client*/
		void AddContact_Host( 	UINT listIndex, const  char * recvBuf, int sockID, sockaddr socketData );
			

		bool FindContact( uint32_t& lIndex, uint32_t& pIndex, sockaddr * IPData );
		
	};

	//c = char, d = unsigned 16 bit decimal, i = unsigned 32 bit decimal s = string
	//normally function will convert host order to network order and visa versa. caps disables this
	static const char * StdFormats = "cdsi";
	uint32_t Pack_Packet( char * szBuf, const char * format, ... );
	uint32_t UnpackPacket( const char * szBuf, const char * format,...);
	
	typedef std::pair< PacketLen(*)(char* buf,char* element ),char> Format;
	void AddFormat( PacketLen (*func)(char* buf,char* element ),char c );


};




#endif
