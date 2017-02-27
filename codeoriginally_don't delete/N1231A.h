/*
 * Copyright 2002 by Agilent Technologies
 *
 * File Name: N1231A.h
 *
 * Header file for the API library for the 
 * Agilent Technologies N1231A PCI Three-Axis Board 
 * designed for use with an Agilent Technologies 
 * laser interferometry position measurement systems.
 * 
 * See also file N1231A_reg.h
 */ 

#ifndef N1231A_H
#define N1231A_H

#include "N1231A_reg.h"

/*
 ****************************************************************
 * C/C++ Compatibility
 ********************************
 */
#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************
 * Export when building DLL, Import in client applications
 ********************************
 */
#ifdef USE_STATIC	// Should not be defined if used with N1231A.dll
#define EXPORT		// Do nothing
#else
#ifdef MAKE_DLL
#define EXPORT	__declspec(dllexport)
#else
#define EXPORT  __declspec(dllimport)
#endif
#endif

/* 
 ************************************
 ************************************
 * DEFINITIONS OF TERMS :
 *
 *  Raw Interferometer Units:
 *    Position and velocity values used by these 
 *    functions are expressed in Raw Interferometer Units.
 *
 *    For Position Information:
 *      Raw Interferometer Position Unit 
 *          = Lambda / (512 * FoldFactor)
 *
 *    For Velocity Information:
 *      Raw Interferometer Velocity Unit 
 *          = (Lambda * 20E6 Hz) / (2^22 * FoldFactor)
 *
 *    Where:
 *      Lambda = Wavelength of Light 
 *      FoldFactor = 
 *        2 for a Linear Optics System,
 *        4 for a Plane Mirror Optics System, or
 *        8 for a High Resolution Optics System.
 *
 ************************************
 ************************************
 */

/*
 ************************************
 ************************************
 * TABLE OF CONTENTS
 ************************************
 ************************************
 *
 * ==================================
 * Typedefs, Constants, Enums, Structures
 * ==================================
 *
 * Used for opening device:
 *		N1231A_HANDLE
 *		N1231A_IGNORE_FIELD
 *		N1231A_LOCATION
 *
 * Used for interrupts
 *		N1231A_EVT_HANDLE;
 *
 * Used for error returns:
 *		N1231A_RETURN
 *
 * Used for axis identification
 *		N1231A_AXIS
 *
 * ==================================
 * High Level Functions
 * ==================================
 *
 * N1231AOpen()
 * N1231ADefaultDevice()
 * N1231AClose()
 * N1231AFind()
 * N1231APresetRawAll()
 * N1231APresetRaw()
 * N1231AGetRawPosVelAll()
 * N1231AGetRawPosVel()
 * N1231AClearStatusBits()
 * N1231AClearStatusAll()
 * N1231AGetStatus()
 * N1231ASetInterruptMask()
 * N1231AGetInterruptMask()
 * N1231ASetConfig()
 * N1231AGetConfig()
 * N1231ASetFilter()
 * N1231AGetFilter()
 * N1231AGetFPGARevs()
 * N1231AGetFPGAIds()
 * N1231AGetHdwRev( )
 * N1231AGetLibraryRev()
 * N1231AGetErrStr()
 *
 * ==================================
 * Interrupt Related Functions
 * ==================================
 *
 * N1231APciInterruptEnable()
 * N1231APciInterruptAttach()
 *
 * ==================================
 * Lower Level Functions
 * ==================================
 *
 * N1231ASamplePosVel()
 * N1231AReadRawPos()
 * N1231AReadRawVel()
 * N1231AWriteRawPresetRegister()
 * N1231AGetRawPresetRegister()
 *
 * ==================================
 * Register Functions (for test use only)
 * ==================================
 *
 * N1231AWriteRegisterLong()
 * N1231AReadRegisterLong()
 * N1231AWriteRegisterWord()
 * N1231AReadRegisterWord()
 *
 */

/*
 ************************************
 ************************************
 * DECLARATIONS
 ************************************
 ************************************
 */

 /*
 ****************************************************************
 * Typedefs, Constants, Enums, Structures
 ********************************
 */

