/*
 * ina226.h
 *
 *  Created on: Sep 14, 2023
 *      Author: 32702
 */

#ifndef INC_IIC_INA226_H_
#define INC_IIC_INA226_H_
#include "i2c.h"
void INA226_init(void);

float INA226_GetBusV(void);
// float INA226_GetCurrent(void);
// float INA226_GetPower(void);

#endif /* INC_IIC_INA226_H_ */
