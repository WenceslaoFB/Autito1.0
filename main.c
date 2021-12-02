/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

 typedef union{
        uint8_t u8[4];
        int8_t  i8[4];

        uint16_t u16[2];
        int16_t  i16[2];

        uint32_t u32;
        int32_t  i32;

        float    f;
}_sWork;

typedef union{
    struct{
        uint8_t b0:1;
        uint8_t b1:1;
        uint8_t b2:1;
        uint8_t b3:1;
        uint8_t b4:1;
        uint8_t b5:1;
        uint8_t b6:1;
        uint8_t b7:1;
    }bit;
    uint8_t byte;
}flag;

flag flag1;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

uint16_t ADCData[32][8];
uint8_t indexADC;

#define Flag flag1.bit.b0
#define LENGHT_START 45
#define LENGHT_CWJAP	35
#define LENGHT_CWJAP_DPTO	36
#define LENGHT_ANS_CWJAP 70
#define LENGHT_ANS_CWJAP_DPTO 71
#define UART_PC		2
#define UART_ESP	1

//Comandos
const char ALIVE[]={"AT\r\n"};
const char MODE[]={"AT+CWMODE=3\r\n"};
const char JAP[]={'A','T','+','C','W','J','A','P','=','"','M','I','C','R','O','S','"',',','"','m','i','c','r','o','s','1','2','3','4','5','6','7','"','\r','\n'};
const char JAP_DPTO[]={'A','T','+','C','W','J','A','P','=','"','F','A','M','Y','L','Y',' ','A','V','A','"',',','"','a','l','v','e','a','r','1','5','6','3','"','\r','\n'};
const char CIFSR[] = {"AT+CIFSR\r\n"};
const char MUX[]={"AT+CIPMUX=0\r\n"};
const char START[]={'A','T','+','C','I','P','S','T','A','R','T','=','"','U','D','P','"',',','"','1','9','2','.','1','6','8','.','1','.','1','2','"',',','3','0','0','1','7',',','3','0','1','7','\r','\n'};
const char START2[] = {",30017, 3017 \r\n"};
const char SEND[]="AT+CIPSEND=\r\n";//COLOCAR NUMERO DE BYTS DSP DE =
const char STOP[]= {"AT+CIPCLOSE\r\n"};
const char DISC[]={"AT+CWQAP\r\n"};

//Respuestas a los comandos
const char MODE_ANS[] = {"AT+CWMODE=3\r\n\r\nOK\r\n"};
const char JAP_ANS[] = {'A','T','+','C','W','J','A','P','=','"','M','I','C','R','O','S','"',',','"','m','i','c','r','o','s','1','2','3','4','5','6','7','"',',','\r','\n','W','I','F','I',' ','C','O','N','N','E','C','T','E','D','\r','\n','W','I','F','I',' ','G','O','T','I','P','\r','\n','\r','\n','O','K','\r','\n'};
const char JAP_DPTO_ANS[]={'A','T','+','C','W','J','A','P','=','"','F','A','M','Y','L','Y',' ','A','V','A','"',',','"','a','l','v','e','a','r','1','5','6','3','"',',','\r','\n','W','I','F','I',' ','C','O','N','N','E','C','T','E','D','\r','\n','W','I','F','I',' ','G','O','T','I','P','\r','\n','\r','\n','O','K','\r','\n'};
const char WIFI_CONN[]={"WIFI CONNECTED\r\nWIFI GOT IP\r\n"};
const char CIFSR_IP[]={"+CIFSR:STAIP,"};
const char MUX_ANS[]={"AT+CIPMUX=0\r\n\r\nOK\r\n"};
const char START_ANS[]={'A','T','+','C','I','P','S','T','A','R','T','=','"','U','D','P','"',',','"','1','9','2','.','1','6','8','.','1','.','1','2','"',',','3','0','0','1','7',',','3','0','1','7','\r','\n','C','O','N','N','E','C','T','\r','\n','\r','\n','O','K','\r','\n'};//59
const char DISC_ANS[]={"AT+CWQAP\r\n\r\nOK\r\n"};
const char SEND_ANS[] ={};

//Constantes/mesajes

const char WIFI_CON_AUTO[] = {"WIFI CONNECTED\r\nWIFI GOT IP\r\n"};
const char WIFI_DESC[] = {"WIFI DISCONNECT\r\n"};
const char OK[]={"\r\nOK\r\n"};
const char UNER[] = {'U','N','E','R'};
const char ALIVE2[] = {'U','N','E','R', 0x02 ,':',0xF0};
const char IPD[]= {'\r','\n','+','I','P','D',','};
const char ACK_D0[]={'U','N','E','R',0x03,':',0xD0,0x0D,0xDC};

