/*
 * rs485_driver.c
 *
 *  Created on: Aug 27, 2024
 *      Author: ASUS
 */


#include "rs485_driver.h"

void rs485_init(rs485_driver *driver){
	HAL_TIM_Base_Start_IT(driver->rs485_timer);
	HAL_UARTEx_ReceiveToIdle_IT(driver->rs485_huart, driver->rs485_RxData, 16);
}

void rs485_send_data(rs485_driver *driver, uint8_t *data){
	if(driver->rs485_isbusy == 0){
		HAL_GPIO_WritePin(driver->rs485_enable_port, driver->rs485_enable_pin, GPIO_PIN_SET);
		HAL_UART_Transmit(driver->rs485_huart, data, 8, 500);
		HAL_GPIO_WritePin(driver->rs485_enable_port, driver->rs485_enable_pin, GPIO_PIN_RESET);
		driver->rs485_isbusy = 255;
	}
	driver->rs485_last_connectivity = driver->rs485_connectivity;
	HAL_GPIO_WritePin(driver->rs485_rx_led_port, driver->rs485_rx_led_pin, GPIO_PIN_RESET);
	//HAL_UARTEx_ReceiveToIdle_IT(driver->rs485_huart, driver->rs485_RxData, 16);
}

void rs485_set_speed(rs485_driver *driver, uint16_t speed, uint8_t dir){
	driver->rs485_TxData[0] = driver->rs485_id;
	driver->rs485_TxData[1] = 0x06;  // Function code
	//address 203A -> Target Speed
	driver->rs485_TxData[2] = 0x20;  // High 8 bit register address
	driver->rs485_TxData[3] = 0x3A;  // Low  8 bit register address
	// Set Speed and Direction
	if(dir == 0){
		driver->rs485_TxData[4] = (speed>>8)&0xFF; // High 8 bit register data
		driver->rs485_TxData[5] = speed&0xFF; // Low  8 bit register address
	}
	if(dir == 1){
		speed = (~speed) + 1;
		driver->rs485_TxData[4] = (speed>>8)&0xFF; // High 8 bit register data
		driver->rs485_TxData[5] = speed&0xFF; // Low  8 bit register address
	}
	uint16_t crc = crc16(driver->rs485_TxData, 6);
	driver->rs485_TxData[6] = crc&0xFF;
	driver->rs485_TxData[7] = (crc>>8)&0xFF;
	rs485_send_data(driver, driver->rs485_TxData);

}

void rs485_enable_velocity_mode(rs485_driver *driver){

	driver->rs485_TxData[0] = driver->rs485_id;
	driver->rs485_TxData[1] = 0x06;  // Function code
	//address 2032 -> Operating Mode
	driver->rs485_TxData[2] = 0x20;  // High 8 bit register address
	driver->rs485_TxData[3] = 0x32;  // Low  8 bit register address
	//data 0x03 -> Set Velocity Mode
	driver->rs485_TxData[4] = 0x00;  // High 8 bit register data
	driver->rs485_TxData[5] = 0x03;  // Low  8 bit register data
	uint16_t crc = crc16(driver->rs485_TxData, 6);
	driver->rs485_TxData[6] = crc&0xFF;
	driver->rs485_TxData[7] = (crc>>8)&0xFF;
	rs485_send_data(driver, driver->rs485_TxData);


	driver->rs485_TxData[0] = driver->rs485_id;
	driver->rs485_TxData[1] = 0x06;  // Function code
	//address 2031 -> Control Word
	driver->rs485_TxData[2] = 0x20;  // High 8 bit register address
	driver->rs485_TxData[3] = 0x31;  // Low  8 bit register address
	//data 0x08 -> Enable Motor
	driver->rs485_TxData[4] = 0x00;  // High 8 bit register data
	driver->rs485_TxData[5] = 0x08;  // Low  8 bit register data
	crc = crc16(driver->rs485_TxData, 6);
	driver->rs485_TxData[6] = crc&0xFF;
	driver->rs485_TxData[7] = (crc>>8)&0xFF;
	rs485_send_data(driver, driver->rs485_TxData);
}

void rs485_UART_receive_handler(rs485_driver *driver){
	driver->rs485_isbusy = 0;
	driver->rs485_timer->Instance->CNT = 0;
	driver->rs485_connectivity = 255;
	HAL_GPIO_WritePin(driver->rs485_rx_led_port, driver->rs485_rx_led_pin, GPIO_PIN_SET);
	HAL_UARTEx_ReceiveToIdle_IT(driver->rs485_huart, driver->rs485_RxData, 16);
}

void rs485_connection_lost_handler(rs485_driver *driver){
	driver->rs485_connectivity= 0;
	HAL_GPIO_WritePin(driver->rs485_enable_port, driver->rs485_enable_pin, GPIO_PIN_SET);
	HAL_UART_Transmit(driver->rs485_huart, driver->rs485_TxData, 8, 500);
	HAL_GPIO_WritePin(driver->rs485_enable_port, driver->rs485_enable_pin, GPIO_PIN_RESET);
	HAL_UARTEx_ReceiveToIdle_IT(driver->rs485_huart, driver->rs485_RxData, 16);
}
