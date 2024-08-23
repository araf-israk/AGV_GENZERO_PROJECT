/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "modbus_crc.h"
#include "line_sensor.h"
#include "LoRa.h"

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
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

// #### RS485 ####

uint8_t RxData[32];
uint8_t TxData[8];
uint16_t Data[10];

// #### END RS485 ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### LINE SENSOR VARIABLES ####

#define line_sensor_front_total_channel 10
#define line_sensor_back_total_channel 10

#define max_rs485_speed 255

int line_sensor_front_read_line_old_value;
int line_sensor_back_read_line_old_value;

volatile uint16_t line_sensor_front_values_dma[line_sensor_front_total_channel];
volatile uint16_t line_sensor_back_values_dma[line_sensor_back_total_channel];

volatile uint16_t line_sensor_front_values_calibrated[line_sensor_front_total_channel];
volatile uint16_t line_sensor_back_values_calibrated[line_sensor_back_total_channel];

volatile uint8_t line_sensor_front_on_line_left_number, line_sensor_front_on_line_middle_number, line_sensor_front_on_line_right_number;
volatile uint8_t line_sensor_front_on_line_total_number;

volatile uint8_t line_sensor_back_on_line_left_number, line_sensor_back_on_line_middle_number, line_sensor_back_on_line_right_number;
volatile uint8_t line_sensor_back_on_line_total_number;


uint16_t line_sensor_front_max_sensor_vales[line_sensor_front_total_channel] = {4095, 4095, 4095, 4095, 4095, 4095, 4095, 3388, 4095, 4095};
uint16_t line_sensor_front_min_sensor_vales[line_sensor_front_total_channel] = {1700, 1800, 1300, 1000, 2300, 1400, 1100, 900, 1300, 2800};

uint16_t line_sensor_back_max_sensor_vales[line_sensor_back_total_channel] = {4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095};
uint16_t line_sensor_back_min_sensor_vales[line_sensor_back_total_channel] = {1400, 1300, 1200, 1000, 1500, 1900, 1800, 1500, 1500, 1500};


const uint8_t line_sensor_front_channel_number = sizeof(line_sensor_front_values_dma)/sizeof(line_sensor_front_values_dma[0]);
const uint8_t line_sensor_back_channel_number = sizeof(line_sensor_back_values_dma)/sizeof(line_sensor_back_values_dma[0]);

volatile uint16_t line_sensor_front_read_line_value;
volatile uint16_t line_sensor_back_read_line_value;

//uint8_t line_gap_disable = 0;

const uint16_t line_sensor_front_threshold = 500;
const uint16_t line_sensor_back_threshold = 500;

ir_array front_array;
ir_array back_array;


// #### END LINE SENSOR VARIABLES ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### AGV ORIENTATION VARIABLES ####

uint16_t agv_orientation = 0xF00F;


// #### END AGV ORIENTATION VARIABLES ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### DiSPLAY DATA AND BUTTON VARIABLES ####

uint8_t Tx_Data_Uart1[5];
uint8_t Rx_Data_Uart1[5];



// #### END DiSPLAY DATA AND BUTTON VARIABLES ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### PID and MOTOR SPEED VARIABLES ####

volatile float P, I, D;

float Kp = 0.025;
float Ki = 0;
float Kd = 0.25;

volatile float pid_last_error;
volatile float pid_error;
volatile float pid_motor_speed_change;

volatile uint16_t pid_motor_speed_A;
volatile uint16_t pid_motor_speed_B;
uint16_t pid_motor_base_speed = 200;


uint8_t agv_turn_count = 0;

uint8_t debug_oled[20];
// #### END PID and MOTOR SPEED VARIABLES ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### Task Completion Variables ####

uint16_t On_Task = 0xF00F;  // not on task = 0xF00F || on task = 0xF11F

uint8_t Station = 0;

uint8_t Current_Station = 0;
uint8_t Target_Station = 0;

uint8_t on_task_decisions[5] = {'L', 'L', 'L', 'L', 'L'};

uint16_t pid_motor_orientation;

// #### END Task Completion Variables ####

// -o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-

// #### LORA ####

LoRa myLoRa;

uint16_t LoRa_stat = 0;

uint8_t LoraTxBuffer[128];
uint8_t LoraRxBuffer[128];

uint8_t lora_receive_toggle = 0;

// #### END LORA ####


//uint8_t uid[MIFARE_UID_MAX_LENGTH];
//int32_t uid_len = 0;
//uint8_t uid_version[10];
//PN532 pn532;
//uint8_t Station_id[5];

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?
uint8_t Distance  = 0;