const char CIPSEND1[]={'A','T','+','C','I','P','S','E','N','D','='};
const char CIPSEND2[]={'\r','\n','\r','\n','O','K','\r','\n','>'};
const char CIPSEND3[]={"Recv"};
const char CIPSEND4[]={" bytes\r\n\r\nSEND OK\r\n"};//25

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint8_t bufferTX_ESP[256];
volatile uint8_t bufferRX_ESP[256];
volatile uint8_t bufferAux[256];
volatile uint8_t indexR_TX_ESP=0;
volatile uint8_t indexW_TX_ESP=0;
volatile uint8_t indexW_RX_ESP=0;
volatile uint8_t indexR_RX_ESP=0;
volatile uint8_t indexW_Aux=0;
volatile uint8_t indexR_Aux=0;
volatile uint8_t timeout1 = 0;
volatile uint8_t timeout2 = 0;
volatile uint8_t timeout3 = 0;
volatile uint8_t timeout4 = 0;

uint8_t AT = 0;
uint8_t tarea = 0;
uint8_t listo_enviar = 0;
uint8_t ESP_recibe = 0;
uint8_t decod_CIPSEND = 0;
uint8_t decod_CIFSR = 0;
uint8_t largo_IP = 0;
uint8_t bytes_send_aux=0;

uint8_t CMD = 0xF0;
uint8_t protUNERcmd;
uint8_t protUNERbytes;
uint8_t cant_bytes;
uint8_t cks;
uint8_t bytes_send;
uint8_t decIPD;

char IP_ESP[15];

_sWork PWM_motor1, PWM_motor2, tiempo_uso;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void uartTX(uint8_t uart);

void initESP();

void decodAnsESP();

void cmdUDP(uint8_t cmd);

void decodCMD();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(&(*huart)==&huart2){
		//indexW_PC++;
		//HAL_UART_Receive_IT(&huart2, &bufferPC[indexW_PC], 1);
	}

	if(&(*huart)==&huart1){
		indexW_RX_ESP++;
		HAL_UART_Receive_IT(&huart1, &bufferRX_ESP[indexW_RX_ESP], 1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM4){
		if(timeout1>0)
			timeout1--;
		if(timeout2>0)
			timeout2--;
		if(timeout3>0)
			timeout3--;
		if(timeout4>0)
			timeout4--;
	}
}

void uartTX(uint8_t uart){

	if(uart == UART_PC ){
		if((huart2.Instance->SR & UART_FLAG_TXE)==UART_FLAG_TXE){
			huart2.Instance->DR=bufferAux[indexR_Aux];
			indexR_Aux++;
		}
	}

	if(uart == UART_ESP ){
		if((huart1.Instance->SR & UART_FLAG_TXE)==UART_FLAG_TXE){
			huart1.Instance->DR=bufferTX_ESP[indexR_TX_ESP];
			indexR_TX_ESP++;
		}
	}
}

void initESP(){

	if(listo_enviar){
		switch(AT){
			case 0:
				memcpy(&bufferTX_ESP[indexW_TX_ESP],MODE,13);
				indexW_TX_ESP+=13;
				timeout2=200;
				listo_enviar=0;
			break;
			case 1:
				timeout2=20000;
				listo_enviar=0;
			break;
			case 2:
				//memcpy(&bufferTX_ESP[indexW_TX_ESP],JAP,LENGHT_CWJAP);
				memcpy(&bufferTX_ESP[indexW_TX_ESP],JAP_DPTO,LENGHT_CWJAP_DPTO);
				indexW_TX_ESP+=LENGHT_CWJAP_DPTO;
				timeout2=2000;
				listo_enviar=0;
			break;
			case 3:
				memcpy(&bufferTX_ESP[indexW_TX_ESP],MUX,13);
				indexW_TX_ESP+=13;
				timeout2=200;
				listo_enviar=0;
			break;
			case 4:
				memcpy(&bufferTX_ESP[indexW_TX_ESP],CIFSR,10);
				indexW_TX_ESP+=10;
				timeout2=4000;
				listo_enviar=0;
				break;
			case 5:
				memcpy(&bufferTX_ESP[indexW_TX_ESP],START,LENGHT_START);
				indexW_TX_ESP+=LENGHT_START;
				timeout2=2000;
				listo_enviar=0;
				break;
			case 6:
				tarea++;
			break;
		}
	}
}

