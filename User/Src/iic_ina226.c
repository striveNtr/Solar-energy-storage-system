/*
 * ina226.c
 *
 *  Created on: Sep 14, 2023
 *      Author: 32702
 */
#include "iic_ina226.h"


uint8_t INA226_SetConfig(uint16_t ConfigWord);
uint8_t INA226_SetCalibrationReg(uint16_t ConfigWord);
// uint8_t INA226_SetMaskEnable(uint16_t ConfigWord);
// uint8_t INA226_SetAlertLimit(uint16_t ConfigWord);

// uint16_t INA226_GetConfig(void);
// uint16_t INA226_GetShuntV(void);
uint16_t INA226_GetBusVReg(void);
// uint16_t INA226_GetPowerReg(void);
// uint16_t INA226_GetCalibrationReg(void);
// uint16_t INA226_GetCurrentReg(void);
// uint16_t INA226_GetManufID(void);
// uint16_t INA226_GetDieID(void);
// uint16_t INA226_GetMaskEnable(void);
// uint16_t INA226_GetAlertLimit(void);


#define INA226_COM_PORT	hi2c1		/*通讯使用的IIC接口*/

#define INA226_ADDRESS 		0x80	/*INA226的地址*/
#define INA226_I2C_TIMEOUT 	10		/*IIC通讯超时*/


#define INA226_CALIB_VAL 1024
#define INA226_CURRENTLSB 0.5F // mA/bit
#define INA226_CURRENTLSB_INV 1/INA226_CURRENTLSB // bit/mA
#define INA226_POWERLSB_INV 1/(INA226_CURRENTLSB*25) // bit/mW


#define INA226_CONFIG 	0x00 // Configuration Register (R/W)初始值4127
#define INA226_SHUNTV 	0x01 // Shunt Voltage (R)初始值0，分流电压测量值
#define INA226_BUSV 	0x02 // Bus Voltage (R)初始值0，总线电压测量值
#define INA226_POWER 	0x03 // Power (R)初始值0，输出功率测量值
#define INA226_CURRENT 	0x04 // Current (R)初始值0，分流电阻电流计算值
#define INA226_CALIB 	0x05 // Calibration (R/W)，设置全量程和电流LSB
#define INA226_MASK 	0x06 // Mask/Enable (R/W)，报警设置和转换准备标志
#define INA226_ALERTL 	0x07 // Alert Limit (R/W)，报警阈值
#define INA226_MANUF_ID 0xFE // Manufacturer ID (R)，0x5449
#define INA226_DIE_ID 	0xFF // Die ID (R),0x2260

#define INA226_MODE_POWER_DOWN 			(0<<0) // Power-Down
#define INA226_MODE_TRIG_SHUNT_VOLTAGE 	(1<<0) // Shunt Voltage, Triggered
#define INA226_MODE_TRIG_BUS_VOLTAGE 	(2<<0) // Bus Voltage, Triggered
#define INA226_MODE_TRIG_SHUNT_AND_BUS 	(3<<0) // Shunt and Bus, Triggered
#define INA226_MODE_POWER_DOWN2 		(4<<0) // Power-Down
#define INA226_MODE_CONT_SHUNT_VOLTAGE 	(5<<0) // Shunt Voltage, Continuous
#define INA226_MODE_CONT_BUS_VOLTAGE 	(6<<0) // Bus Voltage, Continuous
#define INA226_MODE_CONT_SHUNT_AND_BUS 	(7<<0) // Shunt and Bus, Continuous

// Shunt Voltage Conversion Time
#define INA226_VSH_140uS 	(0<<3)
#define INA226_VSH_204uS 	(1<<3)
#define INA226_VSH_332uS 	(2<<3)
#define INA226_VSH_588uS 	(3<<3)
#define INA226_VSH_1100uS 	(4<<3)
#define INA226_VSH_2116uS 	(5<<3)
#define INA226_VSH_4156uS 	(6<<3)
#define INA226_VSH_8244uS 	(7<<3)