/*
 * Used for opening device
 */

typedef void *N1231A_HANDLE;

#define N1231A_IGNORE_FIELD	0xffffffff

typedef struct
{
	/*
	 * Set following fields to identify device to open.
	 * Values set to IGNORE_FIELD (see above) will not be used
	 * when searching for a device, but will be filled in
	 * by the search or open function once a device is found.
	 */
	unsigned long	BusNumber;
	unsigned long	SlotNumber;
} N1231A_LOCATION;

/*
 * Used for interrupts
 */

typedef void *N1231A_EVT_HANDLE;

/*
 * Used for error returns
 */
typedef short N1231A_RETURN;
enum E_N1231A_RETURN
{
	N1231A_SUCCESS		= 0,
	N1231A_ERR_HANDLE	= 16, /* Leave a space for bitmap-type errors */
	N1231A_ERR_PARAM,
	N1231A_ERR_DEVICE,
	N1231A_ERR_DRIVER,
	N1231A_ERR_MEMORY,		/* Insufficient mem for device */
	N1231A_ERR_REG,			/* Invalid register address */
	N1231A_ERR_HEAP,		/* No space on heap */
	N1231A_ERR_HEAP_LOCKED,	/* Should never occur */
	N1231A_ERR_BAD_AXIS,	/* Axis does not exist */
	N1231A_ERR_AXIS,		/* Axis or reference has status error or invalid data */
	N1231A_ERR_UNKNOWN1,	/* Should never occur */
	N1231A_ERR_UNKNOWN2,	/* Should never occur */
	N1231A_ERR_UNKNOWN3,	/* Should never occur */
	N1231A_ERR_UNKNOWN4,	/* Should never occur */
	N1231A_ERR_UNKNOWN5,	/* Should never occur */
	N1231A_ERR_UNKNOWN6,	/* Should never occur */
	N1231A_ERR_MAX_EVENTS
};

/*
 * Used for axis identification
 */
typedef short N1231A_AXIS;
enum E_N1231A_AXIS
{
	AXIS_1,
	AXIS_2,
	AXIS_3	
};

/*
 *********************************************
 *********************************************
 * FUNCTION DECLARATIONS
 *********************************************
 *********************************************
 */

/*
 * ==================================
 * High Level Functions
 * ==================================
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AOpen(N1231A_LOCATION	*pDevice,
		   N1231A_HANDLE	*pN1231AHandle,
		   unsigned long	*pProductId);
/*
 *	Opens N1231A-type device specified by structure *pDevice.
 *
 *	If pDevice points to NULL, then this function will open the
 *	first N1231A-type device that is found.  
 *
 *	If pDevice points to a structure of type N1231A_LOCATION 
 *  then this function will open the first N1231A-type device 
 *	matching the information in pDevice.  Upon successful 
 *	completion of this function any incomplete fields in 
 *	*pDevice will have been filled in (to show the bus and  
 *	slot of the device opened).  
 *
 *	An N1231A-type device is any PCI device with the Agilent 
 *	Vendor ID (0x15bc) and Device ID = 0x0a00.  Future Agilent
 *	axis cards may use these same IDs, but are expected to be
 *	distinguished by the ProductId (see below). 
 *
 *	Parameter *pN1231AHandle returns a handle required by other 
 *	functions to reference the opened device.  
 *
 *	If pProductId is not NULL, then *pProductId will be set
 *	to the ID code of FPGA1. For an N1231A card this value should 
 *	be 0x0001231A.  If other axis cards are present, this code can be 
 *	used to verify that the correct card model was opened.
 *	NOTE THAT *pProductId is used only to return a value -- its 
 *	value upon entry to this function is ignored.
 *
 *	Before exiting from the application which called this function,
 *	a call should be made to N1231AClose() to release the handle and 
 *	its associated memory block.
 *
 *  The function N1231ADefaultDevice() can be called to initialize 
 *  pDevice to find the first N1231A.  Calling N1231AOpen() using 
 *  this default pDevice is the same as calling it with a pointer 
 *  to NULL, except that supplying a pointer to the N1231A_LOCATION
 *  structure allows return of information on the device found.
 *
 *	The return code specifies the success or failure of this function,
 *	as follows:
 *		N1231A_SUCCESS:		The function returned successfully.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_DEVICE:	No device found.
 *		N1231A_ERR_DRIVER:	No device driver installed.
 *		N1231A_ERR_MEMORY:	Insufficient memory.
 *		N1231A_ERR_HEAP:	No space on heap.
 */

