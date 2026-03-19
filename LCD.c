#include "LCD.h"
#include "font.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;


// ================================SPI底层通信===================================
static HAL_StatusTypeDef SPI_Transmit(uint8_t* send_buf, uint16_t size)
{
    return HAL_SPI_Transmit(&hspi1, send_buf, size, 100);
}
static HAL_StatusTypeDef SPI_Receive(uint8_t* recv_buf, uint16_t size)
{
   	return HAL_SPI_Receive(&hspi1, recv_buf, size, 100);
}


void LCD_SendCmd(uint8_t cmd){
	LCD_CS_CLR();
	LCD_RS_CLR();
	SPI_Transmit(&cmd, 1);
	LCD_CS_SET();
}

void LCD_SendData(uint8_t data){
	LCD_CS_CLR();
	LCD_RS_SET();
	SPI_Transmit(&data, 1);
	LCD_CS_SET();
}


// ================================LCD底层通信===================================

void LCD_RESET(void){
	LCD_RST_CLR();
	HAL_Delay(100);
	LCD_RST_SET();
	HAL_Delay(50);
}

void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{
	LCD_SendCmd(LCD_Reg);
	LCD_SendData(LCD_RegValue);
}

void LCD_WriteData_16Bit(uint16_t Data)
{	
	uint8_t Data_Buf[2];
	Data_Buf[0] = Data>>8;
	Data_Buf[1] = Data;
	LCD_CS_CLR();
	LCD_RS_SET();
	SPI_Transmit(Data_Buf, 2);
	LCD_CS_SET();
}



// ================================LCD显示===================================

void LCD_Init(void){
	LCD_RESET();

	//*************2.8 ILI9341 **********//	
	LCD_SendCmd(0xCF);  
	LCD_SendData(0x00); 
	LCD_SendData(0xC1); 
	LCD_SendData(0x30); 
 
	LCD_SendCmd(0xED);  
	LCD_SendData(0x64); 
	LCD_SendData(0x03); 
	LCD_SendData(0X12); 
	LCD_SendData(0X81); 
 
	LCD_SendCmd(0xE8);  
	LCD_SendData(0x85); 
	LCD_SendData(0x00); 
	LCD_SendData(0x78); 

	LCD_SendCmd(0xCB);  
	LCD_SendData(0x39); 
	LCD_SendData(0x2C); 
	LCD_SendData(0x00); 
	LCD_SendData(0x34); 
	LCD_SendData(0x02); 
	
	LCD_SendCmd(0xF7);  
	LCD_SendData(0x20); 
 
	LCD_SendCmd(0xEA);  
	LCD_SendData(0x00); 
	LCD_SendData(0x00); 

	LCD_SendCmd(0xC0);       //Power control 
	LCD_SendData(0x13);     //VRH[5:0] 
 
	LCD_SendCmd(0xC1);       //Power control 
	LCD_SendData(0x13);     //SAP[2:0];BT[3:0] 
 
	LCD_SendCmd(0xC5);       //VCM control 
	LCD_SendData(0x22);   //22
	LCD_SendData(0x35);   //35
 
	LCD_SendCmd(0xC7);       //VCM control2 
	LCD_SendData(0xBD);  //AF

	LCD_SendCmd(0x21);

	LCD_SendCmd(0x36);       // Memory Access Control 
	LCD_SendData(0x08); 

	LCD_SendCmd(0xB6);  
	LCD_SendData(0x0A); 
	LCD_SendData(0xA2); 

	LCD_SendCmd(0x3A);       
	LCD_SendData(0x55); 

	LCD_SendCmd(0xF6);  //Interface Control
	LCD_SendData(0x01); 
	LCD_SendData(0x30);  //MCU

	LCD_SendCmd(0xB1);       //VCM control 
	LCD_SendData(0x00); 
	LCD_SendData(0x1B); 
 
	LCD_SendCmd(0xF2);       // 3Gamma Function Disable 
	LCD_SendData(0x00); 
 
	LCD_SendCmd(0x26);       //Gamma curve selected 
	LCD_SendData(0x01); 
 
	LCD_SendCmd(0xE0);       //Set Gamma 
	LCD_SendData(0x0F); 
	LCD_SendData(0x35); 
	LCD_SendData(0x31); 
	LCD_SendData(0x0B); 
	LCD_SendData(0x0E); 
	LCD_SendData(0x06); 
	LCD_SendData(0x49); 
	LCD_SendData(0xA7); 
	LCD_SendData(0x33); 
	LCD_SendData(0x07); 
	LCD_SendData(0x0F); 
	LCD_SendData(0x03); 
	LCD_SendData(0x0C); 
	LCD_SendData(0x0A); 
	LCD_SendData(0x00); 
 
	LCD_SendCmd(0XE1);       //Set Gamma 
	LCD_SendData(0x00); 
	LCD_SendData(0x0A); 
	LCD_SendData(0x0F); 
	LCD_SendData(0x04); 
	LCD_SendData(0x11); 
	LCD_SendData(0x08); 
	LCD_SendData(0x36); 
	LCD_SendData(0x58); 
	LCD_SendData(0x4D); 
	LCD_SendData(0x07); 
	LCD_SendData(0x10); 
	LCD_SendData(0x0C); 
	LCD_SendData(0x32); 
	LCD_SendData(0x34); 
	LCD_SendData(0x0F); 

	LCD_SendCmd(0x11);       //Exit Sleep 
	HAL_Delay(120); 
	LCD_SendCmd(0x29);       //Display on 
    LCD_direction(3);//    LCD       
	LCD_Clear(BLACK);
}

