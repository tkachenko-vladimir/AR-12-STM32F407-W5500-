/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static eMBEventType eQueuedEvent1;
static eMBEventType eQueuedEvent2;
static BOOL     xEventInQueue1;
static BOOL     xEventInQueue2;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit1( void )
{
    xEventInQueue1 = FALSE;
    return TRUE;
}

BOOL
xMBPortEventInit2( void )
{
    xEventInQueue2 = FALSE;
    return TRUE;
}

BOOL
xMBPortEventPost1( eMBEventType eEvent )
{
    xEventInQueue1 = TRUE;
    eQueuedEvent1 = eEvent;
    return TRUE;
}

BOOL
xMBPortEventPost2( eMBEventType eEvent )
{
    xEventInQueue2 = TRUE;
    eQueuedEvent2 = eEvent;
    return TRUE;
}

BOOL
xMBPortEventGet1( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue1 )
    {
        *eEvent = eQueuedEvent1;
        xEventInQueue1 = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

BOOL
xMBPortEventGet2( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue2 )
    {
        *eEvent = eQueuedEvent2;
        xEventInQueue2 = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