// Bus Voltage Conversion Time (VBUS CT Bit Settings[6-8])
#define INA226_VBUS_140uS 	(0<<6)
#define INA226_VBUS_204uS 	(1<<6)
#define INA226_VBUS_332uS 	(2<<6)
#define INA226_VBUS_588uS 	(3<<6)
#define INA226_VBUS_1100uS 	(4<<6)
#define INA226_VBUS_2116uS 	(5<<6)
#define INA226_VBUS_4156uS 	(6<<6)
#define INA226_VBUS_8244uS 	(7<<6)

// Averaging Mode (AVG Bit Settings[9-11])
#define INA226_AVG_1 	(0<<9)
#define INA226_AVG_4 	(1<<9)
#define INA226_AVG_16 	(2<<9)
#define INA226_AVG_64 	(3<<9)
#define INA226_AVG_128 	(4<<9)
#define INA226_AVG_256 	(5<<9)
#define INA226_AVG_512 	(6<<9)
#define INA226_AVG_1024 (7<<9)

// Reset Bit (RST bit [15])
#define INA226_RESET_ACTIVE 	(1<<15)
#define INA226_RESET_INACTIVE 	(0<<15)

// Mask/Enable Register
#define INA226_MER_SOL 	(1<<15) // Shunt Voltage Over-Voltage
#define INA226_MER_SUL 	(1<<14) // Shunt Voltage Under-Voltage
#define INA226_MER_BOL 	(1<<13) // Bus Voltagee Over-Voltage
#define INA226_MER_BUL 	(1<<12) // Bus Voltage Under-Voltage
#define INA226_MER_POL 	(1<<11) // Power Over-Limit
#define INA226_MER_CNVR (1<<10) // Conversion Ready
#define INA226_MER_AFF 	(1<<4) // Alert Function Flag
#define INA226_MER_CVRF (1<<3) // Conversion Ready Flag
#define INA226_MER_OVF 	(1<<2) // Math Overflow Flag
#define INA226_MER_APOL (1<<1) // Alert Polarity Bit
#define INA226_MER_LEN 	(1<<0) // Alert Latch Enable

#define INA226_MANUF_ID_DEFAULT 	0x5449
#define INA226_DIE_ID_DEFAULT 		0x2260


void INA226_init(void)
{
	/*
	* 设置转换时间8.244ms,求平均值次数16，设置模式为分流和总线连续模式
	* 总数据转换时间 = 8.244*16 = 131.9ms
	*/
    INA226_SetConfig(0x45FF);

	/*
	* 分流电阻最大电压 = 32768 * 0.0000025V = 0.08192V
	* 设置分流电压转电流转换参数:电阻0.01R，分辨率0.2mA
	* 公式1
	* Current_LSB = 预期最大电流 / 2^15
	* Current_LSB = 5 / 32768 = 0.000152A ,选0.2ma
	* 公式2
	* CAL = 0.00512/(Current_LSB*R)
	* CAL = 0.00512/(0.0002*0.01)=2560 = 0x0a00
	*/
    INA226_SetCalibrationReg(0x0a00);

}
/*
**************************************************
* 说明：读取BUS电压，并转换为浮点数据
**************************************************
*/
float INA226_GetBusV(void)
{
	uint16_t regData;
	float fVoltage;
    regData = INA226_GetBusVReg();
	fVoltage = regData * 0.00125f;/*电压的LSB = 1.25mV*/
	return fVoltage;
}
/*
**************************************************
* 说明：读取电流，并转换为浮点数据
**************************************************
*/
// float INA226_GetCurrent()
// {
// 	uint16_t regData;
// 	float fCurrent;
// 	regData = INA226_GetCurrentReg();
// 	if(regData >= 0x8000)	regData = 0;
// 	fCurrent = regData * 0.0002f;/*电流的LSB = 0.2mA，由用户配置*/
// 	return fCurrent;
// }
/*
**************************************************
* 说明：读取功率，并转换为浮点数据
**************************************************
*/
// float INA226_GetPower()
// {
// 	uint16_t regData;
// 	float fPower;
// 	regData = INA226_GetPowerReg();
// 	fPower = regData * 0.005f;/*功率的LSB = 电流的LSB*25*/
// 	return fPower;
// }