void LCD_direction(uint8_t direction)
{ 
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	lcddev.wramcmd=0x2C;
	lcddev.rramcmd=0x2E;
			lcddev.dir = direction%4;
	switch(lcddev.dir){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:
		break;
	}		
}	 


void LCD_WriteRAM_Prepare(void)
{
	LCD_SendCmd(lcddev.wramcmd);
}	 


void LCD_ReadRAM_Prepare(void)
{
	LCD_SendCmd(lcddev.rramcmd);
}	 


void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{	
	LCD_SendCmd(lcddev.setxcmd);	
	LCD_SendData(xStar>>8);
	LCD_SendData(0x00FF&xStar);		
	LCD_SendData(xEnd>>8);
	LCD_SendData(0x00FF&xEnd);

	LCD_SendCmd(lcddev.setycmd);	
	LCD_SendData(yStar>>8);
	LCD_SendData(0x00FF&yStar);		
	LCD_SendData(yEnd>>8);
	LCD_SendData(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

void LCD_Clear(uint16_t Color)
{
	unsigned int i,m;  
	uint8_t send_buf[2];
	send_buf[0] = Color>>8;
	send_buf[1] = Color;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	LCD_CS_CLR();
	LCD_RS_SET();
	for(i=0;i<lcddev.height;i++)
	{
		for(m=0;m<lcddev.width;m++)
		{	
		SPI_Transmit(send_buf, 2);
		}
	}
	 LCD_CS_SET();
} 


void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetCursor(x,y);//设置光标位置 
	LCD_WriteData_16Bit(color); 
}

// ================================================绘图功能==============================================

// 							320
// 				----------------------------> 	X
//			    |
//				|
// 				|
// 		240		|
// 				|
// 				|
// 				v
// 
// 				Y

/**
 * @brief 绘制一条线段
 * @param x1 起始点横坐标
 * @param y1 起始点纵坐标
 * @param x2 终止点横坐标
 * @param y2 终止点纵坐标
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制线段
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
  static uint16_t temp = 0;
  if (x1 == x2)										//绘制竖直线
  {
    if (y1 > y2)
    {
      temp = y1;
      y1 = y2;
      y2 = temp;
    }
    for (uint16_t y = y1; y <= y2; y++)
    {
      LCD_DrawPoint(x1, y,color);
    }
  }
  else if (y1 == y2)								//绘制水平线
  {
    if (x1 > x2)
    {
      temp = x1;
      x1 = x2;
      x2 = temp;
    }
    for (uint16_t x = x1; x <= x2; x++)
    {
      LCD_DrawPoint(x, y1,color);
    }
  }
  else
  {
    // Bresenham直线算法
    int16_t dx = x2 - x1;								//x1 == 10    x2 == 1    	
    int16_t dy = y2 - y1;								//y1 == 3     y2 == 7
    int16_t ux = ((dx > 0) << 1) - 1;					//dx < 0   ux == -1
    int16_t uy = ((dy > 0) << 1) - 1;					//dy > 0   uy == +1		+-表示方向                                   
    int16_t x = x1, y = y1, eps = 0;
    dx = abs(dx);
    dy = abs(dy);
    if (dx > dy)
    {
      for (x = x1; x != x2; x += ux)
      {
        LCD_DrawPoint(x, y,color);
        eps += dy;
        if ((eps << 1) >= dx)
        {
          y += uy;
          eps -= dx;
        }
      }
    }
    else
    {
      for (y = y1; y != y2; y += uy)
      {
        LCD_DrawPoint(x, y,color);
        eps += dx;
        if ((eps << 1) >= dy)
        {
          x += ux;
          eps -= dy;
        }
      }
    }
  }
}


/**
 * @brief 绘制一个矩形
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param color 颜色
 */
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  LCD_DrawLine(x, y, x + w, y, color);
  LCD_DrawLine(x, y + h, x + w, y + h, color);
  LCD_DrawLine(x, y, x, y + h, color);
  LCD_DrawLine(x + w, y, x + w, y + h, color);
}


/**
 * @brief 绘制一个三角形
 * @param x1 第一个点横坐标
 * @param y1 第一个点纵坐标
 * @param x2 第二个点横坐标
 * @param y2 第二个点纵坐标
 * @param x3 第三个点横坐标
 * @param y3 第三个点纵坐标
 * @param color 颜色
 */
void LCD_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
  LCD_DrawLine(x1, y1, x2, y2, color);
  LCD_DrawLine(x2, y2, x3, y3, color);
  LCD_DrawLine(x3, y3, x1, y1, color);
}