//#define TRIG_PIN GPIO_PIN_8
//#define TRIG_PORT GPIOE


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//void Line_Sensor_Calculation(volatile uint16_t *sensor_values,
//						 	  volatile uint16_t *sensor_calibrated_values,
//							      	   uint16_t *sensor_max_values,
//									   uint16_t *sensor_min_values,
//
//							  volatile uint8_t  *sensor_middle_on_line_number,
//
//							  volatile uint8_t  *sensor_total_on_line_number,
//									   uint16_t  sensor_threshhold,
//									   uint8_t   sensor_numbers,
//							  volatile uint16_t *line_position){
//	uint8_t i, on_line = 0;
//	uint32_t avg = 0;
//	uint32_t sum = 0;
//	uint16_t value;
//	uint8_t on_sensor_total_number = 0;
//
//	uint8_t middle_on_line = 0;
//
//
//	for(i = 0; i < sensor_numbers; i++){
//		uint16_t calmin, calmax;
//		uint16_t denominator;
//		calmax = sensor_max_values[i];
//		calmin = sensor_min_values[i];
//
//		denominator = calmax - calmin;
//
//		int x = 0;
//		if(denominator != 0){
//			x = (((signed long)sensor_values[i]) - calmin) * 1000/denominator;
//		}
//		if(x <0){
//			x = 0;
//		}
//		if(x>1000){
//			x = 1000;
//		}
//		value = (1000-x);
//		sensor_calibrated_values[i] = value;
//
//		// start read line number section
//		if(value > 600){
//			on_line = 1;
//		}
//		if(value > 200){
//			avg += (long)(value)*(i*1000);
//			sum += value;
//		}
//		// end read line number section
//
//		// start on line sensor calculation
//		if(value > sensor_threshhold){
//			on_sensor_total_number++;
//			if(i >= 2 && i <= 7){
//				middle_on_line++;
//			}
//		}
//		// end on line sensor calculation
//	}
//
//	// start read line number section
//	if(!on_line){
//		if(_line_read_value < (sensor_numbers - 1) * 1000/2){
//			_line_read_value = 0;
//		}
//		else{
//			_line_read_value = (sensor_numbers - 1)*1000;
//		}
//	}
//	else{
//		_line_read_value = avg/sum;
//	}
//	*line_position = _line_read_value;
//	// end read line number section
//
//	// start on line sensor calculation
//
//	*sensor_middle_on_line_number = middle_on_line;
//
//	*sensor_total_on_line_number = on_sensor_total_number;
//	// end on line sensor calculation
//	// 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 - 8 - 9
//
//	// 2 - 7 --> mid
//
//}

void PID_control(volatile uint16_t *line_position,
				          uint16_t *motor_orientation){

	pid_error = 4500 - *line_position;

	P = pid_error;
	//I = error + I;
	I = 0;
	D = pid_error - pid_last_error;
	pid_last_error = pid_error;

	pid_motor_speed_change = P*Kp + I*Ki + D*Kd;

	if(*motor_orientation == 0xF11F){
		pid_motor_speed_A = pid_motor_base_speed + pid_motor_speed_change;
		pid_motor_speed_B = pid_motor_base_speed - pid_motor_speed_change;
	}
	if(*motor_orientation == 0xF00F){
		pid_motor_speed_A = pid_motor_base_speed - pid_motor_speed_change;
		pid_motor_speed_B = pid_motor_base_speed + pid_motor_speed_change;
	}


	if(pid_motor_speed_A > max_rs485_speed){
		pid_motor_speed_A = max_rs485_speed;
	}
	if(pid_motor_speed_A < 0){
		pid_motor_speed_A = 0;
	}
	if(pid_motor_speed_B > max_rs485_speed){
		pid_motor_speed_B = max_rs485_speed;
	}
	if(pid_motor_speed_B < 0){
		pid_motor_speed_B = 0;
	}

}

void PID_control_line_gap(volatile uint16_t *line_position){

	pid_error = 4500 - *line_position;

	P = pid_error;
	//I = error + I;
	I = 0;
	D = pid_error - pid_last_error;
	pid_last_error = pid_error;

	pid_motor_speed_change = P*Kp + I*Ki + D*Kd;

	pid_motor_speed_A = pid_motor_base_speed + pid_motor_speed_change;
	pid_motor_speed_B = pid_motor_base_speed - pid_motor_speed_change;


	if(pid_motor_speed_A > max_rs485_speed){
		pid_motor_speed_A = max_rs485_speed;
	}
	if(pid_motor_speed_A < 0){
		pid_motor_speed_A = 0;
	}
	if(pid_motor_speed_B > max_rs485_speed){
		pid_motor_speed_B = max_rs485_speed;
	}
	if(pid_motor_speed_B < 0){
		pid_motor_speed_B = 0;
	}

}

void PID_Forward_Rotation(uint16_t enableA, uint16_t enableB, uint16_t *orientation){

	if(*orientation == 0xF11F){
		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, enableA);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, enableB);

		  set_speed(0x01, enableB, 0);
		  HAL_Delay(10);
		  set_speed(0x02, enableA, 1);
		  HAL_Delay(10);
	}

	if(*orientation == 0xF00F){

		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, enableA);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, enableB);

		  set_speed(0x01, enableB, 1);
		  HAL_Delay(10);
		  set_speed(0x02, enableA, 0);
		  HAL_Delay(10);

	}



}

