/*
 * rs485_driver.h
 *
 *  Created on: Aug 27, 2024
 *      Author: ASUS
 */

#ifndef INC_RS485_DRIVER_H_
#define INC_RS485_DRIVER_H_

#include "main.h"
#include "modbus_crc.h"

typedef struct rs485_driver{
	         uint8_t  	rs485_id;
	GPIO_TypeDef* 	  	rs485_enable_port;
			 uint16_t 	rs485_enable_pin;
	GPIO_TypeDef* 	  	rs485_rx_led_port;
			 uint16_t 	rs485_rx_led_pin;
	UART_HandleTypeDef* rs485_huart;
	TIM_HandleTypeDef*  rs485_timer;
	 	 	 uint8_t  	rs485_RxData[16];
	 	 	 uint8_t  	rs485_TxData[8];
	volatile uint8_t  	rs485_isbusy;
	volatile uint8_t  	rs485_connectivity;
	volatile uint8_t  	rs485_last_connectivity;
}rs485_driver;


void rs485_init(rs485_driver *driver);
void rs485_send_data(rs485_driver *driver, uint8_t *data);
void rs485_set_speed(rs485_driver *driver, uint16_t speed, uint8_t dir);
void rs485_enable_velocity_mode(rs485_driver *driver);
void rs485_UART_receive_handler(rs485_driver *driver);
void rs485_connection_lost_handler(rs485_driver *driver);

#endif /* INC_RS485_DRIVER_H_ */
