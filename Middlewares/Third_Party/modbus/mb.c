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
 * File: $Id: mb.c,v 1.28 2010/06/06 13:54:40 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mbport.h"
#include "mbrtu.h"
#include "main.h"
#include "Settings.h"

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBAddress1;
static UCHAR    ucMBAddress2;

enum eMBState
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
};
static enum eMBState eMBState1 = STATE_NOT_INITIALIZED;
static enum eMBState eMBState2 = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend peMBFrameSendCur1;
static pvMBFrameStart pvMBFrameStartCur1;
static pvMBFrameStop pvMBFrameStopCur1;
static peMBFrameReceive peMBFrameReceiveCur1;
static pvMBFrameClose pvMBFrameCloseCur1;

static peMBFrameSend peMBFrameSendCur2;
static pvMBFrameStart pvMBFrameStartCur2;
static pvMBFrameStop pvMBFrameStopCur2;
static peMBFrameReceive peMBFrameReceiveCur2;
static pvMBFrameClose pvMBFrameCloseCur2;
/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived1 ) ( void );
BOOL( *pxMBFrameCBByteReceived2 ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty1 ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty2 ) ( void );
BOOL( *pxMBPortCBTimerExpired1 ) ( void );
BOOL( *pxMBPortCBTimerExpired2 ) ( void );

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );

/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBInit1( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress1 = ucSlaveAddress;

		pvMBFrameStartCur1 = eMBRTUStart1;
		pvMBFrameStopCur1 = eMBRTUStop1;
		peMBFrameSendCur1 = eMBRTUSend1;
		peMBFrameReceiveCur1 = eMBRTUReceive1;
		pvMBFrameCloseCur1 = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
		pxMBFrameCBByteReceived1 = xMBRTUReceiveFSM1;
		pxMBFrameCBTransmitterEmpty1 = xMBRTUTransmitFSM1;
		pxMBPortCBTimerExpired1 = xMBRTUTimerT35Expired1;

		eStatus = eMBRTUInit1( ucMBAddress1, ucPort, ulBaudRate, eParity );

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit1(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBState1 = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}

eMBErrorCode
eMBInit2( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        ucMBAddress2 = ucSlaveAddress;

		pvMBFrameStartCur2 = eMBRTUStart2;
		pvMBFrameStopCur2 = eMBRTUStop2;
		peMBFrameSendCur2 = eMBRTUSend2;
		peMBFrameReceiveCur2 = eMBRTUReceive2;
		pvMBFrameCloseCur2 = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
		pxMBFrameCBByteReceived2 = xMBRTUReceiveFSM2;
		pxMBFrameCBTransmitterEmpty2 = xMBRTUTransmitFSM2;
		pxMBPortCBTimerExpired2 = xMBRTUTimerT35Expired2;

		eStatus = eMBRTUInit2( ucMBAddress2, ucPort, ulBaudRate, eParity );

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit2(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBState2 = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}

eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
        ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
        EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}


eMBErrorCode
eMBClose1( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState1 == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur1 != NULL )
        {
            pvMBFrameCloseCur1(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBClose2( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState2 == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur2 != NULL )
        {
            pvMBFrameCloseCur2(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBEnable1( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState1 == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur1(  );
        eMBState1 = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBEnable2( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState2 == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur2(  );
        eMBState2 = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBDisable1( void )
{
    eMBErrorCode    eStatus;

    if( eMBState1 == STATE_ENABLED )
    {
        pvMBFrameStopCur1(  );
        eMBState1 = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState1 == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBDisable2( void )
{
    eMBErrorCode    eStatus;

    if( eMBState2 == STATE_ENABLED )
    {
        pvMBFrameStopCur2(  );
        eMBState2 = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState2 == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBPoll1( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;

    /* Check if the protocol stack is ready. */
    if( eMBState1 != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet1( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
            eStatus = peMBFrameReceiveCur1( &ucRcvAddress, &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == ucMBAddress1 ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost1( EV_EXECUTE );
                }
            }
            break;

        case EV_EXECUTE:
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );
										ModBus1_cnt++;
										Save_Counts_Del_f = true;
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
                }
                eStatus = peMBFrameSendCur1( ucMBAddress1, ucMBFrame, usLength );
            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}

eMBErrorCode
eMBPoll2( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;

    /* Check if the protocol stack is ready. */
    if( eMBState2 != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet2( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
            eStatus = peMBFrameReceiveCur2( &ucRcvAddress, &ucMBFrame, &usLength );
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( ( ucRcvAddress == ucMBAddress2 ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )
                {
                    ( void )xMBPortEventPost2( EV_EXECUTE );
                }
            }
            break;

        case EV_EXECUTE:
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );
										ModBus2_cnt++;
										Save_Counts_Del_f = true;
                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
                }
                eStatus = peMBFrameSendCur2( ucMBAddress2, ucMBFrame, usLength );
            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}