/**
 * @brief 绘制一个圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 圆半径
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制圆
 */
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
  int16_t a = 0, b = r, di = 3 - (r << 1);
  while (a <= b)
  {
    LCD_DrawPoint(x - b, y - a, color);
    LCD_DrawPoint(x + b, y - a, color);
    LCD_DrawPoint(x - a, y + b, color);
    LCD_DrawPoint(x - b, y - a, color);
    LCD_DrawPoint(x - a, y - b, color);
    LCD_DrawPoint(x + b, y + a, color);
    LCD_DrawPoint(x + a, y - b, color);
    LCD_DrawPoint(x + a, y + b, color);
    LCD_DrawPoint(x - b, y + a, color);
    a++;
    if (di < 0)
    {
      di += 4 * a + 6;
    }
    else
    {
      di += 10 + 4 * (a - b);
      b--;
    }
    LCD_DrawPoint(x + a, y + b, color);
  }
}


/**
 * @brief 绘制一个椭圆
 * @param x 椭圆中心横坐标
 * @param y 椭圆中心纵坐标
 * @param a 椭圆长轴
 * @param b 椭圆短轴
 */
void LCD_DrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint16_t color)
{
  int xpos = 0, ypos = b;
  int a2 = a * a, b2 = b * b;
  int d = b2 + a2 * (0.25 - b);
  while (a2 * ypos > b2 * xpos)
  {
    LCD_DrawPoint(x + xpos, y + ypos, color);
    LCD_DrawPoint(x - xpos, y + ypos, color);
    LCD_DrawPoint(x + xpos, y - ypos, color);
    LCD_DrawPoint(x - xpos, y - ypos, color);
    if (d < 0)
    {
      d = d + b2 * ((xpos << 1) + 3);
      xpos += 1;
    }
    else
    {
      d = d + b2 * ((xpos << 1) + 3) + a2 * (-(ypos << 1) + 2);
      xpos += 1, ypos -= 1;
    }
  }
  d = b2 * (xpos + 0.5) * (xpos + 0.5) + a2 * (ypos - 1) * (ypos - 1) - a2 * b2;
  while (ypos > 0)
  {
    LCD_DrawPoint(x + xpos, y + ypos, color);
    LCD_DrawPoint(x - xpos, y + ypos, color);
    LCD_DrawPoint(x + xpos, y - ypos, color);
    LCD_DrawPoint(x - xpos, y - ypos, color);
    if (d < 0)
    {
      d = d + b2 * ((xpos << 1) + 2) + a2 * (-(ypos << 1) + 3);
      xpos += 1, ypos -= 1;
    }
    else
    {
      d = d + a2 * (-(ypos << 1) + 3);
      ypos -= 1;
    }
  }
}


//========================================绘制字符==================================================

/**
 * @brief 绘制一个ASCII字符
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param ch 字符
 * @param font 字体
 * @param color 颜色
 */

void LCD_PrintASCIIChar(uint16_t x, uint16_t y, char ch, const ASCIIFont *font, uint16_t color){

    // ASCII有效范围：32~126
    if(ch < ' ' || ch > '~')return;
    /**
     * 每一行占多少字节
     * 例如：
     * 6像素宽 → 1byte
     * 12像素宽 → 2byte
     */
    uint8_t lineBytes = (font->w + 7) / 8;

    /**
     * 一个字符总共占多少字节
     */
    uint16_t charBytes = lineBytes * font->h;

    /**
     * 找到当前字符的字模起始地址
     */
    const uint8_t *pChar =
        font->chars + (ch - ' ') * charBytes;

    /**
     * 遍历字符的每一个像素
     */
    for(uint8_t row = 0; row < font->h; row++)
    {
        for(uint8_t col = 0; col < font->w; col++)
        {
            /**
             * 找到当前像素所在的字节
             *
             * row * lineBytes
             *   → 当前行起始地址
             *
             * col/8
             *   → 当前列所在字节
             */
            uint8_t byte = pChar[row * lineBytes + col / 8];

            /**
             * 判断当前像素bit
             *
             * 0x80 >> (col%8)
             * 表示从高位到低位读取
             */
            if(byte & (0x80 >> (col % 8)))
            {
                LCD_DrawPoint(x+col, y+row, color);
            }
        }
	}

}

void LCD_PrintASCIIString(uint16_t x, uint16_t y, char *str, const ASCIIFont *font, uint16_t color)
{
    while(*str)
    {
        LCD_PrintASCIIChar(x, y, *str, font, color);
        x += font->w;
		// if (lcddev.dir %2) {
		// 	x%=lcddev.width;
		// }else {
		// 	x%=lcddev.height;
		// }
        str++;
    }
}

/**
 * @brief 获取UTF-8编码的字符长度
 */
uint8_t _LCD_GetUTF8Len(char *string)
{
  if ((string[0] & 0x80) == 0x00)
  {
    return 1;
  }
  else if ((string[0] & 0xE0) == 0xC0)
  {
    return 2;
  }
  else if ((string[0] & 0xF0) == 0xE0)
  {
    return 3;
  }
  else if ((string[0] & 0xF8) == 0xF0)
  {
    return 4;
  }
  return 0;
}