void PID_Motor_Turn_Left(uint16_t _speed, uint16_t *orientation){
	if(*orientation == 0xF11F){
		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, _speed);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, _speed);

		  set_speed(0x01, _speed, 0);
		  HAL_Delay(10);
		  set_speed(0x02, _speed, 0);
		  HAL_Delay(10);
	}

	if(*orientation == 0xF00F){

		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, _speed);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, _speed);

		  set_speed(0x01, _speed, 0);
		  HAL_Delay(10);
		  set_speed(0x02, _speed, 0);
		  HAL_Delay(10);

	}
}

void PID_Motor_Turn_Right(uint16_t _speed, uint16_t *orientation){
	if(*orientation == 0xF11F){
		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, _speed);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, _speed);

		  set_speed(0x01, _speed, 1);
		  HAL_Delay(10);
		  set_speed(0x02, _speed, 1);
		  HAL_Delay(10);
	}

	if(*orientation == 0xF00F){

		//LEFT
		HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);

		//RIGHT
		HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_SET);

		//Right
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, _speed);

		//Left
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, _speed);

		  set_speed(0x01, _speed, 1);
		  HAL_Delay(10);
		  set_speed(0x02, _speed, 1);
		  HAL_Delay(10);

	}

}


void PID_Motor_All_Break(){
	//LEFT
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);

	//RIGHT
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, GPIO_PIN_RESET);

	//Right
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

	//Left
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

	  motor_enable_velocity_mode(0x01);
	  HAL_Delay(10);
	  motor_enable_velocity_mode(0x02);
	  HAL_Delay(10);
	  set_speed(0x01, 0, 1);
	  HAL_Delay(10);
	  set_speed(0x02, 0, 0);
	  HAL_Delay(10);

}

