/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MAX7219.h"
#include "AD7606.h"
#include "ModBus.h"
#include "Settings.h"
#include "Buttons.h"
#include "USB.h"
#include "Inputs.h"
#include "VV.h"
#include "Protection.h"
#include "W5500_IEC104.h"
#include "Events.h"
#include "time.h"
#include "mb.h"
#include "socket.h"
#include "Iec104.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

osThreadId Task_MainHandle;
osThreadId Task_ModbusHandle;
osThreadId Task_EventsHandle;
osThreadId Task_SettingsHandle;
osThreadId Task_USBHandle;
osThreadId Task_IEC104Handle;
osThreadId Task_VVHandle;
osThreadId Task_ProtectionHandle;
osMessageQId EventsQueueHandle;
osMessageQId AD7606_QueueHandle;
osTimerId TimerMAX7219RefreshHandle;
osTimerId TimerButtonsReadHandle;
osTimerId Timer_SecondsHandle;
osSemaphoreId ADC_Data_SemHandle;
/* USER CODE BEGIN PV */
bool Setup_mode = false, MAX7219_ChkMode = false;
bool MSZ1vv_setup, SVvv_setup, APV_T2vv_setup, ZMN_Ton_vv_setup;
uint16_t Imsz1_setup, Tmsz1_setup, Imsz2_setup, Tmsz2_setup, Isv_setup, Tsv_setup, APV_Tgot_setup, APV_T1_setup, APV_T2_setup, ZMN_Lvl_setup, ZMN_Toff_setup, ZMN_Ton_setup;
bool Imsz1_blnk = false, Tmsz1_blnk = false, Imsz2_blnk = false, Tmsz2_blnk = false, Isv_blnk = false, Tsv_blnk = false, APV_Tgot_blnk = false, APV_T1_blnk = false, APV_T2_blnk = false, ZMN_Lvl_blnk = false, ZMN_Toff_blnk = false, ZMN_Ton_blnk = false, Time_blnk = false;
uint8_t Internal_Faults = 0, Setup_error_flg = 0, MSZ_work_flg = 0, Task_control = 0;
bool INP_DoorOpen;
uint16_t Imsz1A = 0, Imsz1B = 0, Tmsz1A = 0, Tmsz1B = 0;
bool MAX7219_LED_Block, MAX7219_LED_Nespr, MAX7219_LED_ErrZv, MAX7219_LED_Reserve, MAX7219_LED_ExtPow;

uint8_t Year, Month, Date, Hours, Minutes, Seconds;

uint16_t VrmsA, IrmsA, IrmsC;
uint16_t WorkAPV_ZMN_timer = 0;
uint16_t MAX7219_Refresh_cnt = 0, IEC104_PerSend_cnt = 0;
////////////////////////////////////////////////////////////////
bool Summer_Time = false;
int8_t Temp;
unsigned short int Setup2_View_cnt = 0, Save_Settings_Del_cnt = 0, Save_Counts_Del_cnt = 0, Sec_cnt = 0, Min_cnt = 0;
unsigned short int Setup2_En_cnt = 0, DUvv_cnt = 0, Setup1Error_cnt = 0, LEDdigit_cnt = 0;
unsigned char Setup2_View  = 0, MSZ_state = 0, APV_cikl = 0;
unsigned int Setup_En_cnt = 0;

bool VV_Unblock_f = false, ADC_RST_f = false;
bool Save_Settings1_f = false, Save_Settings2_f = false, Save_Counts_f = false, Save_Counts_Del_f = false;
bool Setup2_En = false;
bool Save_Settings_Del_f = false;
bool USB_Read_f = false;

char USB_Send_buf[200], USB_Read_buf[200], USB_Cmd_buf[200];
unsigned short int ADC_Read_Buf[4];
unsigned char USB_Cmd_pos = 0;
unsigned int USB_Menu = 0;

unsigned int VrmsA_d, IrmsA_d, IrmsC_d;
unsigned int VrmsA_poshk = 0, IrmsA_poshk = 0, IrmsC_poshk = 0;
bool RMS_process = false, Debug_mode = false;