void decodAnsESP(){
	static uint8_t i=0,k=2;


	bufferAux[indexW_Aux]=bufferRX_ESP[indexR_RX_ESP];
	indexW_Aux++;



	switch(AT){
		case 0:
			if(bufferRX_ESP[indexR_RX_ESP]==MODE_ANS[i]){
				i++;
				if(i==19){
					AT++;
					i=0;
					listo_enviar=1;
				}
			}else{
				if(i>0){
					indexR_RX_ESP=indexW_RX_ESP;
					listo_enviar=1;
					i=0;
				}
			}
			indexR_RX_ESP++;
			break;

		case 1:
			if(bufferRX_ESP[indexR_RX_ESP]==WIFI_CON_AUTO[i]){
				i++;
				if(i==29){
					AT=3;
					i=0;
					listo_enviar=1;
				}
			}
			else{
				if(bufferRX_ESP[indexR_RX_ESP]==WIFI_DESC[i]){
					i++;
					if(i==17){
						AT=0;
						i=0;
						listo_enviar=1;
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);//Reset ESP8266
						timeout1=20;
					}
				}else{
					AT=2;
					i=0;
					listo_enviar=1;
				}
			}
			indexR_RX_ESP++;
			break;
		case 2:
			if(bufferRX_ESP[indexR_RX_ESP]==JAP_DPTO_ANS[i]){
				i++;

				if(i==LENGHT_ANS_CWJAP_DPTO){
					AT++;
					i=0;
					listo_enviar=1;
				}
			}
			else{
				if(i>0){
					indexR_RX_ESP=indexW_RX_ESP;
					listo_enviar=1;
					i=0;
					break;
				}
			}
			indexR_RX_ESP++;
			break;
		case 3:
			if(bufferRX_ESP[indexR_RX_ESP]==MUX_ANS[i]){
				i++;
				if(i==19){
					AT++;
					i=0;
					listo_enviar=1;
				}
			}else{
				if(i>0){
					indexR_RX_ESP=indexW_RX_ESP;
					listo_enviar=1;
					i=0;
					break;
				}
			}
			indexR_RX_ESP++;
			break;
		case 4:
			switch(decod_CIFSR){
				case 0:
					if(bufferRX_ESP[indexR_RX_ESP]==CIFSR[i]){
						i++;

						if(i==10){
							decod_CIFSR++;
							i=0;
						}
					}else{
						if(i>0){
							indexR_RX_ESP=indexW_RX_ESP;
							listo_enviar=1;
							i=0;
							break;
						}
					}
					indexR_RX_ESP++;
					break;
				case 1:
					if(bufferRX_ESP[indexR_RX_ESP]==CIFSR_IP[i]){
						i++;
						if(i==13){
							decod_CIFSR++;
							i=0;
						}
					}
					indexR_RX_ESP++;
					break;
				case 2:
					IP_ESP[i]=bufferRX_ESP[indexR_RX_ESP];
					i++;
					largo_IP++;
					indexR_RX_ESP++;
					if((bufferRX_ESP[indexR_RX_ESP]=='"')&&(largo_IP>1)){
						i=0;
						decod_CIFSR++;
					}
					break;
				case 3:
					if(bufferRX_ESP[indexR_RX_ESP]==OK[i]){
						i++;
						if(i==6){
							AT++;
							i=0;
							listo_enviar=1;
							decod_CIFSR=0;
						}
					}
					indexR_RX_ESP++;
					break;
			}

			break;
		case 5:
			if(bufferRX_ESP[indexR_RX_ESP]==START_ANS[i]){
				i++;
				if(i==60){
					AT++;
					i=0;
					listo_enviar=1;
				}
			}else{
				if(i>0){
					indexR_RX_ESP=indexW_RX_ESP;
					listo_enviar=1;
					i=0;
					break;
				}
			}
			indexR_RX_ESP++;
			break;
		case 6:
			switch(decod_CIPSEND){
					case 0:
						if(bufferRX_ESP[indexR_RX_ESP]==CIPSEND1[i]){
							i++;
							if(i==11){
								i=0;
								decod_CIPSEND++;
							}
						}
						else{
							if(i>0){
								indexR_RX_ESP=indexW_RX_ESP;
								listo_enviar=1;
								i=0;
								break;
							}
						}
						indexR_RX_ESP++;
						break;
					case 1:
						if((bufferRX_ESP[indexR_RX_ESP]==bytes_send+'0')&&((bytes_send<10))){
							decod_CIPSEND=3;
						}else{

							if(bufferRX_ESP[indexR_RX_ESP]==bytes_send/10+'0'){
								decod_CIPSEND++;
								bytes_send_aux=bytes_send/10;
								bytes_send_aux*=10;
							}

						}
						indexR_RX_ESP++;
						break;
					case 2:
						if(bufferRX_ESP[indexR_RX_ESP]==bytes_send-bytes_send_aux+'0'){
							decod_CIPSEND++;
						}
						indexR_RX_ESP++;
						break;
					case 3:
						if(bufferRX_ESP[indexR_RX_ESP]==CIPSEND2[i]){
							i++;
							if(i==9){
								i=0;
								decod_CIPSEND++;
								ESP_recibe=1;
								listo_enviar=1;
							}
						}
						indexR_RX_ESP++;
						break;
					case 4:
						if(bufferRX_ESP[indexR_RX_ESP]==CIPSEND3[i]){
							i++;
							if(i==4){
								i=0;
								decod_CIPSEND++;
							}
						}
						indexR_RX_ESP++;
						break;
					case 5:
						if((bufferRX_ESP[indexR_RX_ESP]==bytes_send+'0')&&((bytes_send<10))){
							decod_CIPSEND=7;
						}else{

							if(bufferRX_ESP[indexR_RX_ESP]==bytes_send/10+'0'){
								decod_CIPSEND++;
							}

						}
						indexR_RX_ESP++;
						break;
					case 6:
						if(bufferRX_ESP[indexR_RX_ESP]==bytes_send-bytes_send_aux+'0'){
							decod_CIPSEND++;
						}
						indexR_RX_ESP++;
						break;
					case 7:
						if(bufferRX_ESP[indexR_RX_ESP]==CIPSEND4[i]){
							i++;
							if(i==19){
								i=0;
								decod_CIPSEND=0;
								listo_enviar=1;
								AT++;
							}
						}

						indexR_RX_ESP++;
						break;
				}

			break;
		case 7:
			timeout2=2;
			switch(decIPD){
				case 0:
					if(bufferRX_ESP[indexR_RX_ESP]==IPD[i]){
						i++;

						if(i==7){
							i=0;
							decIPD++;
						}
					}
					else{
						if(i>0){
							indexR_RX_ESP=indexW_RX_ESP;
							i=0;
							break;
						}
					}
					indexR_RX_ESP++;
					break;
				case 1:
					if(bufferRX_ESP[indexR_RX_ESP]==':'){
						decIPD++;
					}
					indexR_RX_ESP++;
					break;
				case 2:
					if(bufferRX_ESP[indexR_RX_ESP]==UNER[i]){
						i++;
						if(i==4){
							i=0;
							decIPD++;
							cks='U'^'N'^'E'^'R';
						}
					}else{
						if(i>0){
							indexR_RX_ESP=indexW_RX_ESP;
							i=0;
							decIPD=0;
							break;
						}
					}
					indexR_RX_ESP++;
					break;
				case 3:
					protUNERbytes=bufferRX_ESP[indexR_RX_ESP];
					decIPD++;
					cks^=bufferRX_ESP[indexR_RX_ESP];
					indexR_RX_ESP++;
					break;
				case 4:
					if(bufferRX_ESP[indexR_RX_ESP]==':'){
						decIPD++;
						cks^=bufferRX_ESP[indexR_RX_ESP];

					}else{
						indexR_RX_ESP=indexW_RX_ESP;
						i=0;
						decIPD=0;
						break;
					}
					indexR_RX_ESP++;
					break;
				case 5:
					protUNERcmd=bufferRX_ESP[indexR_RX_ESP];
					decIPD++;
					cks^=bufferRX_ESP[indexR_RX_ESP];
					indexR_RX_ESP++;
					i=indexR_RX_ESP;
					break;
				case 6:
					if(k<protUNERbytes){
						cks^=bufferRX_ESP[i];
						i++;
						k++;
					}else{
						if(cks==bufferRX_ESP[i]){
							decodCMD();
							indexR_RX_ESP++;
							k=2;
							i=0;
							decIPD=0;
						}else{
							indexR_RX_ESP=indexW_RX_ESP;
							i=0;
							decIPD=0;
							k=2;
						}
					}
					break;
			}
			break;
	}
}

