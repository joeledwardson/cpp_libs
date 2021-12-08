#ifndef NETWORK_H
#define NETWORK_H

#define _CRT_SECURE_NO_WARNINGS	//no warnings on using safe version of strcpy() etc
#include "packet.h"	//StdIPacket, StdOPacket
#include "stdlib/platform.h"	//linux or windows
#include "timer.h"	//used in BNP::Timeout_Timer
#include <assert.h>	//use of assert() in function calls
#include <ctype.h>	//isupper(), islower() BNP::Pack_Packet()
#include <vector>	//vector used for list of contacts
#include <stdint.h> //all intenger types e.g. uint16_t_t
#include <string.h>	//strcpy(), memset()...
#include <stdlib.h>	//srand()..
#include <limits>	//std::numeric_limits<T>::max()
#include "basenet.h"//basenetworking class
#include "stdlib/error.h"
#include <iostream> //TEMPORARY DURING DEVELOPMENT

#if COMPILER == COMPILER_VS
#ifdef max
#define _max(a,b) max(a,b)
#undef max	//conflicts with std::numeric_limits<T>::max()
#endif
#endif

#ifndef NETVBSE
#define NETVBSE std::cout
#endif


#if COMPILER == COMPILER_VS
#include <urlmon.h>
#pragma comment( lib,"Urlmon.lib")
#endif


//packet valid or not, see GNP::IPContact::IsPacketValid()
#define PV_VALID 1
#define PV_INVALID 0
#define PV_FUTURE -1

class BaseNetworking;

typedef const unsigned char	NID; //network ID  type
typedef const uint16_t      NID_BIG;




/******Joel's base networking protocol**********/
namespace BNP
{
	char * GetIPAddress( sockaddr s );
	

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
	NID byte_RQHB					= 2; //heartbeat of sender (used in request AND reply)
	//fail
	NID byte_RQDR					= 2; //deny reason
	/****************************/	
	NID byte_TS_8					= 4; //time stamp (8 bytes)
	NID byte_START					= 12; //position for start of packet after identifiers

	

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
	NID CM_QUIT						= 7;

	NID CM_BASETYPES_END			= 8; //end of base comm types - may extend from here
	/************************************************/
	
	NID_BIG RCV_MAX_SIZE			= 2000;

	typedef char	PacketCore[byte_START];
	void ZeroCore( PacketCore core );

	//gets external IP address and stores it in externalIP
	bool GetExternalIP(ULONG& externalIP);


	//gets IP address from host name, if it fails, returns 0
	ULONG GetIpFromHostName( const char * hostName );

	/*** packet destinations *****/
	
	/*****************************/

	class NetIPacket : public StdIPacket {
	public:
		int64_t GetTimeStamp() const;
		NetIPacket();
		virtual ~NetIPacket();
		NetIPacket(const char * buf, unsigned int len);
		void GetCore(PacketCore core) const;
		void Reset();
	};
	class NetOPacket : public StdOPacket {
	public:
		void SetTimeStamp();
		virtual ~NetOPacket();
		NetOPacket(const PacketCore core  );
		void EditCore( unsigned int corePos, byte val );
	};

