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
	UART_HandleTypeDef* rs485_huart;
	 	 	 uint8_t  	rs485_RxData[16];
	 	 	 uint8_t  	rs485_TxData[8];
	volatile uint8_t  	rs485_isbusy;
	volatile uint16_t 	rs485_FailCheck;
	volatile uint8_t  	rs485_connectivity;
	volatile uint16_t 	rs485_exc_time;
	volatile uint16_t 	rs485_t1;
	volatile uint16_t 	rs485_t2;
}rs485_driver;


void rs485_init(rs485_driver *driver);
//void rs485_send_data(rs485_driver *driver, uint8_t *data);
void rs485_set_speed(rs485_driver *driver, uint16_t speed, uint8_t dir);

#endif /* INC_RS485_DRIVER_H_ */
