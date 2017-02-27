/*
 * Copyright 2002 by Agilent Technologies
 *
 * File Name: N1231A_reg.h
 *
 * Header file providing register map for the 
 * Agilent Technologies N1231A PCI Three-Axis Board 
 * designed for use with an Agilent Technologies
 * laser interferometry position measurement system.
 */ 
#ifndef N1231A_REG_H
#define N1231A_REG_H

/*
 *******************************************************************************
 *******************************************************************************
 *
 * REGISTER MAP OF N1231A
 *
 *******************************************************************************
 *	Used for direct register access. 
 *	These constants represent the offset from PCI BAR 2.
 *
 *	If using the N1231A API library, these constants are used with functions:
 *		N1231AReadRegisterLong()
 *		N1231AWriteRegisterLong()
 *		N1231AReadRegisterWord()
 *		N1231AWriteRegisterWord()
 */
#define N1231A_OFST_POS_1			0x0120	/* long read */
#define N1231A_OFST_POS_2			0x0100	/* long read */
#define N1231A_OFST_POS_3			0x0000	/* long read */
#define N1231A_OFST_VEL_1			0x0128	/* long read */
#define N1231A_OFST_VEL_2			0x0108	/* long read */
#define N1231A_OFST_VEL_3			0x0008	/* long read */
#define N1231A_OFST_PRESET_1		0x011c	/* long read/write */
#define N1231A_OFST_PRESET_2		0x0118	/* long read/write */
#define N1231A_OFST_PRESET_3		0x0018	/* long read/write */
#define N1231A_OFST_CONFIG_12		0x0110	/* word read/write */
#define N1231A_OFST_CONFIG_3I		0x0010	/* word read/write */
#define N1231A_OFST_STATUS_12		0x0112	/* word read/write */
#define N1231A_OFST_STATUS_3R		0x0012	/* word read/write */
#define N1231A_OFST_INTR_MASK		0x0016	/* word read/write */
#define N1231A_OFST_FILTER			0x001c	/* word read/write */
#define N1231A_OFST_SAMPLE			0x0020	/* word read/write */
#define N1231A_OFST_PRESET			0x0020	/* word read/write */
#define N1231A_OFST_FPGA1_REV		0x002c	/* long read */
#define N1231A_OFST_FPGA2_REV		0x012c	/* long read */
#define N1231A_OFST_FPGA1_ID		0x0030	/* long read */
#define N1231A_OFST_FPGA2_ID		0x0130	/* long read */

/*
 *******************************************************************************
 *******************************************************************************
 *
 * BIT MAPS
 *
 *******************************************************************************
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Used for status
 *
 * IMPORTANT NOTE: THESE CONSTANTS SHOULD ***ONLY*** BE USED IN CONJUNCTION WITH 
 * THE FOLLOWING API LIBRARY FUNCTIONS:
 *
 *		N1231AClearStatusBits()
 *		N1231AClearStatusAll()
 *		N1231AGetStatus()
 *		N1231APresetRawAll() 
 *		N1231APresetRaw() 
 *
 *	The above functions pack the axis 1, axis 2, axis 3 and Ref status bits into 
 *	one 16-bit word. These constants can be used for interpreting and clearing 
 *  these packed bits.
 */
#define N1231A_NO_SIG_1		0x0004
#define N1231A_GLITCH_1		0x0008
#define N1231A_NO_SIG_2		0x0001
#define N1231A_GLITCH_2		0x0002
#define N1231A_NO_SIG_3		0x0100
#define N1231A_GLITCH_3		0x0200
#define N1231A_NO_SIG_REF	0x1000
#define N1231A_GLITCH_REF	0x2000	

/*
 *******************************************************************************
 * Used for status ONLY WITH DIRECT REGISTER ACCESS
 *
 * IMPORTANT NOTE:
 *	The following constants are provided for use ***ONLY*** in 
 *	conjunction with direct access to the hardware registers.
 *	Direct access is provided by the library functions:  
 *		ReadRegisterWord()
 *		WriteRegisterWord()
 * They SHOULD NOT be used with any other library functions, as 
 * these other functions expect a packed status word. 
 */
#define N1231A_REG_NO_SIG_1		0x0004	// At offset N1231A_STATUS_12 (see note above)
#define N1231A_REG_GLITCH_1		0x0008	// At offset N1231A_STATUS_12 (see note above)
#define N1231A_REG_NO_SIG_2		0x0001	// At offset N1231A_STATUS_12 (see note above)
#define N1231A_REG_GLITCH_2		0x0002	// At offset N1231A_STATUS_12 (see note above)