/************************************************************************/
EXPORT void
N1231ADefaultDevice(N1231A_LOCATION	*pDevice);
/*
 *	Fills structure *pDevice with default values. 
 *	Using these default values with N1231AOpen() will cause the 
 *	first N1231A in the system to be opened.  If there is only 
 *	one N1231A in the system, this provides a simple method
 *	for opening the device.  If there are multiple N1231A cards
 *	then after calling this function it will be necessary to 
 *	modify one or more fields to specify the particular card 
 *	to be opened.
 * 
 * EXAMPLE:
 *  pDevice->BusNumber	= IGNORE_FIELD;
 *  pDevice->SlotNumber	= IGNORE_FIELD;
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AClose(N1231A_HANDLE	*pN1231AHandle);
/*
 *	Closes the N1231A card specified by *pN1231AHandle.
 *  If successful, *pN1231AHandle is set to NULL.
 *
 *	The return code specifies the success of failure of this function,
 *	as follows:
 *		N1231A_SUCCESS:		The function returned successfully
 *      N1231A_PARAM:		A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid
 *		N1231A_ERR_DEVICE:	Device not closed properly.
 *		N1231A_ERR_HEAP:	Error accessing heap.
 */

/************************************************************************/
EXPORT N1231A_RETURN 
N1231AFind(const N1231A_LOCATION	*pDevice,
		   unsigned int				*pNumFound,
		   N1231A_LOCATION			*pDeviceArray,
		   unsigned int				numMax);
