#pragma once
/************************************************************************/
/*								Define		                            */
/************************************************************************/

// LENGTH
#define LENGTH_DEFAULTSTRING				512
#define LENGTH_DEFAULTDATE					64
#define LENGTH_DEFAULTFILENAME				128

#define LENGTH_DEFAULTTINYSTUFF				16

#define LENGTH_ADDRESS						64

// SIZE
#define SIZE_DEFAULTPACKETDATA				1024

// CYCLE
#define	CYCLE_DEFAULT						1

/************************************************************************/
/*								Enumeration                             */
/************************************************************************/
// LOG
enum LogType
{
	Managed,
	Unmanaged
};

// Overlapped
enum SeedOverlappedIoOption
{
	IoSend,
	IoRecv
};