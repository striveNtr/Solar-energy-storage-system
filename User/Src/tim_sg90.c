/*
 * tim_sg90.c
 *
 *  Created on: Sep 17, 2023
 *      Author: 32702
 */


#include "tim_sg90.h"
//2.5%到12.5%
//0.025*1440 = 36      reload arr min   0°
//0.125*1440 = 180     reload arr max   180°
#define ROTA_ANGLE_VAL 2
#define SG90_TIM_MAX	180
#define SG90_TIM_MIN	36

static int sg90_1_angle = 0;
static int sg90_2_angle = 0;

#define SG90_1_ANGLE_CHANGE(dir)	(dir)?(sg90_1_angle+=ROTA_ANGLE_VAL):(sg90_1_angle-=ROTA_ANGLE_VAL)
#define SG90_2_ANGLE_CHANGE(dir)	(dir)?(sg90_1_angle+=ROTA_ANGLE_VAL):(sg90_2_angle-=ROTA_ANGLE_VAL)

void sg90_RotateAngle(uint8_t ch, uint8_t dir)
{	

	if (ch==1)
	{
		if (dir==ANGLE_ADD)
		{
			sg90_1_angle+=ROTA_ANGLE_VAL;
			(sg90_1_angle>SG90_TIM_MAX)?(sg90_1_angle=SG90_TIM_MAX):(0);
		}else
		{
			sg90_1_angle-=ROTA_ANGLE_VAL;
			(sg90_1_angle<SG90_TIM_MIN)?(sg90_1_angle=SG90_TIM_MIN):(0);
		}
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,sg90_1_angle);
	}else if (ch==2)
	{
		if (dir==ANGLE_ADD)
		{
			sg90_2_angle+=ROTA_ANGLE_VAL;
			(sg90_2_angle>108)?(sg90_2_angle=108):(0);
		}else
		{
			sg90_2_angle-=ROTA_ANGLE_VAL;
			(sg90_2_angle<SG90_TIM_MIN)?(sg90_2_angle=SG90_TIM_MIN):(0);
		}
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,sg90_2_angle);
	}
	return;
	
}