void AGV_waiting(){
	if(Target_Station == Current_Station){
		On_Task = 0xF00F;
		if(Target_Station != Station){
			Target_Station = Station;
			if(Current_Station == 0){
				if(Target_Station == 1){
					on_task_decisions[0] = 'L';
					on_task_decisions[1] = 'L';
					on_task_decisions[2] = 'E';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
				if(Target_Station == 2){
					on_task_decisions[0] = 'L';
					on_task_decisions[1] = 'F';
					on_task_decisions[2] = 'L';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
			}

			if(Current_Station == 1){
				if(Target_Station == 0){
					on_task_decisions[0] = 'R';
					on_task_decisions[1] = 'R';
					on_task_decisions[2] = 'E';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
				if(Target_Station == 2){
					on_task_decisions[0] = 'L';
					on_task_decisions[1] = 'L';
					on_task_decisions[2] = 'E';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
			}

			if(Current_Station == 2){
				if(Target_Station == 0){
					on_task_decisions[0] = 'R';
					on_task_decisions[1] = 'F';
					on_task_decisions[2] = 'R';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
				if(Target_Station == 1){
					on_task_decisions[0] = 'R';
					on_task_decisions[1] = 'R';
					on_task_decisions[2] = 'E';
					on_task_decisions[3] = 'E';
					on_task_decisions[4] = 'E';
				}
			}

		}
	}
	if(Target_Station != Current_Station){
		On_Task = 0xF11F;
	}
}

//void AGV_Turn_Detection_Completion(volatile uint16_t *sensor_calibrated_values,
//								   volatile uint8_t  *sensor_middle_on_line_number,
//								   volatile uint8_t  *sensor_total_on_line_number,
//								   	   	   	uint8_t  *decision_array,
//											uint16_t *orientation){
//#define white_detection_thresh_hold 500
//#define black_detection_thresh_hold 500
//#define sensor_mid_on_line_thresh_hold 1
//#define first_timer_buffer 800
//#define second_timer_buffer 300
//#define skip_turn_timer_buffer 200
//#define base_speed 180
//
//	uint8_t _turn_decide = 0;
//
//	if((((sensor_calibrated_values[8] > black_detection_thresh_hold) && (sensor_calibrated_values[9] > black_detection_thresh_hold)) ||
//	   ((sensor_calibrated_values[0] > black_detection_thresh_hold) && (sensor_calibrated_values[1] > black_detection_thresh_hold))) && (*sensor_total_on_line_number >= 8)){
////		if((*sensor_middle_on_line_number >= 5) || (*sensor_middle_on_line_number == 6)){
//
//
//			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_7);
//
////			PID_Forward_Rotation(base_speed, base_speed, orientation);
////			HAL_Delay(100);
//
//			agv_turn_count += 1;
//
//			PID_Motor_All_Break();
//
//			HAL_Delay(2000);
//
//			if(decision_array[agv_turn_count - 1] == 'E'){
//
////
////
////
////								  Line_Sensor_Calculation(line_sensor_front_values_dma,
////														  line_sensor_front_values_calibrated,
////														  line_sensor_front_max_sensor_vales,
////														  line_sensor_front_min_sensor_vales,
////														 &line_sensor_front_on_line_middle_number,
////														 &line_sensor_front_on_line_total_number,
////														  line_sensor_front_trigger_threshhold,
////														  line_sensor_front_total_channel,
////														 &line_sensor_front_read_line_value);
////
////								  Line_Sensor_Calculation(line_sensor_back_values_dma,
////														  line_sensor_back_values_calibrated,
////														  line_sensor_back_max_sensor_vales,
////														  line_sensor_back_min_sensor_vales,
////														 &line_sensor_back_on_line_middle_number,
////														 &line_sensor_back_on_line_total_number,
////														  line_sensor_back_trigger_threshhold,
////														  line_sensor_back_total_channel,
////														 &line_sensor_back_read_line_value);
//
////								  if(line_sensor_back_on_line_total_number >= 9){
////									  agv_orientation = 0xF00F;
////								  }
////								  if(line_sensor_front_on_line_total_number >= 9){
////									  agv_orientation = 0xF11F;
////								  }
//
//
//								PID_Motor_All_Break();
//
//
//
//								Current_Station = Target_Station;
//
//								On_Task = 0xF00F;
//
//								if(agv_orientation == 0xF00F){
//									agv_orientation = 0xF11F;
//								}
//								else if(agv_orientation == 0xF11F){
//									agv_orientation = 0xF00F;
//								}
//
//								_turn_decide = 0;
//								agv_turn_count = 0;
//
//
//			}
//			else if(decision_array[agv_turn_count - 1] != 'E'){
//
//				//line_gap_disable = 1;
//
//				PID_Forward_Rotation(base_speed, base_speed, orientation);
//				HAL_Delay(first_timer_buffer);
//				PID_Motor_All_Break();
//
//				HAL_Delay(1000);
//
//				if(decision_array[agv_turn_count - 1] == 'R'){
//
//					PID_Motor_Turn_Right(base_speed, orientation);
//					HAL_Delay(second_timer_buffer);
//					PID_Motor_All_Break();
//
//					HAL_Delay(1000);
//
//					_turn_decide = 'R';
//				}
//				if(decision_array[agv_turn_count - 1] == 'L'){
//
//					PID_Motor_Turn_Left(base_speed, orientation);
//					HAL_Delay(second_timer_buffer);
//					PID_Motor_All_Break();
//
//					HAL_Delay(1000);
//
//					_turn_decide = 'L';
//				}
//				if(decision_array[agv_turn_count - 1] == 'F'){
//
//					PID_Forward_Rotation(base_speed, base_speed, orientation);
//					HAL_Delay(skip_turn_timer_buffer);
//					_turn_decide = 0;
//				}
//			}
//
//	}
//
//	if((sensor_calibrated_values[8] < white_detection_thresh_hold) && (sensor_calibrated_values[9] < white_detection_thresh_hold) &&
//			(sensor_calibrated_values[0] > black_detection_thresh_hold) && (sensor_calibrated_values[1] > black_detection_thresh_hold)){
//			if((*sensor_middle_on_line_number >= 3) && (*sensor_middle_on_line_number < 6)){
//
//				//line_gap_disable = 1;
//
//			PID_Forward_Rotation(base_speed, base_speed, orientation);
//			HAL_Delay(first_timer_buffer);
//			PID_Motor_All_Break();
//
//			HAL_Delay(1000);
//
//			agv_turn_count += 1;
//
//			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_7);
//
//			if(decision_array[agv_turn_count - 1] == 'L'){
//
//				PID_Motor_Turn_Left(base_speed, orientation);
//				HAL_Delay(second_timer_buffer);
//				PID_Motor_All_Break();
//
//				HAL_Delay(1000);
//
//				_turn_decide = 'L';
//			}
//			if(decision_array[agv_turn_count - 1] == 'F'){
//
//				PID_Forward_Rotation(base_speed, base_speed, orientation);
//				HAL_Delay(skip_turn_timer_buffer);
//				_turn_decide = 0;
//			}
//
//		}
//
//	}
//	if((sensor_calibrated_values[8] > black_detection_thresh_hold) && (sensor_calibrated_values[9] > black_detection_thresh_hold) &&
//			(sensor_calibrated_values[0] < white_detection_thresh_hold) && (sensor_calibrated_values[1] < white_detection_thresh_hold)){
//			if((*sensor_middle_on_line_number >= 3) && (*sensor_middle_on_line_number < 6)){
//
//				//line_gap_disable = 1;
//
//			PID_Forward_Rotation(base_speed, base_speed, orientation);
//			HAL_Delay(first_timer_buffer);
//			PID_Motor_All_Break();
//
//			HAL_Delay(1000);
//
//			agv_turn_count += 1;
//
//			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_7);
//
//			if(decision_array[agv_turn_count - 1] == 'R'){
//
//				PID_Motor_Turn_Right(base_speed, orientation);
//				HAL_Delay(second_timer_buffer);
//				PID_Motor_All_Break();
//
//				HAL_Delay(1000);
//
//				_turn_decide = 'R';
//			}
//			if(decision_array[agv_turn_count - 1] == 'F'){
//
//				PID_Forward_Rotation(base_speed, base_speed, orientation);
//				HAL_Delay(skip_turn_timer_buffer);
//				_turn_decide = 0;
//			}
//		}
//
//	}
//
//
//
//
//	if(_turn_decide == 'L'){
//		while((sensor_calibrated_values[5] < white_detection_thresh_hold) || (sensor_calibrated_values[6] < white_detection_thresh_hold)){
//			  Line_Sensor_Calculation(line_sensor_front_values_dma,
//									  line_sensor_front_values_calibrated,
//									  line_sensor_front_max_sensor_vales,
//									  line_sensor_front_min_sensor_vales,
//									 &line_sensor_front_on_line_middle_number,
//									 &line_sensor_front_on_line_total_number,
//									  line_sensor_front_trigger_threshhold,
//									  line_sensor_front_total_channel,
//									 &line_sensor_front_read_line_value);
//
//			  Line_Sensor_Calculation(line_sensor_back_values_dma,
//									  line_sensor_back_values_calibrated,
//									  line_sensor_back_max_sensor_vales,
//									  line_sensor_back_min_sensor_vales,
//									 &line_sensor_back_on_line_middle_number,
//									 &line_sensor_back_on_line_total_number,
//									  line_sensor_back_trigger_threshhold,
//									  line_sensor_back_total_channel,
//									 &line_sensor_back_read_line_value);
//
//			PID_Motor_Turn_Left(base_speed, orientation);
//		}
//		PID_Motor_All_Break();
//		_turn_decide = 0;
//
//
//		//line_gap_disable = 0;
//
////		else if((sensor_calibrated_values[7] > black_detection_thresh_hold) || (sensor_calibrated_values[8] > black_detection_thresh_hold)){
////
////
////		}
//
//	}
//	if(_turn_decide == 'R'){
//		while((sensor_calibrated_values[6] < white_detection_thresh_hold) || (sensor_calibrated_values[5] < white_detection_thresh_hold)){
//			  Line_Sensor_Calculation(line_sensor_front_values_dma,
//									  line_sensor_front_values_calibrated,
//									  line_sensor_front_max_sensor_vales,
//									  line_sensor_front_min_sensor_vales,
//									 &line_sensor_front_on_line_middle_number,
//									 &line_sensor_front_on_line_total_number,
//									  line_sensor_front_trigger_threshhold,
//									  line_sensor_front_total_channel,
//									 &line_sensor_front_read_line_value);
//
//			  Line_Sensor_Calculation(line_sensor_back_values_dma,
//									  line_sensor_back_values_calibrated,
//									  line_sensor_back_max_sensor_vales,
//									  line_sensor_back_min_sensor_vales,
//									 &line_sensor_back_on_line_middle_number,
//									 &line_sensor_back_on_line_total_number,
//									  line_sensor_back_trigger_threshhold,
//									  line_sensor_back_total_channel,
//									 &line_sensor_back_read_line_value);
//
//			PID_Motor_Turn_Right(base_speed, orientation);
//		}
//		PID_Motor_All_Break();
//		_turn_decide = 0;
//
//		//line_gap_disable = 0;
////		else if((sensor_calibrated_values[4] > black_detection_thresh_hold) || (sensor_calibrated_values[3] > black_detection_thresh_hold)){
////
////		}
//	}
//
//
//}



//void Startup_RFID(){
//
//	PN532_SPI_Init(&pn532);
//	//HAL_GPIO_WritePin(RFID_SSF_GPIO_Port, RFID_SSF_Pin, 0);
//
//	if(PN532_GetFirmwareVersion(&pn532, uid_version) != PN532_STATUS_OK)
//	{
//		while(1){
//
//		}
//	}
//
//	PN532_SamConfiguration(&pn532);
//
//}
//void AGV_RFID_Detection(){
//	uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 100);
//
//	if(uid_len >= 4){
//		Station_id[0] = uid[0];
//		Station_id[1] = uid[1];
//		Station_id[2] = uid[2];
//		Station_id[3] = uid[3];
//		Station_id[4] = uid[4];
//	}
//}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_0){
		Station = 0;
	}

	if(GPIO_Pin == GPIO_PIN_1){
		Station = 1;
	}

	if(GPIO_Pin == GPIO_PIN_2){
		Station = 2;
	}

	if(GPIO_Pin == GPIO_PIN_3){
		Station = 3;
	}

	if(GPIO_Pin == GPIO_PIN_4){
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_7);
	}

	if(GPIO_Pin == myLoRa.DIO0_pin){
		LoRa_receive(&myLoRa, LoraRxBuffer, 128);
		HAL_GPIO_TogglePin(LORA_RX_LED_GPIO_Port, LORA_RX_LED_Pin);
		lora_receive_toggle = 255;

	}
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	Data[0] = RxData[3]<<8 | RxData[4];
	Data[1] = RxData[5]<<8 | RxData[6];
	Data[2] = RxData[7]<<8 | RxData[8];
	Data[3] = RxData[9]<<8 | RxData[10];
	Data[4] = RxData[11]<<8 | RxData[12];
}

void sendData (uint8_t *data)
{
	HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, data, 8, 1000);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port,TX_EN_Pin , GPIO_PIN_RESET);
}

