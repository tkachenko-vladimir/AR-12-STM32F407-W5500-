/* USER CODE BEGIN Header */
#define _PO_VERSION 38
#define _SETTINGS_VER 0xA7
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "socket.h"
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define  _IEC_Connect_timer 60
#define Imsz1_MIN 5
#define Imsz1_MAX 400
#define Tmsz1_MIN 0
#define Tmsz1_MAX 7200
#define Imsz2_MIN 50
#define Imsz2_MAX 800
#define Tmsz2_MIN 0
#define Tmsz2_MAX 300
#define Isv_MIN 50
#define Isv_MAX 1600
#define Tsv_MIN 0
#define Tsv_MAX 500
#define APV_Tgot_MIN 10
#define APV_Tgot_MAX 90
#define APV1_T_MIN 1
#define APV1_T_MAX 50
#define APV2_T_MIN 10
#define APV2_T_MAX 60
#define ZMN_level_MIN 20
#define ZMN_level_MAX 80
#define ZMN_Tvidkl_MIN 1
#define ZMN_Tvidkl_MAX 99
#define ZMN_Tvkl_MIN 10
#define ZMN_Tvkl_MAX 99

#define INPUT_POWER_FAULT 8
#define INPUT_RES_POWER 16
#define INPUT_DOOR 32
#define INPUT_COIL_FAULT 64
#define INPUT_DV8 128

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern osMessageQId EventsQueueHandle;
extern osMessageQId AD7606_QueueHandle;
extern osSemaphoreId ADC_Data_SemHandle;

extern bool Setup_mode, MAX7219_ChkMode;
extern bool MSZ1vv_setup, SVvv_setup, APV_T2vv_setup, ZMN_Ton_vv_setup;
extern uint16_t Imsz1_setup, Tmsz1_setup, Imsz2_setup, Tmsz2_setup, Isv_setup, Tsv_setup, APV_Tgot_setup, APV_T1_setup, APV_T2_setup, ZMN_Lvl_setup, ZMN_Toff_setup, ZMN_Ton_setup;
extern bool Imsz1_blnk, Tmsz1_blnk, Imsz2_blnk, Tmsz2_blnk, Isv_blnk, Tsv_blnk, APV_Tgot_blnk, APV_T1_blnk, APV_T2_blnk, ZMN_Lvl_blnk, ZMN_Toff_blnk, ZMN_Ton_blnk, Time_blnk;
extern uint8_t Internal_Faults, Setup_error_flg, MSZ_work_flg, Task_control;
extern bool INP_DoorOpen;
extern uint16_t Imsz1A, Imsz1B, Tmsz1A, Tmsz1B;
extern bool MAX7219_LED_Block, MAX7219_LED_Nespr, MAX7219_LED_ErrZv, MAX7219_LED_Reserve, MAX7219_LED_ExtPow;

extern uint8_t Year, Month, Date, Hours, Minutes, Seconds;

extern uint16_t VrmsA, IrmsA, IrmsC;
extern uint16_t WorkAPV_ZMN_timer;
////////////////////////////////////////////////////////////////
extern bool Summer_Time;
extern int8_t Temp;
extern unsigned short int Setup2_View_cnt, Save_Settings_Del_cnt, Sec_cnt, Min_cnt;
extern unsigned short int Setup2_En_cnt, DUvv_cnt, Setup1Error_cnt, LEDdigit_cnt;
extern unsigned char Setup2_View, MSZ_state, APV_cikl;
extern unsigned int Setup_En_cnt;

extern bool VV_Unblock_f, ADC_RST_f;
extern bool Save_Settings1_f, Save_Settings2_f, Save_Counts_f, Save_Counts_Del_f;
extern bool Setup2_En;
extern bool Save_Settings_Del_f;
extern bool USB_Read_f;

extern char USB_Send_buf[200], USB_Read_buf[200], USB_Cmd_buf[200];
extern unsigned short int ADC_Read_Buf[4];
extern unsigned char USB_Cmd_pos;
extern unsigned int USB_Menu;

extern unsigned int VrmsA_d, IrmsA_d, IrmsC_d;
extern unsigned int VrmsA_poshk, IrmsA_poshk, IrmsC_poshk;
extern bool RMS_process, Debug_mode;

extern bool VV_Block_f, Command_VV_On, Command_VV_Off;
extern unsigned int ChasGotovn_timer, VV_Ready_Timer, VV_ReadyFault_Timer;
extern unsigned int Timer_OZZ, MSZ_APV_timer;
extern unsigned int MSZ1A_timer, MSZ2A_timer, MSZsvA_timer, MSZ1B_timer, MSZ2B_timer, MSZsvB_timer;
extern unsigned char MSZ_APV_cikl, OZZ_state, MSZ_Worked_flags, VV_Faults, VV_Faults_old;

extern bool VV_Off_State, VV_On_State, VV_Ready_State;
extern unsigned char VV_On_Sourse, VV_Off_Sourse, VV_On_Sourse_tmp, VV_Off_Sourse_tmp, VV_State;