/*
 *	Determines the number of N1231A devices in the system 
 *	and (optionally) builds an array of information on 
 *	these devices.
 *
 *	If numMax is zero and pDeviceArray is NULL, then reports
 *	total number of N1231A devices matching the pDevice description.
 *
 *	Otherwise, builds an array (at pDeviceArray) of the first numMax
 *	devices found.
 *
 *  PARAMETERS:
 *		*pDevice:
 *			Pointer to structure of type N1231A_LOCATION.
 *			If NULL, search will find all N1231A devices.
 *			If non-NULL, search will be limited to devices
 *				matching the specified criteria.
 *		*pNumFound
 *			Returns number of devices found.
 *			If numMax is zero and pDeviceArray is NULL then
 *				this will be the total number of devices matching
 *				the description.
 *			Otherwise, this number will not exceed numMax.
 *		*pDeviceArray
 *			Pointer to array of structures of type N1231A_LOCATION
 *			used to return information on found devices.  If NULL, 
 *			then numMax must be zero.
 *		*numMax
 *			Maximum number of devices to be reported.  The array 
 *			at pDeviceArray must be at least this large. 
 *			If pDeviceArray is NULL, then numMax must be zero.
 *
 *	ERROR CODES:
 *	  The return code specifies the success of failure of this function,
 *	  as follows:
 *		N1231A_SUCCESS:		The function returned successfully.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_DEVICE:	No device found.
 *		N1231A_ERR_DRIVER:	No device driver installed.
 *		N1231A_ERR_MEMORY:	Insufficient memory.
 *	  In case of error, *pNumFound will be set to zero.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231APresetRawAll(N1231A_HANDLE	N1231AHandle,
			       long				preset1,
			       long				preset2,
			       long				preset3,
				   unsigned short	*pStatus);
/*
 *	For the card specified by N1231AHandle, attempts to  
 *	perform the following:
 *
 *	a) Reset all errors on axis 1, 2, and 3 and the Ref axes.
 *	b) Load the Preset Position registers for each axis 
 *     with preset1, preset2, and preset3.
 *	c) Transfer these Preset Position values to the Position registers
 *     (i.e., perform a Postion Preset operation).
 *	d) Set *pStatus to show contents of the status registers.
 *
 *	The preset values are in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS:	An axis still shows a status error.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231APresetRaw(N1231A_HANDLE	N1231AHandle,
				N1231A_AXIS		axis,
			    long			preset,
			    unsigned short	*pStatus);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis, attempts to perform the following:
 *
 *	a) Load the Preset Position register with preset,
 *	b) Reset all errors on the specified axis and the ref axis.
 *	c) Transfer the Preset Position value to the Position register
 *     (i.e., perform a Position Preset operation).
 *	d) Set *pStatus to show contents of the status registers.
 *
 *	The preset value is in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS:	The axis still shows a status error.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetRawPosVelAll(N1231A_HANDLE		N1231AHandle,
			   	      long				*pPosition1,
				      long				*pVelocity1,
			   	      long				*pPosition2,
				      long				*pVelocity2,
				      long				*pPosition3,
				      long				*pVelocity3,
					  unsigned short	*pValid);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Samples position and velocity on axis 1, 2, and 3.
 *	b) Reads position and velocity values on axis 1, 2, and 3
 *     into the corresponding parameters.
 *
 *	If an error has occurred on an axis, the position and 
 *	velocity parameters for that axis will be left unchanged.
 *
 *	The contents of the sample register are returned in *pValid.
 *	Therefore, if N1231A_ERR_AXIS is returned, the *pValid 
 *	parameter can be used to determine which axis were invalid.
 *	The constants:
 *		N1231A_VALID_1
 *		N1231A_VALID_2
 *		N1231A_VALID_3
 *	can be used to assist in analyzing *pValid.
 *
 *	The position and velocity values are in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS:	Data was not valid on at least one axis
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetRawPosVel(N1231A_HANDLE	N1231AHandle,
				   N1231A_AXIS		axis,
			   	   long				*pPosition,
				   long				*pVelocity);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis, attempts to perform the following:
 *
 *	a) Samples position and velocity on the specified axis.
 *	b) Reads position and velocity values on the specified axis
 *     into the corresponding parameters.
 *
 *	If an error has occurred on the specified axis, the position and 
 *	velocity parameters will be left unchanged.
 *
 *	The position and velocity values are in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS:	Data invalid on specified axis.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AClearStatusBits(N1231A_HANDLE		N1231AHandle,
				      unsigned short	resetBits,
				      unsigned short	*pStatus);
/*
 *	For the card specified by N1231AHandle:
 *   
 *	a) Attempts to clear specific bits (resetBits) in status registers
 *  b) Sets *pStatus to the new values of the status registers
 *
 *	NOTE: Although two separate hardware registers are used for status,
 *	      this function packs them into a single *pStatus register.
 *
 *	The following constants can be used to clear and 
 *  interpret the bits in *pStatus:
 *
 *		N1231A_NO_SIG_1
 *		N1231A_GLITCH_1
 *		N1231A_NO_SIG_2
 *		N1231A_GLITCH_2
 *		N1231A_NO_SIG_3
 *		N1231A_GLITCH_3
 *		N1231A_NO_SIG_REF
 *		N1231A_GLITCH_REF
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AClearStatusAll(N1231A_HANDLE	N1231AHandle,
					unsigned short	*pStatus);
/*
 *	For the card specified by N1231AHandle:
 *   
 *	a) Attempts to clear the status registers
 *  b) Sets *pStatus to the show the new values of the status registers
 *
 *	NOTE: Although two separate hardware registers are used for status,
 *	      this function packs them into a single *pStatus register.
 *
 *	The following constants can be used to interpret the 
 *	bits in *pStatus:
 *
 *		N1231A_NO_SIG_1
 *		N1231A_GLITCH_1
 *		N1231A_NO_SIG_2
 *		N1231A_GLITCH_2
 *		N1231A_NO_SIG_3
 *		N1231A_GLITCH_3
 *		N1231A_NO_SIG_REF
 *		N1231A_GLITCH_REF
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetStatus(N1231A_HANDLE	N1231AHandle,
				unsigned short	*pStatus);
/* 
 *	For the card specified by N1231AHandle:
 *   
 *	a) Sets *pStatus to show the contents of the status registers.
 *
 *	NOTE: Although two separate hardware registers are used for status,
 *	      this function packs them into a single *pStatus register.
 *
 *	The following constants can be used to interpret the 
 *	bits in *pStatus:
 *
 *		N1231A_NO_SIG_1
 *		N1231A_GLITCH_1
 *		N1231A_NO_SIG_2
 *		N1231A_GLITCH_2
 *		N1231A_NO_SIG_3
 *		N1231A_GLITCH_3
 *		N1231A_NO_SIG_REF
 *		N1231A_GLITCH_REF
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231ASetInterruptMask(N1231A_HANDLE	N1231AHandle,
					   unsigned short	intrMask);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets the interrupt mask register.
 *
 *  The following constants can be used for IntrMask:
 *		N1231A_INTR_NO_SIG_1
 *		N1231A_INTR_GLITCH_1
 *		N1231A_INTR_NO_SIG_2
 *		N1231A_INTR_GLITCH_2
 *		N1231A_INTR_NO_SIG_3
 *		N1231A_INTR_GLITCH_3
 *		N1231A_INTR_NO_SIG_REF
 *		N1231A_INTR_GLITCH_REF
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetInterruptMask(N1231A_HANDLE	N1231AHandle,
					   unsigned short	*pIntrMask);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets *pIntrMask to show contents of the 
 *     interrupt mask register.
 *
 *	The following constants cam be used to interpret bits in *pIntrMask:
 *		N1231A_INTR_NO_SIG_1
 *		N1231A_INTR_GLITCH_1
 *		N1231A_INTR_NO_SIG_2
 *		N1231A_INTR_GLITCH_2
 *		N1231A_INTR_NO_SIG_3
 *		N1231A_INTR_GLITCH_3
 *		N1231A_INTR_NO_SIG_REF
 *		N1231A_INTR_GLITCH_REF
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231ASetConfig(N1231A_HANDLE	N1231AHandle,
				unsigned short	config);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets the configuration registers.
 *
 *	NOTE: Although two separate hardware registers are used for config,
 *	      this function packs them into a single config register.
 *
 *	The following constants can be used for config: 
 *		N1231A_DIR_MINUS_1
 *		N1231A_DIR_MINUS_2
 *		N1231A_DIR_MINUS_3
 *		N1231A_IRQ_ENB
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetConfig(N1231A_HANDLE	N1231AHandle,
				unsigned short	*pConfig);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets *pConfig to show contents of the 
 *     configuration registers.
 *
 *	NOTE: Although two separate hardware registers are used for config,
 *	      this function packs them into a single *pconfig register.
 *
 *	The same constants shown under N1231ASetConfig()
 *	can be used to interpret the bits in the word.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231ASetFilter(N1231A_HANDLE	N1231AHandle,
			    unsigned short	filter);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets the filter register.
 *
 *	The following constants can be used for filter: 
 *
 *		N1231A_FILTER_ENB
 *
 *		N1231A_KP0
 *		N1231A_KP1
 *		N1231A_KP2
 *		N1231A_KP3
 *
 *		N1231A_KV0
 *		N1231A_KV1
 *		N1231A_KV2
 *		N1231A_KV3
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetFilter(N1231A_HANDLE	N1231AHandle,
			    unsigned short	*pFilter);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets *pFilter to show contents of the filter register.
 *
 *	The same constants shown under N1231ASetFilter()
 *	can be used to interpret the bits in the word.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetFPGARevs(N1231A_HANDLE	N1231AHandle,
				  unsigned long	*pFPGA1Rev,
			  	  unsigned long	*pFPGA2Rev);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets *pFPGA1Rev and *pFPGA2Rev to show the 
 *	   revision codes of the two FPGAs.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetFPGAIds(N1231A_HANDLE	N1231AHandle,
				 unsigned long	*pFPGA1Id,
				 unsigned long	*pFPGA2Id);
/*
 *	For the card specified by N1231AHandle:
 * 
 *	a) Sets *pFPGA1Id and *pFPGA2Id to show the 
 *	   ID codes of the two FPGAs.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetHdwRev(N1231A_HANDLE	N1231AHandle,
				unsigned short	*pHdwRev);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Reads the Hardware Revision code.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT unsigned int
N1231AGetLibraryRev(void);
/*
 *  Returns the library revision number.
 */

