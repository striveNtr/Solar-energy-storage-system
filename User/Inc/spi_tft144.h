/*
 * spi_tft144.h
 *
 *  Created on: Sep 14, 2023
 *      Author: 32702
 */

#ifndef INC_SPI_TFT144_H_
#define INC_SPI_TFT144_H_

#include "main.h"

#define LCD_1_4  //LCD_1_4 采用1.44寸屏 LCD_2_4采用2.4寸屏

typedef enum
{
    Color16_WHITE         =0xFFFF,
    Color16_BLACK         =0x0000,
    Color16_BLUE          =0x001F,
    Color16_BRED          =0XF81F,
    Color16_GRED          =0XFFE0,
    Color16_GBLUE         =0X07FF,
    Color16_RED           =0xF800,
    Color16_MAGENTA       =0xF81F,
    Color16_GREEN         =0x07E0,
    Color16_CYAN          =0x7FFF,
    Color16_YELLOW        =0xFFE0,
    Color16_BROWN         =0XBC40, //棕色
    Color16_BRRED         =0XFC07, //棕红色
    Color16_GRAY          =0X8430, //灰色
    //GUI颜色

    Color16_DARKBLUE      =0X01CF,//深蓝色
    Color16_LIGHTBLUE     =0X7D7C,//浅蓝色
    Color16_GRAYBLUE      =0X5458, //灰蓝色
    //以上三色为PANEL的颜色

    Color16_LIGHTGREEN    =0X841F, //浅绿色
    Color16_LIGHTGRAY     =0XEF5B,//浅灰色(PANNEL)
    Color16_LGRAY         =0XC618, //浅灰色(PANNEL),窗体背景色

    Color16_LGRAYBLUE     =0XA651, //浅灰蓝色(中间层颜色)
    Color16_LBBLUE        =0X2B12, //浅棕蓝色(选择条目的反色)
}Color16;

typedef struct
{
    u16 width;//LCD 宽度
    u16 height;//LCD 高度
    u16 id;//LCD ID
    u8  dir;//横屏还是竖屏控制：0，竖屏；1，横屏。
    u16 wramcmd;//开始写gram指令
    u16 setxcmd;//设置x坐标指令
    u16 setycmd;//设置y坐标指令
    u8  xoffset;
    u8  yoffset;
}_lcd_dev;


// #define LCD_CLK     WritePin(B,10)
#define LCD_CLK_Clr()  HAL_GPIO_WritePin(TFT_SCL_GPIO_Port,TFT_SCL_Pin, GPIO_PIN_RESET)//SCL
#define LCD_CLK_Set()  HAL_GPIO_WritePin(TFT_SCL_GPIO_Port,TFT_SCL_Pin, GPIO_PIN_SET)
// #define LCD_MOSI    WritePin(B,11)
#define LCD_MOSI_Clr()  HAL_GPIO_WritePin(TFT_SDA_GPIO_Port,TFT_SDA_Pin, GPIO_PIN_RESET)//RES
#define LCD_MOSI_Set()  HAL_GPIO_WritePin(TFT_SDA_GPIO_Port,TFT_SDA_Pin, GPIO_PIN_SET)
// #define LCD_RES     WritePin(B,12)
#define LCD_RES_Clr()  HAL_GPIO_WritePin(TFT_RES_GPIO_Port,TFT_RES_Pin, GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(TFT_RES_GPIO_Port,TFT_RES_Pin, GPIO_PIN_SET)
// #define LCD_DC      WritePin(B,13)
#define LCD_DC_Clr()   HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin, GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin, GPIO_PIN_SET)

#ifdef LCD_1_4 //采用1.44寸屏
// #define LCD_CS1     WritePin(B,14)
#define LCD_CS_Clr()   HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin, GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin, GPIO_PIN_SET)
// #define LCD_LED     WritePin(B,15)
#define LCD_BLK_Clr()  HAL_GPIO_WritePin(TFT_BL_GPIO_Port,TFT_BL_Pin, GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(TFT_BL_GPIO_Port,TFT_BL_Pin, GPIO_PIN_SET)

#define USE_HORIZONTAL      0//定义液晶屏顺时针旋转方向     0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
//定义LCD的尺寸
#define LCD_W 128
#define LCD_H 128
#elif defined(LCD_2_4)
#define LCD_LED     WritePin(B,11)
#define LCD_CS1     WritePin(B,13)
static const MyPinDef Pins_LCD_ILI9341[] = {PB7,PB8,PB9,PB10,PB11,PB13};

#define USE_HORIZONTAL      2//定义液晶屏顺时针旋转方向     0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 320
#endif

//////////////////////////////////////////////////////////////////////////////////
//LCD参数
//TFTLCD部分外要调用的函数
extern Color16 FRONT_COLOR;//默认红色
extern Color16 BACK_COLOR; //背景颜色.默认为白色
extern _lcd_dev lcddev;//管理LCD重要参数


void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(Color16 color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//画点
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_direction(u8 direction );
//void SPIv_WriteData(u8 Data);//
void LCD_WriteData_16Bit(u16 Data);


#endif /* INC_SPI_TFT144_H_ */