bool VV_Block_f = false, Command_VV_On = false, Command_VV_Off = false;
unsigned int ChasGotovn_timer = 0, VV_Ready_Timer = 0, VV_ReadyFault_Timer = 0;
unsigned int Timer_OZZ = 0, MSZ_APV_timer = 0;
unsigned int MSZ1A_timer = 0, MSZ2A_timer = 0, MSZsvA_timer = 0, MSZ1B_timer = 0, MSZ2B_timer = 0, MSZsvB_timer = 0, Min_timer = 0;
unsigned char MSZ_APV_cikl = 0, OZZ_state = 0, MSZ_Worked_flags = 0, VV_Faults = 0, VV_Faults_old = 0;

bool VV_Off_State = false, VV_On_State = false, VV_Ready_State = false;
unsigned char VV_On_Sourse = 0, VV_Off_Sourse = 0, VV_On_Sourse_tmp = 0, VV_Off_Sourse_tmp = 0, VV_State = 0;

unsigned int Control_VV_Off_timer = 0, Control_VV_On_timer = 0, MB_CoilFault_timer = 0;
unsigned short int ADC_BUSY_timer = 10, Int_Flts_cnt = 0;
bool LED_state = true, Save_State_f = false, Read_Temp_f = false;

bool MB_PuskMSZ1 = false, MB_VozvratMSZ1 = false, MB_WorkMSZ1 = false;
bool MB_PuskMSZ2 = false, MB_VozvratMSZ2 = false, MB_WorkMSZ2 = false;
bool MB_PuskSV = false, MB_WorkSV = false, MB_GotovAPV = false;
bool MB_PuskAPV1 = false, MB_WorkAPV1 = false, MB_PuskAPV2 = false, MB_WorkAPV2 = false;
bool MB_PuskZMN = false, MB_VozvratZMN = false, MB_WorkZMN = false, MB_PuskAPV_ZMN = false;
bool MB_VozvratAPV_ZMN = false, MB_WorkAPV_ZMN = false;

bool IEC_PuskMSZ1 = false, IEC_VozvratMSZ1 = false, IEC_WorkMSZ1 = false;
bool IEC_PuskMSZ2 = false, IEC_VozvratMSZ2 = false, IEC_WorkMSZ2 = false;
bool IEC_PuskSV = false, IEC_WorkSV = false, IEC_GotovAPV = false;
bool IEC_PuskAPV1 = false, IEC_WorkAPV1 = false, IEC_PuskAPV2 = false, IEC_WorkAPV2 = false;
bool IEC_PuskZMN = false, IEC_VozvratZMN = false, IEC_WorkZMN = false, IEC_PuskAPV_ZMN = false;
bool IEC_VozvratAPV_ZMN = false, IEC_WorkAPV_ZMN = false, IEC_VV_Block = false, IEC_VV_Fault = false, IEC_Spr_Flt = false;
bool IEC_Coil_Flt = false;
bool IEC_Connect = false;
unsigned int IEC_VrmsA = 0, IEC_IrmsA = 0, IEC_IrmsC = 0;
unsigned char IEC_VrmsA_D = 5, IEC_IrmsA_D = 5, IEC_IrmsC_D = 5;
uint16_t IEC_VV_Fault_timer = 0, IEC_Connect_timer = 0;
uint8_t DelayStartWD = 10;

char Iec104_RecvBuf[512];
uint16_t Iec104_RecvLen;
uint16_t W5500rst_cnt = 0, W5500rst_cntP = 0;