/************************************************************************/
EXPORT char* 
N1231AGetErrStr(N1231A_RETURN	err);
/*
 *	Returns a string describing the error conditions 
 *	related to error parameter "err".
 */

/************************************************************************/
/*
 * ==================================
 * Interrupt Related Functions
 * ==================================
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231APciInterruptEnable(N1231A_HANDLE	N1231AHandle,
						 int			enable);
/*
 *	For the card specified by N1231AHandle:
 *	
 *	Enables interrupts if parameter enable is non-zero.
 *	Disables interrupts if parameter enable is zero.
 *
 *	NOTE: See also the IRQ_ENB bit in the configuration
 *		  register and the Interrupt Mask bits.
 *
 *		  The Interrupt Mask bits determine whether 
 *		  individual status bits can generate interrupts.
 *
 *		  The IRQ_ENB bit in the configuration register 
 *		  controls whether interrupt conditions can be 
 *		  passed to the PCI interface.  It can be used to 
 *        temporarily inhibit generation of interrupts.
 *
 *		  This function (N1231APciInterruptEnable) is used 
 *		  to turn on (or off) PCI interrupt handling   
 *		  so that it can respond to the card conditions.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231APciInterruptAttach(N1231A_HANDLE		N1231AHandle,
						 N1231A_EVT_HANDLE	*eventHandle);
/*
 *	For the card specified by N1231AHandle:
 *
 *	Returns an *eventHandle to a Windows Event Object that can be used by
 *	a Windows Wait Function (for example, WaitForSingleObject()).  The 
 *	Event Object will be set to a signalled state when the N1231A card 
 *	generates an interrupt.  
 */