void cmdUDP(uint8_t cmd){

	if(listo_enviar){
		switch(cmd){
			case 0xF0:
				if(!ESP_recibe){
					AT=6;
					memcpy(&bufferTX_ESP[indexW_TX_ESP],SEND,11);
					indexW_TX_ESP+=11;
					memcpy(&bufferTX_ESP[indexW_TX_ESP],"8\r\n",3);
					indexW_TX_ESP+=3;
					bytes_send=8;
					timeout2=400;
					listo_enviar=0;
				}
				else{
					memcpy(&bufferTX_ESP[indexW_TX_ESP],ALIVE2,7);
					indexW_TX_ESP+=7;
					bufferTX_ESP[indexW_TX_ESP]='U'^'N'^'E'^'R'^0x02^':'^0xF0;
					indexW_TX_ESP++;
					ESP_recibe=0;
					tarea++;
					timeout1=400;
				}
			break;
			case 0xD0:
				if(!ESP_recibe){
					AT=6;
					memcpy(&bufferTX_ESP[indexW_TX_ESP],SEND,11);
					indexW_TX_ESP+=11;
					memcpy(&bufferTX_ESP[indexW_TX_ESP],"9\r\n",3);
					indexW_TX_ESP+=3;
					bytes_send=9;
					timeout2=400;
					listo_enviar=0;
				}
				else{
					memcpy(&bufferTX_ESP[indexW_TX_ESP],ACK_D0,9);
					indexW_TX_ESP+=9;
					bufferTX_ESP[indexW_TX_ESP]='U'^'N'^'E'^'R'^0x03^':'^0xD0^0x0D;
					indexW_TX_ESP++;
					ESP_recibe=0;
					tarea++;
				}
			break;
		}
	}
}