void motor_enable_velocity_mode(uint8_t id){
	  TxData[0] = id;  // slave address
	  TxData[1] = 0x06;  // Function code for Read Holding Registers

	  TxData[2] = 0x20;
	  TxData[3] = 0x32;
	  //The Register address will be 00000000 00000100 = 4 + 40001 = 40005

	  TxData[4] = 0x00;
	  TxData[5] = 0x03;
	  // no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	  uint16_t crc = crc16(TxData, 6);
	  TxData[6] = crc&0xFF;   // CRC LOW
	  TxData[7] = (crc>>8)&0xFF;  // CRC HIGH
	  sendData(TxData);

	  HAL_Delay(10);

	  TxData[0] = id;  // slave address
	  TxData[1] = 0x06;  // Function code for Read Holding Registers

	  TxData[2] = 0x20;
	  TxData[3] = 0x31;
	  //The Register address will be 00000000 00000100 = 4 + 40001 = 40005

	  TxData[4] = 0x00;
	  TxData[5] = 0x08;
	  // no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	  crc = crc16(TxData, 6);
	  TxData[6] = crc&0xFF;   // CRC LOW
	  TxData[7] = (crc>>8)&0xFF;  // CRC HIGH

	  sendData(TxData);
	  HAL_Delay(10);
}