/************************************************************************/
/*
 * ==================================
 * Lower Level Functions
 * ==================================
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231ASamplePosVel(N1231A_HANDLE	N1231AHandle,
				   N1231A_AXIS		axis);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis:
 *
 *	a) Samples position and velocity.
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AReadRawPos(N1231A_HANDLE	N1231AHandle,
				 N1231A_AXIS	axis,
			   	 long			*pPosition);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis:
 *
 *	a) Reads position value.
 *
 *  IMPORTANT NOTE: This function reads the sample that was taken 
 *  on the last "sample" command.  It does not cause a new sample 
 *  to be latched.  Therefore, repeated calls to this function will
 *  return the same value unless a new sample command is issued.
 *  See also: N1231ASamplePosVel() and N1231AGetRawPosVel().
 *
 *	If an error has occurred on the axis, *pPosition 
 *	will be left unchanged.
 *
 *	The position value is in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS		Error on the axis or reference.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AReadRawVel(N1231A_HANDLE	N1231AHandle,
				 N1231A_AXIS	axis,
			   	 long			*pVelocity);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis:
 *
 *	a) Reads velocity value.
 *
 *  IMPORTANT NOTE: This function reads the sample that was taken 
 *  on the last "sample" command.  It does not cause a new sample 
 *  to be latched.  Therefore, repeated calls to this function will
 *  return the same value unless a new sample command is issued.
 *  See also: N1231ASamplePosVel() and N1231AGetRawPosVel().
 *
 *	If an error has occurred on the axis, *pVelocity 
 *	will be left unchanged.
 *
 *	The velocity value is in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_AXIS:	Error on axis or reference.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AWriteRawPresetRegister(N1231A_HANDLE	N1231AHandle,
						     N1231A_AXIS	axis,
						     long			presetValue);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis:
 *
 *	a) Writes presetValue to the preset position register for the axis.
 *     
 *  IMPORTANT NOTE: 
 *    Does not transfer this value into the position register.
 *    See also: N1231APresetRaw().
 *
 *	The preset value is in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AGetRawPresetRegister(N1231A_HANDLE	N1231AHandle,
					       N1231A_AXIS		axis,
					       long				*pPresetValue);
/*
 *	For the card specified by N1231AHandle and the axis specified
 *  by axis:
 *
 *	a) Sets *pPresetValue to the contents of the 
 *	   preset position register for the axis.
 *
 *	The preset value is in raw interferometer 
 *	units (see Definitions of Terms near top of this file).
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 */


