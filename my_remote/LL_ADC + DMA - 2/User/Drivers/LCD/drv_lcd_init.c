#include "drv_lcd_init.h"
#include "bsp_spi.h"

uint8_t Buff[2*LCD_W] = {0};


void LCD_Wri_Bus(uint8_t dat)
{
	SPI_Send_8Byte(LCD_SPI, dat, 100);
	
}

/**
  * @brief  8字节数据写入
  */
void LCD_WR_DATA8(u8 dat)
{
	LCD_DC(1);//写数据
	LCD_Wri_Bus(dat);
}
/**
  * @brief  16字节数据写入
  */
void LCD_WR_DATA(u16 dat)
{
	LCD_DC(1);//写数据
	LCD_Wri_Bus(dat>>8);
	LCD_Wri_Bus(dat);
}
/**
  * @brief  写入命令
  */
void LCD_WR_REG(u8 dat)
{
	LCD_DC(0);//写命令
	LCD_Wri_Bus(dat);
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
}

void drv_LCD_Init(void)
{
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_5, (uint32_t)(&LCD_SPI->DR));
	LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_5,(uint32_t)Buff);
	LL_SPI_EnableDMAReq_TX(LCD_SPI);
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_5);
	LL_SPI_Enable(LCD_SPI);
	LCD_RES(1);
	LCD_CS(0);
	delay_ms_nos(100);
	
	LCD_BLK(1);//打开背光
  delay_ms_nos(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out 
	delay_ms_nos(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------//  
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x2d); 
	LCD_WR_REG(0xB2); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x2d); 
	LCD_WR_REG(0xB3); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x2d); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x2d); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_WR_REG(0xB4); //Dot inversion 
	LCD_WR_DATA8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA8(0xa2); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x84);
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA8(0xc5); 
	LCD_WR_REG(0xC2); 
	LCD_WR_DATA8(0x0a); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_REG(0xC3); 
	LCD_WR_DATA8(0x8a); 
	LCD_WR_DATA8(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA8(0x8a); 
	LCD_WR_DATA8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA8(0x0E); 
	LCD_WR_REG(0x36); //MX, MY, RGB mode 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0); 
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_WR_REG(0xE0); 
	
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x1a); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x18); 
	LCD_WR_DATA8(0x2f); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x20); 
	LCD_WR_DATA8(0x22); 
	LCD_WR_DATA8(0x1f); 
	LCD_WR_DATA8(0x1b); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x37); 
	LCD_WR_DATA8(0x00); 	
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x10);
	
	LCD_WR_REG(0xE1); 
	
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x1b); 
	LCD_WR_DATA8(0x0f); 
	LCD_WR_DATA8(0x17); 
	LCD_WR_DATA8(0x33); 
	LCD_WR_DATA8(0x2c); 
	LCD_WR_DATA8(0x29); 
	LCD_WR_DATA8(0x2e); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x39); 
	LCD_WR_DATA8(0x3f); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x10); 
	
	LCD_WR_REG(0x2b);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x9f);
	
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA8(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA8(0x00);
	
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA8(0x05); 
	LCD_WR_REG(0x29); //Display on 
}