extern unsigned int Control_VV_Off_timer, Control_VV_On_timer, MB_CoilFault_timer;
extern unsigned short int ADC_BUSY_timer, Int_Flts_cnt;
extern bool LED_state, Save_State_f, Read_Temp_f;

extern bool MB_PuskMSZ1, MB_VozvratMSZ1, MB_WorkMSZ1;
extern bool MB_PuskMSZ2, MB_VozvratMSZ2, MB_WorkMSZ2;
extern bool MB_PuskSV, MB_WorkSV, MB_GotovAPV;
extern bool MB_PuskAPV1, MB_WorkAPV1, MB_PuskAPV2, MB_WorkAPV2;
extern bool MB_PuskZMN, MB_VozvratZMN, MB_WorkZMN, MB_PuskAPV_ZMN;
extern bool MB_VozvratAPV_ZMN, MB_WorkAPV_ZMN;

extern bool IEC_PuskMSZ1, IEC_VozvratMSZ1, IEC_WorkMSZ1;
extern bool IEC_PuskMSZ2, IEC_VozvratMSZ2, IEC_WorkMSZ2;
extern bool IEC_PuskSV, IEC_WorkSV, IEC_GotovAPV;
extern bool IEC_PuskAPV1, IEC_WorkAPV1, IEC_PuskAPV2, IEC_WorkAPV2;
extern bool IEC_PuskZMN, IEC_VozvratZMN, IEC_WorkZMN, IEC_PuskAPV_ZMN;
extern bool IEC_VozvratAPV_ZMN, IEC_WorkAPV_ZMN, IEC_VV_Block, IEC_VV_Fault, IEC_Spr_Flt;
extern bool IEC_Coil_Flt;
extern bool IEC_Connect;
extern unsigned int IEC_VrmsA, IEC_IrmsA, IEC_IrmsC;
extern unsigned char IEC_VrmsA_D, IEC_IrmsA_D, IEC_IrmsC_D;
extern uint16_t IEC_VV_Fault_timer, IEC_Connect_timer;
extern uint8_t DelayStartWD;

extern char Iec104_RecvBuf[512];
extern uint16_t Iec104_RecvLen;
extern uint16_t W5500rst_cnt, W5500rst_cntP;