void set_speed(uint8_t id, uint16_t speed, uint8_t dir){

	TxData[0] = id;  // slave address
	TxData[1] = 0x06;  // Function code for Read Holding Registers

	TxData[2] = 0x20;
	TxData[3] = 0x3A;
	//The Register address will be 00000000 00000100 = 4 + 40001 = 40005
	if(dir == 0){
		TxData[4] = (speed>>8)&0xFF;
		TxData[5] = speed&0xFF;
	}
	if(dir == 1){
		speed = (~speed) + 1;
		TxData[4] = (speed>>8)&0xFF;
		TxData[5] = speed&0xFF;
	}

	// no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	uint16_t crc = crc16(TxData, 6);
	TxData[6] = crc&0xFF;   // CRC LOW
	TxData[7] = (crc>>8)&0xFF;  // CRC HIGH

	sendData(TxData);

	HAL_Delay(10);
}



int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */



  HAL_UARTEx_ReceiveToIdle_IT(&huart1, RxData, 32);



  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  motor_enable_velocity_mode(0x01);
  HAL_Delay(10);
  motor_enable_velocity_mode(0x02);
  HAL_Delay(10);


//  Line_Sensor_Calculation(line_sensor_front_values_dma,
//						  line_sensor_front_values_calibrated,
//						  line_sensor_front_max_sensor_vales,
//						  line_sensor_front_min_sensor_vales,
//						 &line_sensor_front_on_line_middle_number,
//						 &line_sensor_front_on_line_total_number,
//						  line_sensor_front_trigger_threshhold,
//						  line_sensor_front_total_channel,
//						 &line_sensor_front_read_line_value);
//
//  Line_Sensor_Calculation(line_sensor_back_values_dma,
//						  line_sensor_back_values_calibrated,
//						  line_sensor_back_max_sensor_vales,
//						  line_sensor_back_min_sensor_vales,
//						 &line_sensor_back_on_line_middle_number,
//						 &line_sensor_back_on_line_total_number,
//						  line_sensor_back_trigger_threshhold,
//						  line_sensor_back_total_channel,
//						 &line_sensor_back_read_line_value);

  if((line_sensor_back_on_line_total_number >= 9) &&(line_sensor_front_on_line_total_number >= 1)){
	  agv_orientation = 0xF00F;
  }
  if((line_sensor_front_on_line_total_number >= 9) && (line_sensor_back_on_line_total_number >= 1)){
	  agv_orientation = 0xF11F;
  }

  myLoRa = newLoRa();

  myLoRa.CS_port         = LORA_NSS_GPIO_Port;
  myLoRa.CS_pin          = LORA_NSS_Pin;
  myLoRa.reset_port      = LORA_RST_GPIO_Port;
  myLoRa.reset_pin       = LORA_RST_Pin;
  myLoRa.DIO0_port       = LORA_DIO0_GPIO_Port;
  myLoRa.DIO0_pin        = LORA_DIO0_Pin;
  myLoRa.hSPIx           = &hspi1;

  myLoRa.frequency             = 433;             // default = 433 MHz
  myLoRa.spredingFactor        = SF_7;            // default = SF_7
  myLoRa.bandWidth             = BW_125KHz;       // default = BW_125KHz
  myLoRa.crcRate               = CR_4_5;          // default = CR_4_5
  myLoRa.power                 = POWER_20db;      // default = 20db
  myLoRa.overCurrentProtection = 100;             // default = 100 mA
  myLoRa.preamble              = 8;              // default = 8;

  if(LoRa_init(&myLoRa) == LORA_OK){
	  LoRa_stat = 1;
  }

  LoRa_startReceiving(&myLoRa);

  LoraTxBuffer[0] = 0xAA;
  LoraTxBuffer[1] = 0xBB;
  LoraTxBuffer[2] = 0xCC;

  //LoRa_transmit(&myLoRa, LoraTxBuffer, 3, 500);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) front_array.ir_sen_val_dma, line_sensor_front_channel_number);
  ir_array_init(&front_array,
                line_sensor_front_values_calibrated,
                line_sensor_front_max_sensor_vales,
                line_sensor_front_min_sensor_vales,
                &line_sensor_front_channel_number,
                &line_sensor_front_read_line_value,
                &line_sensor_front_read_line_old_value,
                &line_sensor_front_on_line_total_number,
                &line_sensor_front_on_line_middle_number,
                &line_sensor_front_on_line_left_number,
                &line_sensor_front_on_line_right_number,
                &line_sensor_front_threshold);
  Line_Sensor_Calculation(&front_array);


  HAL_ADC_Start_DMA(&hadc3, (uint32_t*) back_array.ir_sen_val_dma, line_sensor_back_channel_number);
  ir_array_init(&back_array,
                line_sensor_back_values_calibrated,
                line_sensor_back_max_sensor_vales,
                line_sensor_back_min_sensor_vales,
                &line_sensor_back_channel_number,
                &line_sensor_back_read_line_value,
                &line_sensor_back_read_line_old_value,
                &line_sensor_back_on_line_total_number,
                &line_sensor_back_on_line_middle_number,
                &line_sensor_back_on_line_left_number,
                &line_sensor_back_on_line_right_number,
                &line_sensor_back_threshold);
  Line_Sensor_Calculation(&back_array);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