/************************************************************************/
/*
 * ==================================
 * Register Functions (for test use only)
 * ==================================
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AWriteRegisterLong(N1231A_HANDLE	N1231AHandle,
						unsigned int	N1231ARegister,
						long			value);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Writes 32-bit value to the specified register.
 *
 *	The following constants can be used 
 *	for the parameter N1231ARegister:
 *
 *		N1231A_OFST_PRESET_1
 *		N1231A_OFST_PRESET_2
 *		N1231A_OFST_PRESET_3	
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_REG:		Invalid register for long write.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AReadRegisterLong(N1231A_HANDLE	N1231AHandle,
					   unsigned int		N1231ARegister,
					   long				*value);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Reads 32-bit value from the specified register.
 *
 *	The following constants can be used 
 *	for the parameter N1231ARegister:
 *
 *		N1231A_OFST_POS_1
 *		N1231A_OFST_POS_2
 *		N1231A_OFST_POS_3
 *		N1231A_OFST_VEL_1
 *		N1231A_OFST_VEL_2
 *		N1231A_OFST_VEL_3
 *		N1231A_OFST_PRESET_1
 *		N1231A_OFST_PRESET_2
 *		N1231A_OFST_PRESET_3
 *		N1231A_OFST_FPGA1_REV
 *		N1231A_OFST_FPGA2_REV
 *		N1231A_OFST_FPGA1_ID
 *		N1231A_OFST_FPGA2_ID
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_REG:		Invalid register for long read.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AWriteRegisterWord(N1231A_HANDLE	N1231AHandle,
						unsigned int	N1231ARegister,
						short			value);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Writes 16-bit value to the specified register.
 *
 *	The following constants can be used 
 *	for the parameter N1231ARegister:
 *
 *		N1231A_OFST_CONFIG_12	
 *		N1231A_OFST_CONFIG_3I	
 *		N1231A_OFST_STATUS_12	
 *		N1231A_OFST_STATUS_3R	
 *		N1231A_OFST_INTR_MASK
 *		N1231A_OFST_FILTER
 *		N1231A_OFST_SAMPLE
 *		N1231A_OFST_PRESET
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_REG:		Invalid register for word write.
 */

/************************************************************************/
EXPORT N1231A_RETURN
N1231AReadRegisterWord(N1231A_HANDLE	N1231AHandle,
					   unsigned int		N1231ARegister,
					   short			*value);
/*
 *	For the card specified by N1231AHandle:
 *
 *	a) Reads 16-bit value from the specified register.
 *
 *	The following constants can be used 
 *	for the parameter N1231ARegister:
 *
 *		N1231A_OFST_CONFIG_12	
 *		N1231A_OFST_CONFIG_3I	
 *		N1231A_OFST_STATUS_12	
 *		N1231A_OFST_STATUS_3R	
 *		N1231A_OFST_INTR_MASK
 *		N1231A_OFST_FILTER
 *		N1231A_OFST_SAMPLE
 *		N1231A_OFST_PRESET
 *
 *	Return value:
 *		N1231A_SUCCESS:		Success.
 *		N1231A_ERR_PARAM:	A passed parameter is NULL.
 *		N1231A_ERR_HANDLE:	N1231AHandle invalid.
 *		N1231A_ERR_REG:		Invalid register for word read.
 */

#ifdef __cplusplus
}
#endif

#endif /* IFNDEF N1231A_H */
