/*
 * spi_tft144_gui.h
 *
 *  Created on: Sep 14, 2023
 *      Author: 32702
 */

#ifndef INC_SPI_TFT144_GUI_H_
#define INC_SPI_TFT144_GUI_H_

#include "main.h"

#define F_SIZE      16

void GUI_DrawPoint(u16 x,u16 y,u16 color);
void GUI_Drawbmp16(u16 x,u16 y,const unsigned char *p);
void LCD_DrawPoint_big(u16 x,u16 y);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle_Fill(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawCircle(int xc, int yc,int r, int fill);
void LCD_DrawTriangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_DrawTriangel_Fill(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_DrawMatrixCode(u16 x, u16 y, u8 width,u8 high,u8 *charPtr,bool overlap);
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u8 charSize,bool overlap);
void LCD_ShowChar(u16 x,u16 y, u8 num,u8 charSize,bool overlap);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 charSize);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 charSize,bool overlap);

void LCD_JustString(u16 x,u16 y,char *p,u8 charSize);
void LCD_ShowString(u16 x, u16 y, const char *str,u8 charSize,bool overlap);
void LCD_StrCenter(u16 x, u16 y, char *str,u8 charSize,bool overlap);


u16 LCD_GetPos_X(u8 charSize,u8 index);
u16 LCD_GetPos_Y(u8 charSize,u8 index);

#endif /* INC_SPI_TFT144_GUI_H_ */