//	  agv_orientation = 0xF00F;
//
//	  if(lora_receive_toggle == 255){
//
//		  if(LoRa_transmit(&myLoRa, LoraTxBuffer, 3, 500) == 1){
//			  //lora_receive_toggle = 0;
//			  HAL_GPIO_TogglePin(LORA_TX_LED_GPIO_Port, LORA_TX_LED_Pin);
//		  }
//		  lora_receive_toggle = 0;
//	  }

	  Line_Sensor_Calculation(&front_array);
	  Line_Sensor_Calculation(&back_array);

//	  Line_Sensor_Calculation(line_sensor_front_values_dma,
//							  line_sensor_front_values_calibrated,
//							  line_sensor_front_max_sensor_vales,
//							  line_sensor_front_min_sensor_vales,
//							 &line_sensor_front_on_line_middle_number,
//							 &line_sensor_front_on_line_total_number,
//							  line_sensor_front_trigger_threshhold,
//							  line_sensor_front_total_channel,
//							 &line_sensor_front_read_line_value);
//
//	  Line_Sensor_Calculation(line_sensor_back_values_dma,
//							  line_sensor_back_values_calibrated,
//							  line_sensor_back_max_sensor_vales,
//							  line_sensor_back_min_sensor_vales,
//							 &line_sensor_back_on_line_middle_number,
//							 &line_sensor_back_on_line_total_number,
//							  line_sensor_back_trigger_threshhold,
//							  line_sensor_back_total_channel,
//							 &line_sensor_back_read_line_value);

//	  PID_control(&line_sensor_front_read_line_value, &agv_orientation);
//
//	  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
//
//	  if(line_sensor_back_total_channel >= 1){
//
//			  PID_control(&line_sensor_back_read_line_value, &agv_orientation);
//
//			  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
//
//	  }
//	  if(line_sensor_back_total_channel == 0){
//
//		  PID_control(&line_sensor_front_read_line_value, &agv_orientation);
//
//		  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
//	  }

//
	  //PID_Forward_Rotation(10, 50, &agv_orientation);
	  //agv_orientation = 0xF11F;

//	  motor_enable_velocity_mode(0x01);
//	  HAL_Delay(10);
//	  motor_enable_velocity_mode(0x02);
//	  HAL_Delay(10);
//
//	  set_speed(0x01, 10, 1);
//	  HAL_Delay(10);
//	  set_speed(0x02, 50, 0);
//	  HAL_Delay(10);

//	  motor_enable_velocity_mode(0x01);
//	  HAL_Delay(10);
//	  motor_enable_velocity_mode(0x02);
//	  HAL_Delay(10);
//	  HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_7);
//	  agv_orientation = 0xF00F;
//	  PID_Forward_Rotation(50, 10, &agv_orientation);
//	  HAL_Delay(2000);
//	  PID_Motor_Turn_Left(10, &agv_orientation);
//	  HAL_Delay(2000);
//	  PID_Motor_Turn_Right(10, &agv_orientation);
//	  HAL_Delay(2000);
//	  PID_Motor_All_Break();
//	  HAL_Delay(2000);