	template <typename T> int FindElement( std::vector<T>& elementList, T item ) {
		for( unsigned int i=0;i<elementList.size();i++) {
			if( elementList[i]==item)
				return i;
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

	typedef unsigned short UpdateReference;
	typedef byte PacketNumber;
	typedef PacketNumber UpdateNumber;
	typedef unsigned int ListPos;

	struct HeartBeat {

		PacketNumber GetPacketID();
		void Update();
		HeartBeat( );
		const char * GetCore();
	private:
		PacketNumber packetID;
		PacketCore core;
		

	};

	struct Update
	{
		/*this ID is purely managed by IPComm, not sent to any contacts. it is a reference so that the udpateIDList
		contained by contacts contains IDs refering to this value in updates. the actual ID of the update is set when
		GetUpdate() is called*/
		const UpdateReference updateID;
		//update type MUST be <=255
		Update( const UpdateReference& IDOfUpdate,NID updateType , const StdOPacket& xDatPacket, bool isForceSend = false);	//just packet data passed
		Update( const UpdateReference& IDOfUpdate, const NetOPacket& tDatPacket, bool isForceSend = false );	//complete packet passed
		~Update();
		NetOPacket& GetUpdate( UpdateNumber upID );
		const uint32_t packLen;
		const bool forceSend;
		NetOPacket GenCore( NID updateType );
	protected:
		NetOPacket p;
	};

	class IPContact
	{
	public:

		IPContact(const NetIPacket& request , unsigned int sockID, bool isHostContact,sockaddr * s = NULL);

		~IPContact();

		const unsigned int socketID;
		
		const sockaddr * ContactIPData() const;

		//-1 = is valid but future packet
		//0 = invalid
		//1 = correct packet number =  next incremental ID
		// cmType == CM_HEARTBEAT or cmType == CM_UPDATE_SND 
		int IsPacketValid( byte cmType, PacketNumber packetID );
						
		unsigned int GetNUpdateAcpt();
		void ClearAcptList();
		int FindAcceptID( UpdateNumber updateAcptID );
		PacketNumber GetUpdateAcptID( unsigned int IDindex );

		unsigned int GetNUpdateSnd();
		int FindUpdate_RefID( UpdateReference ref);
		int FindUpdate_UPID( UpdateNumber updateID );
		UpdateReference GetUpdateRef( unsigned int refIndex );
		PacketNumber GetUpdateID( unsigned int IDIndex );


		//returns true if update found. 0 if not
		bool GetNxtUpdate( NetIPacket& packet );
	
		bool HasTimedOut();
		
		void AddUpdate( UpdateReference updateRef );

		//returns true if processed. false if not
		bool ProcessRcvData( const NetIPacket& packet) ;
	private:
		
		typedef std::pair< UpdateReference, UpdateNumber > Update_Ref_ID;
		std::vector<NetIPacket>futureUpdates;	//received updates with ID too high, wait for expected ID before returning this to user
		std::vector< Update_Ref_ID >updateIDList;	//first - reference to ID of update in Update list, second - actual ID to be sent
		std::vector<UpdateNumber>updateAcceptList;				//ID to be sent as received
		Timeout_Timer timeout;

		UpdateNumber slfUPID; // current update ID for sending updates (increments when new ID for updateIDList is created)
		UpdateNumber curUPID; // contact's current Update ID, i.e. this number will be the id of the packet recieved when contact sends update to us
		PacketNumber curHBID; // (contact's current heartbeat ID)
		sockaddr socketData;
		const bool hostContact;
		static const int nPacketLag = 20;	//number of packets allowed to fall behind until out of sync
		static const int size_Ref_ID = sizeof( Update_Ref_ID );
	};

	struct ContPos {
		ContPos();
		ContPos( ListPos listIndex,ListPos contactIndex );
		ListPos lIndex,cIndex;
	};

	class IPComm {
	public:
		IPComm(unsigned int numberOfLists, std::vector<SockData> socketList);
		
		virtual ~IPComm();

		void MoveContact( ContPos source, ListPos dstL );

		//processes update: removes sent updates, and sends updates that have yet to be recieved
		void Std_SndData();

		//return -1 for error. 0 for no data, >0 refers to length of packet
		//this function ONLY returns >0 for  VALID RECEIEVED UPDATE, returns len
		//data stored in packet on success
		//IMPORTANT* - packet must be allocated sufficient memory for receiving data	
		int Std_RcvData( ContPos pos,  NetIPacket& packet,bool& prc);

		void RemoveContact( ContPos pos );

		/* add a contact with (acting as a client). listIndex = index of list to append with contact. recvBuf = buffer recieved
		from host. sockID = index of socket in basenetworking list. should be wrapped as public access should not
		be permitted to socket indexing within basenetworking*/
		void AddContact_Client(	ListPos listIndex, const NetIPacket& request, unsigned int sockIndex );


		bool HasTimedOut( ContPos pos );

		int SendPacket(  const IPContact& contact ,  NetOPacket& packet ) ;
		int SendPacket( unsigned int socketIndex,  NetOPacket& packet, const sockaddr * to=NULL );
		int SendPacket(const IPContact& contact, const PacketCore c ) ;
	

		//adds update to updates list. DOEST NOT set which contact to recieve update. InitCurUpdate() must be called to do this
		void AddUpdate( int updateType, const StdOPacket& packet , bool forceSend = false);

		//initialises current update to ALL contacts
		void InitCurUpdate();
		//initialises current update to all contacts in specified list
		void InitCurUpdate( ListPos listIndex );
		//initialises current update to specific player
		void InitCurUpdate( ContPos pos );
		
		
		/*returns true if successful, false if not

		pass index of socket for basenetworking list of sockets. protected because public
		access to this class should not be dealing with the socket list in basenetworking.

		parameter 'usrOut' contains only user data AFTER byte_START
		reply stored in parameter 'in', ensure 'in' has SUFFICIENT memory allocated
		*/
		bool Connect( unsigned int socketIndex, byte& denyReason, const StdOPacket& usrOut, NetIPacket& in  );

		NetOPacket FormRequest();

		void AddList();
		void RmList(ListPos listIndex);

	protected:
		
		class ReceievePacket : public NetIPacket {
		public:
			ReceievePacket();
			char * GetInBuf();
		};
		/*returns:
		0: nothing receieved,
		-1: packet too small
		>0: len*/
		int ReceiveData( unsigned int socketIndex, ReceievePacket& in,sockaddr*src=NULL );
		
		bool ContactInRange( ContPos pos ) const;
		IPContact * GetContact( ContPos pos ) const;
		BaseNetworking networker;

		unsigned int nLists;

		Timeout_Timer UPTimer;
		Timeout_Timer HBTimer;

		//current update ID, only used for Update ID rerence with contact clients, actual update id to be sent
		//is stored in the client itself
		byte curUpListID;

		HeartBeat heartbeat;
				
		void SendUpdates(	std::vector<IPContact*>& list ,bool forceSend);
		void SendUpdateAccepts( std::vector<IPContact*>& list  );
		void SendHeartBeats(  std::vector<IPContact*>& list );
		bool IsUpdateSent(  std::vector<IPContact*>& list, UpdateReference ref);

		std::vector<Update*> updateList;
		std::vector< std::vector<IPContact*> > contactLists;
		
		PacketNumber requestID;
		ReceievePacket inputPacket;

	};

	class IPComm_HostType : public IPComm{
	public:
		unsigned int GetNLists();

		IPComm_HostType(unsigned int numberOfLists, std::vector<SockData> socketList);

		virtual ~IPComm_HostType();
		
		NetOPacket FormReplySuccess(  	NetIPacket& request);
		NetOPacket FormReplyFailure(	NetIPacket& request, char denyReason );

		/* add a contact with (acting as the host). listIndex = index of list to append with contact. recvBuf = buffer recieved
		when client sent reqest. sockID = index of socket in basenetworking list. should be wrapped as public access should not
		be permitted to socket indexing within basenetworking. socketData = IP data associated with client
		IMPORTANT - USES SOCKADDR FROM MOST RECENT RECVDATA() CALLED
		*/
		void AddContact_Host( 	UINT listIndex, const NetIPacket& request, unsigned int sockID, sockaddr socketData );

		/*returns:
		-1: packet receieved but is invalid, or has been processed.
		0: nothing receieved,
		>0: length, (also stored in packet.len() )*/
		int RecvData( ContPos& pos, unsigned int sock, bool& isConnected, NetIPacket& packet );
		sockaddr GetSockAddr();
	protected:
			
		bool FindContact( ContPos& pos, sockaddr * IPData );
		sockaddr sData;
	};

	class IPComm_StdClient : public IPComm {
	public:
		IPComm_StdClient( unsigned long ipAddr, unsigned short port );
		~IPComm_StdClient();
		void Std_SndData();
		int Std_RcvData(NetIPacket& packet,bool& prc);
		bool HasTimedOut();
		int SendPacket(  NetOPacket& packet ) ;
		void AddUpdate( int updateType, const StdOPacket& packet , bool forceSend = false);
		void InitCurUpdate();
		bool Connect( byte& denyReason, const StdOPacket& usrOut, NetIPacket& in  );
	private:
		static const ContPos contactPos;
		std::vector<SockData>GetSockList(unsigned long ipAddr, unsigned short port );
	};

	class RecvPacketProcess {
	public:
		virtual bool ThisProcess( byte cmType, byte updateType, bool isConnected ) = 0;
		virtual ~RecvPacketProcess();
	};
	
	
};




#endif