void Setup1End(void)
{
	if(((Imsz1_setup < Imsz2_setup) || (!MSZ1vv_setup)) && ((Imsz2_setup < Isv_setup) || (!SVvv_setup)) && (((Tmsz1_setup * 100) >= (Tmsz2_setup * 10)) || (!MSZ1vv_setup)) && (((Tmsz2_setup * 10) >= Tsv_setup)  || (!SVvv_setup)))
	{
		Setup_mode = false;
		Setup_error_flg = 0;
		if(MSZ1vv_setup)
		{
			if(Imsz1 != Imsz1_setup)
			{
				Set_Imsz1(Imsz1_setup);
			}
		}
		if(SVvv_setup)
		{
			if(Isv != Isv_setup)
			{
				Set_Isv(Isv_setup);
			}
		}
		if(MSZ1vv != MSZ1vv_setup)
		{
			Set_MSZ1vv(MSZ1vv_setup);
		}
		if(SVvv != SVvv_setup)
		{
			Set_SVvv(SVvv_setup);
		}
		if(APV_T2vv != APV_T2vv_setup)
		{
			Set_APV_T2vv(APV_T2vv_setup);
		}
		if(ZMN_Ton_vv != ZMN_Ton_vv_setup)
		{
			Set_ZMN_Ton_vv(ZMN_Ton_vv_setup);
		}
		if(Tmsz1 != Tmsz1_setup)
		{
			Set_Tmsz1(Tmsz1_setup);
		}
		if(Imsz2 != Imsz2_setup)
		{
			Set_Imsz2(Imsz2_setup);
		}
		if(Tmsz2 != Tmsz2_setup)
		{
			Set_Tmsz2(Tmsz2_setup);
		}
		if(Tsv != Tsv_setup)
		{
			Set_Tsv(Tsv_setup);
		}
		if(APV_Tgot != APV_Tgot_setup)
		{
			Set_Tgot(APV_Tgot_setup);
		}
		if(APV_T1 != APV_T1_setup)
		{
			Set_APVt1(APV_T1_setup);
		}
		if(APV_T2 != APV_T2_setup)
		{
			Set_APVt2(APV_T2_setup);
		}
		if(ZMN_Lvl != ZMN_Lvl_setup)
		{
			Set_ZMNlvl(ZMN_Lvl_setup);
		}
		if(ZMN_Toff != ZMN_Toff_setup)
		{
			Set_ZMNoff(ZMN_Toff_setup);
		}
		if(ZMN_Ton != ZMN_Ton_setup)
		{
			Set_ZMNon(ZMN_Ton_setup);
		}
		Save_Settings2_f = true;
	}
	else
	{
		Setup1Error_cnt = 20;
		if(Imsz1_setup >= Imsz2_setup)
		{
			Setup_error_flg |= 1;
		}
		if(Imsz2_setup >= Isv_setup)
		{
			Setup_error_flg |= 2;
		}
		if((Tmsz1_setup * 100) < (Tmsz2_setup * 10))
		{
			Setup_error_flg |= 4;
		}
		if((Tmsz2_setup * 10) < Tsv_setup)
		{
			Setup_error_flg |= 8;
		}
	}
}

void Setup2End(void)
{
	Setup2_En = false;
	if(Setup2_View != 0)
	{
		Setup2_View_cnt = 30000;
	}
	Save_Settings2_f = true;
	ModBus_Init();
}

uint8_t Cnt200ms = 0;