//	  	  if(On_Task == 0xF11F){
//
////	  		  Line_Sensor_Calculation(line_sensor_front_values_dma,
////	  								  line_sensor_front_values_calibrated,
////	  								  line_sensor_front_max_sensor_vales,
////	  								  line_sensor_front_min_sensor_vales,
////	  								 &line_sensor_front_on_line_middle_number,
////	  								 &line_sensor_front_on_line_total_number,
////	  								  line_sensor_front_trigger_threshhold,
////	  								  line_sensor_front_total_channel,
////	  								 &line_sensor_front_read_line_value);
////
////
////	  		  Line_Sensor_Calculation(line_sensor_back_values_dma,
////	  								  line_sensor_back_values_calibrated,
////	  								  line_sensor_back_max_sensor_vales,
////	  								  line_sensor_back_min_sensor_vales,
////	  								 &line_sensor_back_on_line_middle_number,
////	  								 &line_sensor_back_on_line_total_number,
////	  								  line_sensor_back_trigger_threshhold,
////	  								  line_sensor_back_total_channel,
////	  								 &line_sensor_back_read_line_value);
//
//
//
//	  		  if(agv_orientation == 0xF00F){
//
//		  		  Line_Sensor_Calculation(line_sensor_front_values_dma,
//		  								  line_sensor_front_values_calibrated,
//		  								  line_sensor_front_max_sensor_vales,
//		  								  line_sensor_front_min_sensor_vales,
//		  								 &line_sensor_front_on_line_middle_number,
//		  								 &line_sensor_front_on_line_total_number,
//		  								  line_sensor_front_trigger_threshhold,
//		  								  line_sensor_front_total_channel,
//		  								 &line_sensor_front_read_line_value);
//
//				  AGV_Turn_Detection_Completion(line_sensor_front_values_calibrated,
//											   &line_sensor_front_on_line_middle_number,
//											   &line_sensor_front_on_line_total_number,
//												on_task_decisions,
//											   &agv_orientation);
//
//				  PID_control(&line_sensor_front_read_line_value, &agv_orientation);
//
//				  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
//
//
////		  		  if((line_sensor_front_total_channel == 0)&&(line_gap_disable == 0)){
////
////
////					  PID_control(&line_sensor_back_read_line_value, &agv_orientation);
////
////					  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
////		  		  }
//
//
//
//
//
//	  		  }
//	  		  if(agv_orientation == 0xF11F){
//
//		  		  Line_Sensor_Calculation(line_sensor_back_values_dma,
//		  								  line_sensor_back_values_calibrated,
//		  								  line_sensor_back_max_sensor_vales,
//		  								  line_sensor_back_min_sensor_vales,
//		  								 &line_sensor_back_on_line_middle_number,
//		  								 &line_sensor_back_on_line_total_number,
//		  								  line_sensor_back_trigger_threshhold,
//		  								  line_sensor_back_total_channel,
//		  								 &line_sensor_back_read_line_value);
//
//				  AGV_Turn_Detection_Completion(line_sensor_back_values_calibrated,
//											   &line_sensor_back_on_line_middle_number,
//											   &line_sensor_back_on_line_total_number,
//												on_task_decisions,
//											   &agv_orientation);
//
//				  PID_control(&line_sensor_back_read_line_value, &agv_orientation);
//
//				  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
//
//
////		  		  if((line_sensor_back_total_channel == 0) && (line_gap_disable == 0)){
////
////
////					  PID_control(&line_sensor_front_read_line_value, &agv_orientation);
////
////					  PID_Forward_Rotation(pid_motor_speed_A, pid_motor_speed_B, &agv_orientation);
////
////
////		  		  }
//
//
//
//
//
//	  		  }
//
//	  	  }
//	  	  if(On_Task == 0xF00F){
//	  		  AGV_waiting();
//	  		  PID_Motor_All_Break();
//	  	  }




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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 44;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 15;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 2950;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 10;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.DataAlign = ADC3_DATAALIGN_RIGHT;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 10;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  hadc3.Init.Oversampling.Ratio = ADC3_OVERSAMPLING_RATIO_2;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC3_SAMPLETIME_92CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

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
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 64;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 255;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, IN3_Pin|IN4_Pin|IN2_Pin|IN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7|LORA_TX_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LORA_RX_LED_GPIO_Port, LORA_RX_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LORA_NSS_Pin|LORA_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE0
                           PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_0
                          |GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IN3_Pin IN4_Pin IN2_Pin IN1_Pin */
  GPIO_InitStruct.Pin = IN3_Pin|IN4_Pin|IN2_Pin|IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : TRIG_Pin */
  GPIO_InitStruct.Pin = TRIG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TRIG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TX_EN_Pin */
  GPIO_InitStruct.Pin = TX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TX_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PG7 LORA_NSS_Pin LORA_RST_Pin LORA_TX_LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_7|LORA_NSS_Pin|LORA_RST_Pin|LORA_TX_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : LORA_RX_LED_Pin */
  GPIO_InitStruct.Pin = LORA_RX_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LORA_RX_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LORA_DIO0_Pin */
  GPIO_InitStruct.Pin = LORA_DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LORA_DIO0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
