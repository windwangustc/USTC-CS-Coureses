/*!
 * \file      pinName-board.h
 *
 * \brief     Target board GPIO pins definitions
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __PIN_NAME_BOARD_H__
#define __PIN_NAME_BOARD_H__

#if defined ( FREEDOM )
/*!
 * Freescale MCU Pin Names
 */
#define MCU_PINS \
    PA_0 = 0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7, PA_8, PA_9, PA_10, PA_11, PA_12, PA_13, PA_14, PA_15, \
	PA_16, PA_17, PA_18, PA_19, PA_20, PA_21, PA_22, PA_23, PA_24, PA_25, PA_26, PA_27, PA_28, PA_29, PA_30, PA_31, \
    PB_0,  PB_1,  PB_2,  PB_3,  PB_4,  PB_5,  PB_6,  PB_7,  PB_8,  PB_9,  PB_10, PB_11, PB_12, PB_13, PB_14, PB_15, \
	PB_16, PB_17, PB_18, PB_19, PB_20, PB_21, PB_22, PB_23, PB_24, PB_25, PB_26, PB_27, PB_28, PB_29, PB_30, PB_31, \
    PC_0,  PC_1,  PC_2,  PC_3,  PC_4,  PC_5,  PC_6,  PC_7,  PC_8,  PC_9,  PC_10, PC_11, PC_12, PC_13, PC_14, PC_15, \
	PC_16, PC_17, PC_18, PC_19, PC_20, PC_21, PC_22, PC_23, PC_24, PC_25, PC_26, PC_27, PC_28, PC_29, PC_30, PC_31, \
    PD_0,  PD_1,  PD_2,  PD_3,  PD_4,  PD_5,  PD_6,  PD_7,  PD_8,  PD_9,  PD_10, PD_11, PD_12, PD_13, PD_14, PD_15, \
	PD_16, PD_17, PD_18, PD_19, PD_20, PD_21, PD_22, PD_23, PD_24, PD_25, PD_26, PD_27, PD_28, PD_29, PD_30, PD_31, \
    PE_0,  PE_1,  PE_2,  PE_3,  PE_4,  PE_5,  PE_6,  PE_7,  PE_8,  PE_9,  PE_10, PE_11, PE_12, PE_13, PE_14, PE_15, \
	PE_16, PE_17, PE_18, PE_19, PE_20, PE_21, PE_22, PE_23, PE_24, PE_25, PE_26, PE_27, PE_28, PE_29, PE_30, PE_31
#else
/*!
 * STM32 Pin Names
 */
#define MCU_PINS \
    PA_0 = 0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7, PA_8, PA_9, PA_10, PA_11, PA_12, PA_13, PA_14, PA_15, \
    PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7, PB_8, PB_9, PB_10, PB_11, PB_12, PB_13, PB_14, PB_15,     \
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9, PC_10, PC_11, PC_12, PC_13, PC_14, PC_15,     \
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, PD_8, PD_9, PD_10, PD_11, PD_12, PD_13, PD_14, PD_15,     \
    PE_0, PE_1, PE_2, PE_3, PE_4, PE_5, PE_6, PE_7, PE_8, PE_9, PE_10, PE_11, PE_12, PE_13, PE_14, PE_15,     \
    PF_0, PF_1, PF_2, PF_3, PF_4, PF_5, PF_6, PF_7, PF_8, PF_9, PF_10, PF_11, PF_12, PF_13, PF_14, PF_15,     \
    PH_0, PH_1, PH_2, PH_3, PH_4, PH_5, PH_6, PH_7, PH_8, PH_9, PH_10, PH_11, PH_12, PH_13, PH_14, PH_15
#endif // FREEDOM
#endif // __PIN_NAME_BOARD_H__