#define N1231A_REG_NO_SIG_3		0x0001	// At offset N1231A_STATUS_3R (see note above)
#define N1231A_REG_GLITCH_3		0x0002	// At offset N1231A_STATUS_3R (see note above)
#define N1231A_REG_NO_SIG_REF	0x0010	// At offset N1231A_STATUS_3R (see note above)
#define N1231A_REG_GLITCH_REF	0x0020	// At offset N1231A_STATUS_3R (see note above)

/*
 *******************************************************************************
 * Used for configuration
 *
 * IMPORTANT NOTE: THESE CONSTANTS SHOULD ***ONLY*** BE USED IN CONJUNCTION WITH 
 * THE FOLLOWING API LIBRARY FUNCTIONS:
 *
 *		N1231ASetConfig()
 *		N1231AGetConfig()
 *  
 *	The above functions pack the axis 1, axis 2, axis 3, and IRQ control bits into
 *	one 16-bit word. These  constants can be used for setting and interpreting 
 *  these packed bits.
 */
#define N1231A_DIR_MINUS_1	0x0004
#define N1231A_DIR_MINUS_2	0x0002
#define N1231A_DIR_MINUS_3	0x0200
#define N1231A_IRQ_ENB		0x1000

/*
 *******************************************************************************
 * Used for configuration ONLY WITH DIRECT REGISTER ACCESS
 *
 * IMPORTANT NOTE:
 *	The following constants are provided for use ***ONLY*** in 
 *	conjunction with direct access to the hardware registers,
 *	Direct access is provided by the library functions:  
 *		ReadRegisterWord()
 *		WriteRegisterWord()
 * They SHOULD NOT be used with any other library functions, as 
 * these other functions expect a packed status word. 
 */
#define N1231A_REG_DIR_MINUS_1	0x0004	// At offset N1231A_CONFIG_12 (see note above)
#define N1231A_REG_DIR_MINUS_2	0x0002	// At offset N1231A_CONFIG_12 (see note above)

#define N1231A_REG_DIR_MINUS_3	0x0002	// At offset N1231A_CONFIG_3I (see note above)
#define N1231A_REG_IRQ_ENB		0x1000	// At offset N1231A_CONFIG_3I (see note above)

/*
 *******************************************************************************
 * Used for Interrupt Mask
 *
 *	These constants can be used in conjunction with the following 
 *	API Library Functions:
 * 
 *		N1231ASetInterruptMask()
 *		N1231AGetInterruptMask()
 *
 *	These can also be used in conjunction with direct register access
 *	at address offset N1231A_INTR_MASK
 */
#define N1231A_INTR_NO_SIG_1		0x0100
#define N1231A_INTR_GLITCH_1		0x0200
#define N1231A_INTR_NO_SIG_2		0x0001
#define N1231A_INTR_GLITCH_2		0x0002
#define N1231A_INTR_NO_SIG_3		0x0010
#define N1231A_INTR_GLITCH_3		0x0020
#define N1231A_INTR_NO_SIG_REF		0x1000
#define N1231A_INTR_GLITCH_REF		0x2000

/*
 *******************************************************************************
 * Used for Filter
 *
 *	These constants can be used in conjunction with the following 
 *	API Library Functions:
 * 
 *		N1231ASetFilter()
 *		N1231AGetFilter()
 *
 *	These can also be used in conjunction with direct register access
 *	at address offset N1231A_FILTER
 */
#define N1231A_FILTER_ENB	0X0010

#define N1231A_KP0			0x0000
#define N1231A_KP1			0x0001
#define	N1231A_KP2			0x0002
#define N1231A_KP3			0x0003

#define N1231A_KV0			0x0000
#define N1231A_KV1			0x0004
#define N1231A_KV2			0x0008
#define N1231A_KV3			0x000C

/*
 *******************************************************************************
 * Used for reading Sample Register
 *
 *	These constants can be used in conjunction with direct register access
 *	reads at address offset N1231A_SAMPLE
 */
#define N1231A_ERR_1		0x0100
#define N1231A_VALID_1		0x0200
#define N1231A_ERR_2		0x0001
#define N1231A_VALID_2		0x0002
#define N1231A_ERR_3		0x0010
#define N1231A_VALID_3		0x0020

/*
 *******************************************************************************
 * Used for writing Sample Register
 *
 *	These constants can be used in conjunction with direct register access
 *	writes at address offset N1231A_SAMPLE
 */
#define N1231A_PRESET_1		0x0100
#define N1231A_SAMPLE_1		0x0200
#define N1231A_PRESET_2		0x0001
#define N1231A_SAMPLE_2		0x0002
#define N1231A_PRESET_3		0x0010
#define N1231A_SAMPLE_3		0x0020


#endif /* ifndef N1231A_REG_H */