uint8_t INA226_SetConfig(uint16_t ConfigWord)
{
    uint8_t SentTable[3];
    SentTable[0] = INA226_CONFIG;
    SentTable[1] = (ConfigWord & 0xFF00) >> 8;
    SentTable[2] = (ConfigWord & 0x00FF);
    return HAL_I2C_Master_Transmit(&INA226_COM_PORT, INA226_ADDRESS, SentTable, 3, INA226_I2C_TIMEOUT);
}

// uint16_t INA226_GetConfig()
// {
//     uint8_t SentTable[1] = {INA226_CONFIG};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint16_t INA226_GetShuntV()
// {
//     uint8_t SentTable[1] = {INA226_SHUNTV};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

uint16_t INA226_GetBusVReg()
{
    uint8_t SentTable[1] = {INA226_BUSV};
    uint8_t ReceivedTable[2];
    HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
    if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
    else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
}

uint8_t INA226_SetCalibrationReg(uint16_t ConfigWord)
{
    uint8_t SentTable[3];
    SentTable[0] = INA226_CALIB;
    SentTable[1] = (ConfigWord & 0xFF00) >> 8;
    SentTable[2] = (ConfigWord & 0x00FF);
    return HAL_I2C_Master_Transmit(&INA226_COM_PORT, INA226_ADDRESS, SentTable, 3, INA226_I2C_TIMEOUT);
}

// uint16_t INA226_GetCalibrationReg()
// {
//     uint8_t SentTable[1] = {INA226_CALIB};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint16_t INA226_GetPowerReg()
// {
//     uint8_t SentTable[1] = {INA226_POWER};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint16_t INA226_GetCurrentReg()
// {
//     uint8_t SentTable[1] = {INA226_CURRENT};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint16_t INA226_GetManufID()
// {
//     uint8_t SentTable[1] = {INA226_MANUF_ID};
//     uint8_t ReceivedTable[2];

//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint16_t INA226_GetDieID()
// {
//     uint8_t SentTable[1] = {INA226_DIE_ID};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint8_t INA226_SetMaskEnable(uint16_t ConfigWord)
// {
//     uint8_t SentTable[3];
//     SentTable[0] = INA226_MASK;
//     SentTable[1] = (ConfigWord & 0xFF00) >> 8;
//     SentTable[2] = (ConfigWord & 0x00FF);
//     return HAL_I2C_Master_Transmit(&INA226_COM_PORT, INA226_ADDRESS, SentTable, 3, INA226_I2C_TIMEOUT);
// }

// uint16_t INA226_GetMaskEnable()
// {
//     uint8_t SentTable[1] = {INA226_MASK};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }

// uint8_t INA226_SetAlertLimit(uint16_t ConfigWord)
// {
//     uint8_t SentTable[3];
//     SentTable[0] = INA226_ALERTL;
//     SentTable[1] = (ConfigWord & 0xFF00) >> 8;
//     SentTable[2] = (ConfigWord & 0x00FF);
//     return HAL_I2C_Master_Transmit(&INA226_COM_PORT, INA226_ADDRESS, SentTable, 3, INA226_I2C_TIMEOUT);
// }

// uint16_t INA226_GetAlertLimit()
// {
//     uint8_t SentTable[1] = {INA226_ALERTL};
//     uint8_t ReceivedTable[2];
//     HAL_I2C_Master_Transmit(&INA226_COM_PORT,INA226_ADDRESS, SentTable, 1, INA226_I2C_TIMEOUT);
//     if (HAL_I2C_Master_Receive(&INA226_COM_PORT,INA226_ADDRESS, ReceivedTable, 2, INA226_I2C_TIMEOUT) != HAL_OK) return 0xFF;
//     else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);
// }