void Setup1End(void);
void Setup2End(void);
////////////////////////////////////////////////////////////////
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OUT_RELE_3_Pin GPIO_PIN_2
#define OUT_RELE_3_GPIO_Port GPIOE
#define OUT_RELE_2_Pin GPIO_PIN_3
#define OUT_RELE_2_GPIO_Port GPIOE
#define OUT_RELE_1_Pin GPIO_PIN_4
#define OUT_RELE_1_GPIO_Port GPIOE
#define OUT_VV_OFF_RELE_Pin GPIO_PIN_5
#define OUT_VV_OFF_RELE_GPIO_Port GPIOE
#define OUT_VV_ON_RELE_Pin GPIO_PIN_6
#define OUT_VV_ON_RELE_GPIO_Port GPIOE
#define TEST_LED_Pin GPIO_PIN_13
#define TEST_LED_GPIO_Port GPIOC
#define OUT_VV_ON_MSFT_Pin GPIO_PIN_1
#define OUT_VV_ON_MSFT_GPIO_Port GPIOC
#define OUT_VV_OFF_MSFT_Pin GPIO_PIN_2
#define OUT_VV_OFF_MSFT_GPIO_Port GPIOC
#define RS232_TX_Pin GPIO_PIN_2
#define RS232_TX_GPIO_Port GPIOA
#define RS232_RX_Pin GPIO_PIN_3
#define RS232_RX_GPIO_Port GPIOA
#define AD7606_CS_Pin GPIO_PIN_4
#define AD7606_CS_GPIO_Port GPIOA
#define AD7606_SCK_Pin GPIO_PIN_5
#define AD7606_SCK_GPIO_Port GPIOA
#define AD7606_MISO_Pin GPIO_PIN_6
#define AD7606_MISO_GPIO_Port GPIOA
#define AD7606_MOSI_Pin GPIO_PIN_7
#define AD7606_MOSI_GPIO_Port GPIOA
#define AD7606_BUSY_Pin GPIO_PIN_4
#define AD7606_BUSY_GPIO_Port GPIOC
#define AD7606_BUSY_EXTI_IRQn EXTI4_IRQn
#define AD7606_RESET_Pin GPIO_PIN_5
#define AD7606_RESET_GPIO_Port GPIOC
#define KBRD_O1_Pin GPIO_PIN_0
#define KBRD_O1_GPIO_Port GPIOB
#define KBRD_O2_Pin GPIO_PIN_1
#define KBRD_O2_GPIO_Port GPIOB
#define KBRD_O3_Pin GPIO_PIN_2
#define KBRD_O3_GPIO_Port GPIOB
#define KEY_VV_ON_Pin GPIO_PIN_7
#define KEY_VV_ON_GPIO_Port GPIOE
#define KEY_VV_OFF_Pin GPIO_PIN_8
#define KEY_VV_OFF_GPIO_Port GPIOE
#define LED_VV_ON_Pin GPIO_PIN_9
#define LED_VV_ON_GPIO_Port GPIOE
#define LED_VV_OFF_Pin GPIO_PIN_10
#define LED_VV_OFF_GPIO_Port GPIOE
#define W5500_RESET_Pin GPIO_PIN_11
#define W5500_RESET_GPIO_Port GPIOE
#define W5500_CS_Pin GPIO_PIN_12
#define W5500_CS_GPIO_Port GPIOE
#define MODBUS2_TX_Pin GPIO_PIN_10
#define MODBUS2_TX_GPIO_Port GPIOB
#define MODBUS2_RX_Pin GPIO_PIN_11
#define MODBUS2_RX_GPIO_Port GPIOB
#define MAX7219_CS_Pin GPIO_PIN_12
#define MAX7219_CS_GPIO_Port GPIOB
#define MAX7219_CLK_Pin GPIO_PIN_13
#define MAX7219_CLK_GPIO_Port GPIOB
#define TEST_OUT_Pin GPIO_PIN_14
#define TEST_OUT_GPIO_Port GPIOB
#define MAX7219_DAT_Pin GPIO_PIN_15
#define MAX7219_DAT_GPIO_Port GPIOB
#define KBRD_I1_Pin GPIO_PIN_8
#define KBRD_I1_GPIO_Port GPIOD
#define KBRD_I2_Pin GPIO_PIN_9
#define KBRD_I2_GPIO_Port GPIOD
#define KBRD_I3_Pin GPIO_PIN_10
#define KBRD_I3_GPIO_Port GPIOD
#define KBRD_I4_Pin GPIO_PIN_11
#define KBRD_I4_GPIO_Port GPIOD
#define KBRD_I5_Pin GPIO_PIN_12
#define KBRD_I5_GPIO_Port GPIOD
#define KBRD_I6_Pin GPIO_PIN_13
#define KBRD_I6_GPIO_Port GPIOD
#define KBRD_I7_Pin GPIO_PIN_14
#define KBRD_I7_GPIO_Port GPIOD
#define AD7606_CONV_Pin GPIO_PIN_6
#define AD7606_CONV_GPIO_Port GPIOC
#define AD7606_OS0_Pin GPIO_PIN_7
#define AD7606_OS0_GPIO_Port GPIOC
#define AD7606_OS1_Pin GPIO_PIN_8
#define AD7606_OS1_GPIO_Port GPIOC
#define AD7606_OS2_Pin GPIO_PIN_9
#define AD7606_OS2_GPIO_Port GPIOC
#define MODBUS1_DIR_Pin GPIO_PIN_8
#define MODBUS1_DIR_GPIO_Port GPIOA
#define MODBUS1_TX_Pin GPIO_PIN_9
#define MODBUS1_TX_GPIO_Port GPIOA
#define MODBUS1_RX_Pin GPIO_PIN_10
#define MODBUS1_RX_GPIO_Port GPIOA
#define INPUT_VV_READY_Pin GPIO_PIN_15
#define INPUT_VV_READY_GPIO_Port GPIOA
#define W5500_SCK_Pin GPIO_PIN_10
#define W5500_SCK_GPIO_Port GPIOC
#define W5500_MISO_Pin GPIO_PIN_11
#define W5500_MISO_GPIO_Port GPIOC
#define W5500_MOSI_Pin GPIO_PIN_12
#define W5500_MOSI_GPIO_Port GPIOC
#define INPUT_VV_ON_Pin GPIO_PIN_0
#define INPUT_VV_ON_GPIO_Port GPIOD
#define INPUT_VV_OFF_Pin GPIO_PIN_1
#define INPUT_VV_OFF_GPIO_Port GPIOD
#define W5500_INT_Pin GPIO_PIN_2
#define W5500_INT_GPIO_Port GPIOD
#define W5500_INT_EXTI_IRQn EXTI2_IRQn
#define INPUT_PWR_FLT_Pin GPIO_PIN_3
#define INPUT_PWR_FLT_GPIO_Port GPIOD
#define INPUT_RES_PWR_Pin GPIO_PIN_4
#define INPUT_RES_PWR_GPIO_Port GPIOD
#define INPUT_DOOR_CTRL_Pin GPIO_PIN_5
#define INPUT_DOOR_CTRL_GPIO_Port GPIOD
#define INPUT_VV_COIL_FLT_Pin GPIO_PIN_6
#define INPUT_VV_COIL_FLT_GPIO_Port GPIOD
#define INPUT_DV8_Pin GPIO_PIN_7
#define INPUT_DV8_GPIO_Port GPIOD
#define KBRD_O5_Pin GPIO_PIN_4
#define KBRD_O5_GPIO_Port GPIOB
#define KBRD_O4_Pin GPIO_PIN_5
#define KBRD_O4_GPIO_Port GPIOB
#define MODBUS2_DIR_Pin GPIO_PIN_9
#define MODBUS2_DIR_GPIO_Port GPIOB
#define OUT_RELE_5_Pin GPIO_PIN_0
#define OUT_RELE_5_GPIO_Port GPIOE
#define OUT_RELE_4_Pin GPIO_PIN_1
#define OUT_RELE_4_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