void Timers_Other(void) {
	Cnt200ms++;
	if(Cnt200ms == 200) {
		Cnt200ms = 0;
		if(MAX7219_Brightness_cur != MAX7219_Brightness) {
			if(MAX7219_Brightness_cur > MAX7219_Brightness) {
				MAX7219_Brightness_cur--;
			} else {
				MAX7219_Brightness_cur++;
			}
		}
	}
	if(IEC_VV_Fault_timer != 0)
	{
		IEC_VV_Fault_timer--;
		if(IEC_VV_Fault_timer == 0)
		{
			IEC_VV_Fault = true;
//			IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+1);//102 VV Fault
		}
	}
	if(VV_Faults & 1) {
		if(!IEC_Spr_Flt) {
			IEC_Spr_Flt = true;
//			IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+2);//103 VV Spring Fault
		}
	} else {
		if(IEC_Spr_Flt) {
			IEC_Spr_Flt = false;
//			IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+2);//103 VV Spring Fault None
		}
	}
	if(VV_Faults & 32) {
		if(!IEC_Coil_Flt) {
			IEC_Coil_Flt = true;
//			IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+3);//104 VV Coil Fault
		}
	} else {
		if(IEC_Coil_Flt) {
			IEC_Coil_Flt = false;
//			IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+3);//104 VV Coil Fault None
		}
	}
	if(MB_CoilFault_timer != 0)
	{
		MB_CoilFault_timer--;
		if(MB_CoilFault_timer == 0)
		{
			VV_Faults |= 32;
		}
	}
	if(Control_VV_Off_timer != 0)
	{
		Control_VV_Off_timer--;
	}
	if(Control_VV_On_timer != 0)
	{
		Control_VV_On_timer--;
	}
	if(ChasGotovn_timer != 0)
	{
		ChasGotovn_timer--;
		if(ChasGotovn_timer == 0)
		{
			MSZ_APV_cikl = 0;
			MSZ_Worked_flags = 0;
			Save_State_f = true;
			GotovAPV();
		}
	}
	if(VV_ReadyFault_Timer != 0) {
		VV_ReadyFault_Timer--;
		if(VV_ReadyFault_Timer == 0) {
			VV_Faults |= 1;
		}
	}
	if(VV_Ready_Timer != 0)
	{
		VV_Ready_Timer--;
	}
	if(MSZ_APV_timer != 0)
	{
		MSZ_APV_timer--;
	}

	if((MSZ2A_timer == 0) && (MSZ2A_timer != 0xFFFFFFFF))
	{
		MSZ1A_timer++;
	}
	if((MSZ2B_timer == 0) && (MSZ2B_timer != 0xFFFFFFFF))
	{
		MSZ1B_timer++;
	}
	if(MSZ2A_timer != 0)
	{
		MSZ2A_timer--;
		if(MSZ2A_timer == 0)
		{
			MSZ_state = 1;
			MSZ_Worked_flags &= 255 - 1;
			MSZ_Worked_flags |= 2;
			Save_State_f = true;
		}
	}
	if(MSZsvA_timer != 0)
	{
		MSZsvA_timer--;
		if(MSZsvA_timer == 0)
		{
			MSZ_state = 1;
			MSZ_Worked_flags &= 255 - 3;
			MSZ_Worked_flags |= 4;
			Save_State_f = true;
		}
	}
	if(MSZ2B_timer != 0)
	{
		MSZ2B_timer--;
		if(MSZ2B_timer == 0)
		{
			MSZ_state = 1;
			MSZ_Worked_flags &= 255 - 1;
			MSZ_Worked_flags |= 2;
			Save_State_f = true;
		}
	}
	if(MSZsvB_timer != 0)
	{
		MSZsvB_timer--;
		if(MSZsvB_timer == 0)
		{
			MSZ_state = 1;
			MSZ_Worked_flags &= 255 - 3;
			MSZ_Worked_flags |= 4;
			Save_State_f = true;
		}
	}
	if(Timer_OZZ != 0)
	{
		Timer_OZZ--;
		if(Timer_OZZ == 0)
		{
			OZZ_state++;
		}
	}
	Sec_cnt++;
	if(Sec_cnt == 1000)
	{
		Sec_cnt = 0;
		Min_cnt++;
		if(Min_cnt == 60)
		{
			Min_cnt = 0;
			
		}
		if(Save_Counts_Del_cnt != 0) {
			Save_Counts_Del_cnt--;
			if(Save_Counts_Del_cnt == 0)
			{
				Save_Counts_f = true;
			}
		}
		if(IEC_Connect_timer != 0)
		{
			IEC_Connect_timer--;
		}
		if(DUvv_cnt != 0)
		{
			DUvv_cnt--;
			if(DUvv_cnt == 0)
			{
				Set_DUvv(true);
			}
		}
		if(Setup1Error_cnt != 0)
		{
			Setup1Error_cnt--;
			if(Setup1Error_cnt == 0)
			{
				Setup_mode = false;
				Setup_error_flg = 0;
				if(MSZ1vv_setup)
				{
					Set_Imsz1(Imsz1_setup);
				}
				if(SVvv_setup)
				{
					Set_Isv(Isv_setup);
				}
				if(APV_T2vv_setup)
				{
					Set_APVt2(APV_T2_setup);
				}
				if(MSZ1vv != MSZ1vv_setup)
				{
					Set_MSZ1vv(MSZ1vv_setup);
				}
				if(SVvv != SVvv_setup)
				{
					Set_SVvv(SVvv_setup);
				}
				if(APV_T2vv != APV_T2vv_setup)
				{
					Set_APV_T2vv(APV_T2vv_setup);
				}
				if(ZMN_Ton_vv != ZMN_Ton_vv_setup)
				{
					Set_ZMN_Ton_vv(ZMN_Ton_vv_setup);
				}
				if(Tmsz1 != Tmsz1_setup)
				{
					Set_Tmsz1(Tmsz1_setup);
				}
				if(Imsz2 != Imsz2_setup)
				{
					Set_Imsz2(Imsz2_setup);
				}
				if(Tmsz2 != Tmsz2_setup)
				{
					Set_Tmsz2(Tmsz2_setup);
				}
				if(Tsv != Tsv_setup)
				{
					Set_Tsv(Tsv_setup);
				}
				if(APV_Tgot != APV_Tgot_setup)
				{
					Set_Tgot(APV_Tgot_setup);
				}
				if(APV_T1 != APV_T1_setup)
				{
					Set_APVt1(APV_T1_setup);
				}
				if(ZMN_Lvl != ZMN_Lvl_setup)
				{
					Set_ZMNlvl(ZMN_Lvl_setup);
				}
				if(ZMN_Toff != ZMN_Toff_setup)
				{
					Set_ZMNoff(ZMN_Toff_setup);
				}
				if(ZMN_Ton != ZMN_Ton_setup)
				{
					Set_ZMNon(ZMN_Ton_setup);
				}
				Save_Settings2_f = true;
			}
		}
	}
	if(Setup_En_cnt != 0)
	{
		Setup_En_cnt--;
		if(Setup_En_cnt == 0)
		{
			Setup1End();
		}
	}
	if(Setup2_En_cnt != 0)
	{
		Setup2_En_cnt--;
		if(Setup2_En_cnt == 0)
		{
			Setup2End();
		}
	}
	if(Setup2_View_cnt != 0)
	{
		Setup2_View_cnt--;
		if(Setup2_View_cnt == 0)
		{
			Setup2_View = 0;
		}
	}
	if(Save_Settings_Del_cnt != 0)
	{
		Save_Settings_Del_cnt--;
		if(Save_Settings_Del_cnt == 0)
		{
			Save_Settings2_f = true;
		}
	}
	if(WorkAPV_ZMN_timer != 0) {
		WorkAPV_ZMN_timer--;
		if(MB_PuskMSZ1 || MB_PuskMSZ2 || MB_PuskSV) {
			MSZ_state = 1;
			if(MB_PuskMSZ1) {
				MSZ_Worked_flags = 1;
			}
			if(MB_PuskMSZ2) {
				MSZ_Worked_flags = 2;
			}
			if(MB_PuskSV) {
				MSZ_Worked_flags = 4;
			}
		}
	}
	if(VV_Cmd_Timer != 0) {
		VV_Cmd_Timer--;
	}
	if(WorkAPV_ZMN_Timer != 0) {
		WorkAPV_ZMN_Timer--;
	}
}
////////////////////////////////////////////////////////////////
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM5_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI3_Init(void);
static void MX_IWDG_Init(void);
void StartTask_Main(void const * argument);
void StartTask_Modbus(void const * argument);
void StartTask_Events(void const * argument);
void StartTask_Settings(void const * argument);
void StartTask_USB(void const * argument);
void StartTask_IEC104(void const * argument);
void StartTask_VV(void const * argument);
void StartTask_Protection(void const * argument);
void CallbackMAX7219Refresh(void const * argument);
void CallbackButtonsRead(void const * argument);
void Callback_Seconds(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM5_Init();
  MX_SPI1_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_SPI3_Init();
//  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	Time_Get();
	Settings_Init();
	MAX7219_Init();
	AD7606_Init();
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim7);
	ModBus_Init();
	Inputs_Init();
	IEC104_Init();
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of ADC_Data_Sem */
  osSemaphoreDef(ADC_Data_Sem);
  ADC_Data_SemHandle = osSemaphoreCreate(osSemaphore(ADC_Data_Sem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of TimerMAX7219Refresh */
  osTimerDef(TimerMAX7219Refresh, CallbackMAX7219Refresh);
  TimerMAX7219RefreshHandle = osTimerCreate(osTimer(TimerMAX7219Refresh), osTimerPeriodic, NULL);

  /* definition and creation of TimerButtonsRead */
  osTimerDef(TimerButtonsRead, CallbackButtonsRead);
  TimerButtonsReadHandle = osTimerCreate(osTimer(TimerButtonsRead), osTimerPeriodic, NULL);

  /* definition and creation of Timer_Seconds */
  osTimerDef(Timer_Seconds, Callback_Seconds);
  Timer_SecondsHandle = osTimerCreate(osTimer(Timer_Seconds), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  osTimerStart(TimerMAX7219RefreshHandle, 100);
  osTimerStart(TimerButtonsReadHandle, 20);
  osTimerStart(Timer_SecondsHandle, 1000);
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of EventsQueue */
  osMessageQDef(EventsQueue, 32, uint32_t);
  EventsQueueHandle = osMessageCreate(osMessageQ(EventsQueue), NULL);

  /* definition and creation of AD7606_Queue */
  osMessageQDef(AD7606_Queue, 5, 6);
  AD7606_QueueHandle = osMessageCreate(osMessageQ(AD7606_Queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task_Main */
  osThreadDef(Task_Main, StartTask_Main, osPriorityNormal, 0, 128);
  Task_MainHandle = osThreadCreate(osThread(Task_Main), NULL);

  /* definition and creation of Task_Modbus */
  osThreadDef(Task_Modbus, StartTask_Modbus, osPriorityNormal, 0, 128);
  Task_ModbusHandle = osThreadCreate(osThread(Task_Modbus), NULL);

  /* definition and creation of Task_Events */
  osThreadDef(Task_Events, StartTask_Events, osPriorityNormal, 0, 128);
  Task_EventsHandle = osThreadCreate(osThread(Task_Events), NULL);

  /* definition and creation of Task_Settings */
  osThreadDef(Task_Settings, StartTask_Settings, osPriorityNormal, 0, 128);
  Task_SettingsHandle = osThreadCreate(osThread(Task_Settings), NULL);

  /* definition and creation of Task_USB */
  osThreadDef(Task_USB, StartTask_USB, osPriorityNormal, 0, 128);
  Task_USBHandle = osThreadCreate(osThread(Task_USB), NULL);

  /* definition and creation of Task_IEC104 */
  osThreadDef(Task_IEC104, StartTask_IEC104, osPriorityNormal, 0, 128);
  Task_IEC104Handle = osThreadCreate(osThread(Task_IEC104), NULL);

  /* definition and creation of Task_VV */
  osThreadDef(Task_VV, StartTask_VV, osPriorityNormal, 0, 128);
  Task_VVHandle = osThreadCreate(osThread(Task_VV), NULL);

  /* definition and creation of Task_Protection */
  osThreadDef(Task_Protection, StartTask_Protection, osPriorityNormal, 0, 128);
  Task_ProtectionHandle = osThreadCreate(osThread(Task_Protection), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	LogWrite(0x01000000);	
  osMessagePut(EventsQueueHandle, 0x01000000, osWaitForever);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 31;
  hrtc.Init.SynchPrediv = 1023;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(0)
	{
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	}
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 83;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 999;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 19200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, OUT_RELE_3_Pin|OUT_RELE_2_Pin|OUT_RELE_1_Pin|OUT_VV_OFF_RELE_Pin 
                          |OUT_VV_ON_RELE_Pin|LED_VV_ON_Pin|LED_VV_OFF_Pin|OUT_RELE_5_Pin 
                          |OUT_RELE_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, TEST_LED_Pin|OUT_VV_ON_MSFT_Pin|OUT_VV_OFF_MSFT_Pin|AD7606_RESET_Pin 
                          |AD7606_CONV_Pin|AD7606_OS0_Pin|AD7606_OS1_Pin|AD7606_OS2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(AD7606_CS_GPIO_Port, AD7606_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, KBRD_O1_Pin|KBRD_O2_Pin|KBRD_O3_Pin|MAX7219_CS_Pin 
                          |KBRD_O5_Pin|KBRD_O4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, W5500_RESET_Pin|W5500_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TEST_OUT_Pin|MODBUS2_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MODBUS1_DIR_GPIO_Port, MODBUS1_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT_RELE_3_Pin OUT_RELE_2_Pin OUT_RELE_1_Pin OUT_VV_OFF_RELE_Pin 
                           OUT_VV_ON_RELE_Pin LED_VV_ON_Pin LED_VV_OFF_Pin W5500_RESET_Pin 
                           W5500_CS_Pin OUT_RELE_5_Pin OUT_RELE_4_Pin */
  GPIO_InitStruct.Pin = OUT_RELE_3_Pin|OUT_RELE_2_Pin|OUT_RELE_1_Pin|OUT_VV_OFF_RELE_Pin 
                          |OUT_VV_ON_RELE_Pin|LED_VV_ON_Pin|LED_VV_OFF_Pin|W5500_RESET_Pin 
                          |W5500_CS_Pin|OUT_RELE_5_Pin|OUT_RELE_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : TEST_LED_Pin OUT_VV_ON_MSFT_Pin OUT_VV_OFF_MSFT_Pin AD7606_RESET_Pin 
                           AD7606_CONV_Pin AD7606_OS0_Pin AD7606_OS1_Pin AD7606_OS2_Pin */
  GPIO_InitStruct.Pin = TEST_LED_Pin|OUT_VV_ON_MSFT_Pin|OUT_VV_OFF_MSFT_Pin|AD7606_RESET_Pin 
                          |AD7606_CONV_Pin|AD7606_OS0_Pin|AD7606_OS1_Pin|AD7606_OS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AD7606_CS_Pin MODBUS1_DIR_Pin */
  GPIO_InitStruct.Pin = AD7606_CS_Pin|MODBUS1_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : AD7606_BUSY_Pin */
  GPIO_InitStruct.Pin = AD7606_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AD7606_BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KBRD_O1_Pin KBRD_O2_Pin KBRD_O3_Pin MAX7219_CS_Pin 
                           TEST_OUT_Pin KBRD_O5_Pin KBRD_O4_Pin MODBUS2_DIR_Pin */
  GPIO_InitStruct.Pin = KBRD_O1_Pin|KBRD_O2_Pin|KBRD_O3_Pin|MAX7219_CS_Pin 
                          |TEST_OUT_Pin|KBRD_O5_Pin|KBRD_O4_Pin|MODBUS2_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_VV_ON_Pin KEY_VV_OFF_Pin */
  GPIO_InitStruct.Pin = KEY_VV_ON_Pin|KEY_VV_OFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : KBRD_I1_Pin KBRD_I2_Pin KBRD_I3_Pin KBRD_I4_Pin 
                           KBRD_I5_Pin KBRD_I6_Pin KBRD_I7_Pin */
  GPIO_InitStruct.Pin = KBRD_I1_Pin|KBRD_I2_Pin|KBRD_I3_Pin|KBRD_I4_Pin 
                          |KBRD_I5_Pin|KBRD_I6_Pin|KBRD_I7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT_VV_READY_Pin */
  GPIO_InitStruct.Pin = INPUT_VV_READY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INPUT_VV_READY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INPUT_VV_ON_Pin INPUT_VV_OFF_Pin INPUT_PWR_FLT_Pin INPUT_RES_PWR_Pin 
                           INPUT_DOOR_CTRL_Pin INPUT_VV_COIL_FLT_Pin INPUT_DV8_Pin */
  GPIO_InitStruct.Pin = INPUT_VV_ON_Pin|INPUT_VV_OFF_Pin|INPUT_PWR_FLT_Pin|INPUT_RES_PWR_Pin 
                          |INPUT_DOOR_CTRL_Pin|INPUT_VV_COIL_FLT_Pin|INPUT_DV8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : W5500_INT_Pin */
  GPIO_InitStruct.Pin = W5500_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(W5500_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTask_Main */
/**
  * @brief  Function implementing the Task_Main thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartTask_Main */
void StartTask_Main(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 5 */
	MX_IWDG_Init();
  /* Infinite loop */
  for(;;)
  {
		if(Task_control == 0)
		{
			HAL_IWDG_Refresh(&hiwdg);
			Task_control = 1 + 2 + 4 + 8 + 16;
		}
		if(IEC_Connect_timer == 0)
		{
 			osThreadTerminate(Task_IEC104Handle);
			osThreadDef(Task_IEC104, StartTask_IEC104, osPriorityNormal, 0, 128);
			Task_IEC104Handle = osThreadCreate(osThread(Task_IEC104), NULL);
			IEC_Connect = false;
			IEC_Connect_timer = _IEC_Connect_timer;
			W5500rst_cnt++;
			W5500rst_cntP++;
		}
		if(Save_Settings_Del_f)
		{
			Save_Settings_Del_f = false;
			Save_Settings_Del_cnt = 3000;
		}
		if(Save_Counts_Del_f)
		{
			Save_Counts_Del_f = false;
			if(Save_Counts_Del_cnt == 0) {
				Save_Counts_Del_cnt = 60;
			}
		}
		if(ADC_RST_f)
		{
			AD7606_Init();
			ADC_RST_f = false;
		}
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTask_Modbus */
/**
* @brief Function implementing the Task_Modbus thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Modbus */
void StartTask_Modbus(void const * argument)
{
  /* USER CODE BEGIN StartTask_Modbus */
  /* Infinite loop */
  for(;;)
  {
		Task_control &= 0xFF - 1;
		ModBus_Poll();
    osDelay(10);
  }
  /* USER CODE END StartTask_Modbus */
}

/* USER CODE BEGIN Header_StartTask_Events */
/**
* @brief Function implementing the Task_Events thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Events */
void StartTask_Events(void const * argument)
{
  /* USER CODE BEGIN StartTask_Events */
  /* Infinite loop */
  for(;;)
  {
		Events_Poll();
  }
  /* USER CODE END StartTask_Events */
}

/* USER CODE BEGIN Header_StartTask_Settings */
/**
* @brief Function implementing the Task_Settings thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Settings */
void StartTask_Settings(void const * argument)
{
  /* USER CODE BEGIN StartTask_Settings */
  /* Infinite loop */
  for(;;)
  {
		Task_control &= 0xFF - 2;
		Settings_Poll();
    osDelay(10);
  }
  /* USER CODE END StartTask_Settings */
}

/* USER CODE BEGIN Header_StartTask_USB */
/**
* @brief Function implementing the Task_USB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_USB */
void StartTask_USB(void const * argument)
{
  /* USER CODE BEGIN StartTask_USB */
  /* Infinite loop */
  for(;;)
  {
		Task_control &= 0xFF - 4;
		USB_Poll();
    osDelay(5);
  }
  /* USER CODE END StartTask_USB */
}

/* USER CODE BEGIN Header_StartTask_IEC104 */
/**
* @brief Function implementing the Task_IEC104 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_IEC104 */
void StartTask_IEC104(void const * argument)
{
  /* USER CODE BEGIN StartTask_IEC104 */
  /* Infinite loop */
  for(;;)
  {
		IEC104_Poll();
    osDelay(1);
  }
  /* USER CODE END StartTask_IEC104 */
}

/* USER CODE BEGIN Header_StartTask_VV */
/**
* @brief Function implementing the Task_VV thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_VV */
void StartTask_VV(void const * argument)
{
  /* USER CODE BEGIN StartTask_VV */
  /* Infinite loop */
  for(;;)
  {
		Task_control &= 0xFF - 8;
		VV_Poll();
    osDelay(10);
  }
  /* USER CODE END StartTask_VV */
}

/* USER CODE BEGIN Header_StartTask_Protection */
/**
* @brief Function implementing the Task_Protection thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Protection */
void StartTask_Protection(void const * argument)
{
  /* USER CODE BEGIN StartTask_Protection */
  /* Infinite loop */
  for(;;)
  {
		Task_control &= 0xFF - 16;
		Protection_Poll();
//		osThreadYield();
    osDelay(1);
		
  }
  /* USER CODE END StartTask_Protection */
}

/* CallbackMAX7219Refresh function */
void CallbackMAX7219Refresh(void const * argument)
{
  /* USER CODE BEGIN CallbackMAX7219Refresh */
  /* USER CODE END CallbackMAX7219Refresh */
}

/* CallbackButtonsRead function */
void CallbackButtonsRead(void const * argument)
{
  /* USER CODE BEGIN CallbackButtonsRead */
	IEC104_PerSend_cnt++;
	if(IEC104_PerSend_cnt == 1500) {
		IEC104_PerSend_cnt = 0;
		IEC104_BuildDetectF_Spont(1, (float)IrmsA, IEC104_INFOADDR_VALUE_HXGF+1);
		IEC104_BuildDetectF_Spont(1, (float)IrmsC, IEC104_INFOADDR_VALUE_HXGF+2);
		IEC104_BuildDetectF_Spont(1, (float)VrmsA, IEC104_INFOADDR_VALUE_HXGF+3);
	}
	MAX7219_Refresh_cnt++;
	if(MAX7219_Refresh_cnt == 5) {
		MAX7219_Refresh_cnt = 0;
		MAX7219_Refresh();
	}
  Buttons_Read();
  Inputs_Poll();
  /* USER CODE END CallbackButtonsRead */
}

/* Callback_Seconds function */
void Callback_Seconds(void const * argument)
{
  /* USER CODE BEGIN Callback_Seconds */
  Time_Get();
	HAL_GPIO_TogglePin(TEST_LED_GPIO_Port, TEST_LED_Pin);
	HAL_GPIO_TogglePin(TEST_OUT_GPIO_Port, TEST_OUT_Pin);
	Read_Temp_f = true;
	Min_timer++;
	if(Min_timer == 60) {
		Min_timer = 0;
		IEC104_BuildDetectF_Spont(1, (float)(((uint16_t)Minutes << 8) + Seconds), IEC104_INFOADDR_VALUE_HXGF+24);
	}
  /* USER CODE END Callback_Seconds */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	if (htim->Instance == TIM5) {
		MAX7219_Timers();
		AD7606_Timers();
		VV_Timers();
		Timers_Other();
	}
	if (htim->Instance == TIM7) {
		HAL_GPIO_WritePin(AD7606_CONV_GPIO_Port, AD7606_CONV_Pin, GPIO_PIN_SET);
	}
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
