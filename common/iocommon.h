
#pragma once
/**************************************************************************
	created:	2012/11/16	14:38	
	filename: 	iocommon.H
	file path:	E:\Documents\Visual Studio 2008\Projects\WinSock\common
	author:		DAILM, en_name: Dicky

	purpose:	
**************************************************************************/

//-------------------------------------------
// define io data struct
typedef struct _PTUSERIO_OID_DATA 
{
	ULONG           Oid;
	ULONG           Length;
	UCHAR           Data[1];
}PTUSERIO_OID_DATA, *PPTUSERIO_OID_DATA;


typedef struct _PassthruStatistics	
{
	ULONG    nMPSendPktsCt;			// package by MPSendPackets 
	ULONG    nMPSendPktsDropped;	// drop a packet in MPSendPackets
	ULONG    nPTRcvCt;				// package by pPTReceive
	ULONG    nPTRcvDropped;			// drop a packet in PTReceive
	ULONG    nPTRcvPktCt;			// package by PTReceivePacket
	ULONG    nPTRcvPktDropped;		// drop a packet in PTReceivePacket
}PassthruStatistics, *PPassthruStatistics;

typedef struct _PassthruFilter		// filter rules
{
	USHORT protocol;				// used protocol

	ULONG sourceIP;				
	ULONG sourceMask;				// mask

	ULONG destinationIP;		
	ULONG destinationMask;		

	USHORT sourcePort;			
	USHORT destinationPort;		
	BOOLEAN bDrop;					// whether drop the packet

}PassthruFilter, *PPassthruFilter;


//-------------------------------------------
//  define IO control code
#define FSCTL_PTUSERIO_BASE      FILE_DEVICE_NETWORK
// enumeration adapter
#define IOCTL_PTUSERIO_ENUMERATE   \
	CTL_CODE(FSCTL_PTUSERIO_BASE, 0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// open a adapter
#define IOCTL_PTUSERIO_OPEN_ADAPTER   \
	CTL_CODE(FSCTL_PTUSERIO_BASE, 0x202, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_PTUSERIO_SET_OID	\
	CTL_CODE(FSCTL_PTUSERIO_BASE, 0x203, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_PTUSERIO_QUERY_OID  \
	CTL_CODE(FSCTL_PTUSERIO_BASE, 0x204, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)



// achieve net act statistics
#define IOCTL_PTUSERIO_QUERY_STATISTICS   \
	CTL_CODE(FSCTL_PTUSERIO_BASE, \
	0x205, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// reset net activity
#define IOCTL_PTUSERIO_RESET_STATISTICS   \
	CTL_CODE(FSCTL_PTUSERIO_BASE, \
	0x206, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// add a filter rule
#define IOCTL_PTUSERIO_ADD_FILTER			\
	CTL_CODE(FSCTL_PTUSERIO_BASE,	\
	0x207, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// clear filter rules
#define IOCTL_PTUSERIO_CLEAR_FILTER			\
	CTL_CODE(FSCTL_PTUSERIO_BASE,	\
	0x208, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)







