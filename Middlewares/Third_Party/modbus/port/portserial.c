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
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "main.h"
 
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
 
/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR( void );
//static void prvvUARTRxISR( void );
 
/* -----------------------    variables     ---------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
 
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable1( BOOL xRxEnable, BOOL xTxEnable )
{
  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
  * transmitter empty interrupts.
  */
  
  if (xRxEnable) {        
		HAL_GPIO_WritePin(MODBUS1_DIR_GPIO_Port, MODBUS1_DIR_Pin, GPIO_PIN_RESET);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  } else {    
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
  }
  
  if (xTxEnable) {    
		HAL_GPIO_WritePin(MODBUS1_DIR_GPIO_Port, MODBUS1_DIR_Pin, GPIO_PIN_SET);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
  } else {
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
  }  
}
 
void
vMBPortSerialEnable2( BOOL xRxEnable, BOOL xTxEnable )
{
  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
  * transmitter empty interrupts.
  */
  
  if (xRxEnable) {        
		HAL_GPIO_WritePin(MODBUS2_DIR_GPIO_Port, MODBUS2_DIR_Pin, GPIO_PIN_RESET);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
  } else {    
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
  }
  
  if (xTxEnable) {    
		HAL_GPIO_WritePin(MODBUS2_DIR_GPIO_Port, MODBUS2_DIR_Pin, GPIO_PIN_SET);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_TXE);
  } else {
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_TXE);
  }  
}

BOOL
xMBPortSerialPutByte1( CHAR ucByte )
{
  /* Put a byte in the UARTs transmit buffer. This function is called
  * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
  * called. */
  return (HAL_OK == HAL_UART_Transmit(&huart1, (uint8_t*)&ucByte, 1, 10));
}
 
BOOL
xMBPortSerialPutByte2( CHAR ucByte )
{
  /* Put a byte in the UARTs transmit buffer. This function is called
  * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
  * called. */
  return (HAL_OK == HAL_UART_Transmit(&huart3, (uint8_t*)&ucByte, 1, 10));
}

BOOL
xMBPortSerialGetByte1( CHAR * pucByte )
{
  /* Return the byte in the UARTs receive buffer. This function is called
  * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
  */
  *pucByte = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF);  
  return TRUE;
}
 
BOOL
xMBPortSerialGetByte2( CHAR * pucByte )
{
  /* Return the byte in the UARTs receive buffer. This function is called
  * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
  */
  *pucByte = (uint8_t)(huart3.Instance->DR & (uint8_t)0x00FF);  
  return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
* (or an equivalent) for your target processor. This function should then
* call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
* a new character can be sent. The protocol stack will then call 
* xMBPortSerialPutByte( ) to send the character.
 
static void prvvUARTTxReadyISR( void )
{
pxMBFrameCBTransmitterEmpty(  );
}
*/
 
/* Create an interrupt handler for the receive interrupt for your target
* processor. This function should then call pxMBFrameCBByteReceived( ). The
* protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
* character.
 
static void prvvUARTRxISR( void )
{
pxMBFrameCBByteReceived(  );
}
*/
