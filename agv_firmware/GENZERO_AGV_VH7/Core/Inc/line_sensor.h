/*
 * line_sensor.h
 *
 *  Created on: Aug 23, 2024
 *      Author: arafi
 */

#ifndef INC_LINE_SENSOR_H_
#define INC_LINE_SENSOR_H_


#include "main.h"


typedef struct ir_array {
    volatile uint16_t ir_sen_val_dma[10];
    volatile uint16_t ir_sen_val_cal[10];
             uint16_t ir_sen_max_val[10];
             uint16_t ir_sen_min_val[10];
             uint8_t  ir_sen_channel_num;
    volatile uint16_t ir_sen_read_line_val;
             int      ir_sen_read_line_old_val;
    volatile uint8_t  ir_sen_on_line_total_num;
    volatile uint8_t  ir_sen_on_line_mid_num;
    volatile uint8_t  ir_sen_on_line_left_num;
    volatile uint8_t  ir_sen_on_line_right_num;
             uint16_t ir_sen_threshold;
    volatile uint8_t  ir_sen_val_cal_trig[10];

}ir_array;


void ir_array_init(ir_array *ir_struct,
                   volatile uint16_t *ir_sen_val_cal,
                   uint16_t *ir_sen_max_val,
                   uint16_t *ir_sen_min_val,
             const uint8_t *ir_sen_channel_num,
          volatile uint16_t *ir_sen_read_line_val,
                   int      *ir_sen_read_line_old_val,
          volatile uint8_t  *ir_sen_on_line_total_num,
          volatile uint8_t  *ir_sen_on_line_mid_num,
		  volatile uint8_t  *ir_sen_on_line_left_num,
          volatile uint8_t  *ir_sen_on_line_right_num,
             const uint16_t *ir_sen_threshold);

void Line_Sensor_Calculation(ir_array *ir_struct);


#endif /* INC_LINE_SENSOR_H_ */