void decodCMD(){

	switch(protUNERcmd){
		case 0xF0:

		break;
		case 0xD0:

			//COMANDO EN HEX 55 4E 45 52 0E 3A D0 C8 00 00 00 C8 00 00 00 88 13 00 00 73 (5 SEG)
			//55 4E 45 52 0E 3A D0 C8 00 00 00 C8 00 00 00 E8 03 00 00 03 (1 SEG)
			//55 4E 45 52 0E 3A D0 C8 00 00 00 C8 00 00 00 D0 07 00 00 3F (2 SEG)

			PWM_motor1.u8[0]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor1.u8[1]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor1.u8[2]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor1.u8[3]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;

			PWM_motor2.u8[0]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor2.u8[1]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor2.u8[2]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			PWM_motor2.u8[3]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;

			tiempo_uso.u8[0]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			tiempo_uso.u8[1]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			tiempo_uso.u8[2]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;
			tiempo_uso.u8[3]=bufferRX_ESP[indexR_RX_ESP];
			indexR_RX_ESP++;

			timeout4 = tiempo_uso.u32/100;

			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWM_motor1.u32);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, PWM_motor2.u32);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);

			Flag = 1;
			tarea= 2;
			CMD = 0xD0;
			listo_enviar = 1;

		break;
	}

}
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);/*
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 400);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 400);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
*/

  //uint8_t data_T[6] = "AT\r\n";
  //uint8_t data_R[2] = "OK";
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim4);
  /*HAL_UART_Transmit(&huart1, data_T, 6, HAL_MAX_DELAY);*/
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);//ESP8266 Pin Enable
  timeout2=6000;
  timeout3=2000;
  timeout1=6000;
  listo_enviar=1;
  Flag=0;
  tarea = 0;
  while (1)
  {
	  if(!timeout3){
	  	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  	timeout3=2000;
	  }

	  switch(tarea){
	  	  	  case 0:
	  	  		  if(!timeout1){
	  	  			 initESP();
	  	  			 HAL_UART_Receive_IT(&huart1, &bufferRX_ESP[indexW_RX_ESP], 1);
	  	  		  	 tarea++;
	  	  		  }
	  	  		  break;
	  	  	  case 1:
	  	  		  initESP();
	  	  		  break;
	  	  	  case 2:
	  	  		  cmdUDP(CMD);
	  	  		  break;
	  	  }
	  if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)==GPIO_PIN_RESET)&&(!timeout1)){
	  		  HAL_UART_AbortReceive_IT(&huart1);
	  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);//ESP8266 Pin Enable
	  		  timeout1=30;
	  		  AT=1;
	  		  tarea=0;
	  }
	  if(indexR_TX_ESP!=indexW_TX_ESP){
	  		  uartTX(UART_ESP);
	  	  }

	  if(indexR_Aux!=indexW_Aux){
	  		  uartTX(UART_PC);
	  }

	  if((indexR_RX_ESP!=indexW_RX_ESP)&&(!timeout2)){
		  decodAnsESP();
	  }

	  if((!timeout4)&&(Flag)){
	  		Flag=0;
	  		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,0);
	  		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,0);
	  		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	  		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	  }

}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 8;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 99;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 840;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  htim4.Init.Prescaler = 999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 42;
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
  huart1.Init.BaudRate = 115200;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
