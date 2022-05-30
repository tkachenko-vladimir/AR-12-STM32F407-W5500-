/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu.c,v 1.18 2007/09/12 10:15:56 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbrtu.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_ERROR              /*!< If the frame is invalid. */
} eMBRcvState;

typedef enum
{
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT               /*!< Transmitter is in transfer state. */
} eMBSndState;

/* ----------------------- Static variables ---------------------------------*/
static volatile eMBSndState eSndState1;
static volatile eMBSndState eSndState2;
static volatile eMBRcvState eRcvState1;
static volatile eMBRcvState eRcvState2;

volatile UCHAR  ucRTUBuf1[MB_SER_PDU_SIZE_MAX];
volatile UCHAR  ucRTUBuf2[MB_SER_PDU_SIZE_MAX];

static volatile UCHAR *pucSndBufferCur1;
static volatile UCHAR *pucSndBufferCur2;
static volatile USHORT usSndBufferCount1;
static volatile USHORT usSndBufferCount2;

static volatile USHORT usRcvBufferPos1;
static volatile USHORT usRcvBufferPos2;

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBRTUInit1( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;

    ( void )ucSlaveAddress;
    ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
	/* If baudrate > 19200 then we should use the fixed timer values
	 * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
	 */
	if( ulBaudRate > 19200 )
	{
		usTimerT35_50us = 35;       /* 1800us. */
	}
	else
	{
		/* The timer reload value for a character is given by:
		 *
		 * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
		 *             = 11 * Ticks_per_1s / Baudrate
		 *             = 220000 / Baudrate
		 * The reload for t3.5 is 1.5 times this value and similary
		 * for t3.5.
		 */
		usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
	}
	if( xMBPortTimersInit1( ( USHORT ) usTimerT35_50us ) != TRUE )
	{
		eStatus = MB_EPORTERR;
	}
    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

eMBErrorCode
eMBRTUInit2( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;

    ( void )ucSlaveAddress;
    ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
	/* If baudrate > 19200 then we should use the fixed timer values
	 * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
	 */
	if( ulBaudRate > 19200 )
	{
		usTimerT35_50us = 35;       /* 1800us. */
	}
	else
	{
		/* The timer reload value for a character is given by:
		 *
		 * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
		 *             = 11 * Ticks_per_1s / Baudrate
		 *             = 220000 / Baudrate
		 * The reload for t3.5 is 1.5 times this value and similary
		 * for t3.5.
		 */
		usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
	}
	if( xMBPortTimersInit2( ( USHORT ) usTimerT35_50us ) != TRUE )
	{
		eStatus = MB_EPORTERR;
	}
    EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBRTUStart1( void )
{
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState1 = STATE_RX_INIT;
    vMBPortSerialEnable1( TRUE, FALSE );
    vMBPortTimersEnable1(  );

    EXIT_CRITICAL_SECTION(  );
}

void
eMBRTUStart2( void )
{
    ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState2 = STATE_RX_INIT;
    vMBPortSerialEnable2( TRUE, FALSE );
    vMBPortTimersEnable2(  );

    EXIT_CRITICAL_SECTION(  );
}

void
eMBRTUStop1( void )
{
    ENTER_CRITICAL_SECTION(  );
    vMBPortSerialEnable1( FALSE, FALSE );
    vMBPortTimersDisable1(  );
    EXIT_CRITICAL_SECTION(  );
}

void
eMBRTUStop2( void )
{
    ENTER_CRITICAL_SECTION(  );
    vMBPortSerialEnable2( FALSE, FALSE );
    vMBPortTimersDisable2(  );
    EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBRTUReceive1( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    BOOL            xFrameReceived = FALSE;
    eMBErrorCode    eStatus = MB_ENOERR;
	
	
	 (void)(xFrameReceived);
	

    ENTER_CRITICAL_SECTION(  );
    assert( usRcvBufferPos1 < MB_SER_PDU_SIZE_MAX );

    /* Length and CRC check */
    if( ( usRcvBufferPos1 >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( ( UCHAR * ) ucRTUBuf1, usRcvBufferPos1 ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf1[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( usRcvBufferPos1 - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & ucRTUBuf1[MB_SER_PDU_PDU_OFF];
        xFrameReceived = TRUE;
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBRTUReceive2( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    BOOL            xFrameReceived = FALSE;
    eMBErrorCode    eStatus = MB_ENOERR;
	
	
	 (void)(xFrameReceived);
	

    ENTER_CRITICAL_SECTION(  );
    assert( usRcvBufferPos2 < MB_SER_PDU_SIZE_MAX );

    /* Length and CRC check */
    if( ( usRcvBufferPos2 >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( ( UCHAR * ) ucRTUBuf2, usRcvBufferPos2 ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf2[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( usRcvBufferPos2 - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & ucRTUBuf2[MB_SER_PDU_PDU_OFF];
        xFrameReceived = TRUE;
    }
    else
    {
        eStatus = MB_EIO;
    }

    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBRTUSend1( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState1 == STATE_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur1 = ( UCHAR * ) pucFrame - 1;
        usSndBufferCount1 = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur1[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount1 += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucSndBufferCur1, usSndBufferCount1 );
        ucRTUBuf1[usSndBufferCount1++] = ( UCHAR )( usCRC16 & 0xFF );
        ucRTUBuf1[usSndBufferCount1++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState1 = STATE_TX_XMIT;
        vMBPortSerialEnable1( FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBRTUSend2( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState2 == STATE_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur2 = ( UCHAR * ) pucFrame - 1;
        usSndBufferCount2 = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur2[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount2 += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucSndBufferCur2, usSndBufferCount2 );
        ucRTUBuf2[usSndBufferCount2++] = ( UCHAR )( usCRC16 & 0xFF );
        ucRTUBuf2[usSndBufferCount2++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState2 = STATE_TX_XMIT;
        vMBPortSerialEnable2( FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

BOOL
xMBRTUReceiveFSM1( void )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;

    assert( eSndState1 == STATE_TX_IDLE );

    /* Always read the character. */
    ( void )xMBPortSerialGetByte1( ( CHAR * ) & ucByte );

    switch ( eRcvState1 )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        vMBPortTimersEnable1(  );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        vMBPortTimersEnable1(  );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos1 = 0;
        ucRTUBuf1[usRcvBufferPos1++] = ucByte;
        eRcvState1 = STATE_RX_RCV;

        /* Enable t3.5 timers. */
        vMBPortTimersEnable1(  );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos1 < MB_SER_PDU_SIZE_MAX )
        {
            ucRTUBuf1[usRcvBufferPos1++] = ucByte;
        }
        else
        {
            eRcvState1 = STATE_RX_ERROR;
        }
        vMBPortTimersEnable1(  );
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBRTUReceiveFSM2( void )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;

    assert( eSndState2 == STATE_TX_IDLE );

    /* Always read the character. */
    ( void )xMBPortSerialGetByte2( ( CHAR * ) & ucByte );

    switch ( eRcvState2 )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:
        vMBPortTimersEnable2(  );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        vMBPortTimersEnable2(  );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos2 = 0;
        ucRTUBuf2[usRcvBufferPos2++] = ucByte;
        eRcvState2 = STATE_RX_RCV;

        /* Enable t3.5 timers. */
        vMBPortTimersEnable2(  );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos2 < MB_SER_PDU_SIZE_MAX )
        {
            ucRTUBuf2[usRcvBufferPos2++] = ucByte;
        }
        else
        {
            eRcvState2 = STATE_RX_ERROR;
        }
        vMBPortTimersEnable2(  );
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBRTUTransmitFSM1( void )
{
    BOOL            xNeedPoll = FALSE;

    assert( eRcvState1 == STATE_RX_IDLE );

    switch ( eSndState1 )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        /* enable receiver/disable transmitter. */
        vMBPortSerialEnable1( TRUE, FALSE );
        break;

    case STATE_TX_XMIT:
        /* check if we are finished. */
        if( usSndBufferCount1 != 0 )
        {
            xMBPortSerialPutByte1( ( CHAR )*pucSndBufferCur1 );
            pucSndBufferCur1++;  /* next byte in sendbuffer. */
            usSndBufferCount1--;
        }
        else
        {
            xNeedPoll = xMBPortEventPost1( EV_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBPortSerialEnable1( TRUE, FALSE );
            eSndState1 = STATE_TX_IDLE;
        }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBRTUTransmitFSM2( void )
{
    BOOL            xNeedPoll = FALSE;

    assert( eRcvState2 == STATE_RX_IDLE );

    switch ( eSndState2 )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        /* enable receiver/disable transmitter. */
        vMBPortSerialEnable2( TRUE, FALSE );
        break;

    case STATE_TX_XMIT:
        /* check if we are finished. */
        if( usSndBufferCount2 != 0 )
        {
            xMBPortSerialPutByte2( ( CHAR )*pucSndBufferCur2 );
            pucSndBufferCur2++;  /* next byte in sendbuffer. */
            usSndBufferCount2--;
        }
        else
        {
            xNeedPoll = xMBPortEventPost2( EV_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBPortSerialEnable2( TRUE, FALSE );
            eSndState2 = STATE_TX_IDLE;
        }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBRTUTimerT35Expired1( void )
{
    BOOL            xNeedPoll = FALSE;

    switch ( eRcvState1 )
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost1( EV_READY );
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost1( EV_FRAME_RECEIVED );
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:
        assert( ( eRcvState1 == STATE_RX_INIT ) ||
                ( eRcvState1 == STATE_RX_RCV ) || ( eRcvState1 == STATE_RX_ERROR ) );
    }

    vMBPortTimersDisable1(  );
    eRcvState1 = STATE_RX_IDLE;

    return xNeedPoll;
}

BOOL
xMBRTUTimerT35Expired2( void )
{
    BOOL            xNeedPoll = FALSE;

    switch ( eRcvState2 )
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost2( EV_READY );
        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost2( EV_FRAME_RECEIVED );
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:
        assert( ( eRcvState2 == STATE_RX_INIT ) ||
                ( eRcvState2 == STATE_RX_RCV ) || ( eRcvState2 == STATE_RX_ERROR ) );
    }

    vMBPortTimersDisable2(  );
    eRcvState2 = STATE_RX_IDLE;

    return xNeedPoll;
}
