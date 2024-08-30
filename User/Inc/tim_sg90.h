/*
 * tim_sg90.h
 *
 *  Created on: Sep 17, 2023
 *      Author: 32702
 */

#ifndef INC_TIM_SG90_H_
#define INC_TIM_SG90_H_
#include "tim.h"
#define ANGLE_ADD       1
#define ANGLE_SUBTRACT  0

void sg90_RotateAngle(uint8_t ch, uint8_t dir);

#endif /* INC_TIM_SG90_H_ */
