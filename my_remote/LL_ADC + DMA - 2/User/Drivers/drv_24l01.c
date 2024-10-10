/**
  ******************************************************************************
  * @file    drv_24L01.c/h
  * @brief   Code for NRF24L01 driver in STM32 series MCU	
	===================================================================      
									##### How to use this driver #####
	=================================================================== 
    @note
			-#调用drv_24l01_Init初始化无线模块
			-#使用NRF24L01_Check检查模块是否在线
			-#使用NRF24L01_TX_Mode设置发送模式
			-#使用NRF24L01_TxPacket发送一次数据
			-#使用NRF24L01_RX_Mode设置接收模式
			-#使用NRF24L01_RxPacket接收一次数据
			
    @attention
			-# 本模块需要依赖bsp_spi.c/h
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have any 
  * bugs, update the version Number, write dowm your name and the date. The most
  * important thing is make sure the users will have clear and definite under-
  * standing through your new brief.
  ******************************************************************************
*/
#include "drv_24l01.h"
#include "bsp_spi.h"

#define NRF_SPIx		SPI2
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //接收地址

/**
  * @brief  初始化nrf24l01
 **/	
void drv_24l01_Init(void)
{
	NRF_CE(0);
	NRF_CS(1);
	SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV32);
	LL_SPI_Enable(NRF_SPIx);
}

/**
  * @brief  写寄存器
	* @param	reg:	 寄存器
	* @param	value: 写入的值
 **/
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  NRF_CS(0);
  status =SPI_ReadWrite_8Byte(NRF_SPIx, reg);//发送寄存器号 
  SPI_ReadWrite_8Byte(NRF_SPIx, value);      //写入寄存器的值
	NRF_CS(1);  								//不选中nrf
  return(status);       			//返回状态值
}

/**
  * @brief  读寄存器
	* @param	reg:	 寄存器
 **/
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
	NRF_CS(0);		
  SPI_ReadWrite_8Byte(NRF_SPIx, reg);   //发送寄存器号
  reg_val=SPI_ReadWrite_8Byte(NRF_SPIx, 0XFF);//读取寄存器内容
  NRF_CS(1);		    
  return(reg_val);           //返回状态值
}	

/**
  * @brief  在指定位置读出指定长度的数据
	* @param	reg:	寄存器
	* @param	pBuf:	数据指针
	* @param	len:	数据长度
	* @return 此次读到的状态寄存器值
 **/
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
  uint8_t status,u8_ctr;	       
  NRF_CS(0);          
  status=SPI_ReadWrite_8Byte(NRF_SPIx, reg);//发送寄存器值(位置),并读取状态值   	   
  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI_ReadWrite_8Byte(NRF_SPIx, 0XFF);//读出数据
  NRF_CS(1);
  return status;        //返回读到的状态值
}

/**
  * @brief  在指定位置写指定长度的数据
	* @param	reg:	寄存器
	* @param	pBuf:	数据指针
	* @param	len:	数据长度
	* @return 此次读到的状态寄存器值
 **/
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
  uint8_t status,u8_ctr;	    
 	NRF_CS(0);          
  status = SPI_ReadWrite_8Byte(NRF_SPIx, reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_ReadWrite_8Byte(NRF_SPIx, *pBuf++); //写入数据	 
  NRF_CS(1);       
  return status;          //返回读到的状态值
}	


/**
  * @brief  检测24L01是否存在
	* @return 0，成功;1，失败
 **/
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	//SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV64);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	

/**
  * @brief  启动NRF24L01发送一次数据
	* @param	txbuf:	待发送数据首地址
	* @return 发送完成状况
 **/
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
 	//SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV64);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节 
	while(NRF_IRQ!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}

/**
  * @brief  启动NRF24L01接收一次数据
	* @param	rxbuf:	接收数据缓存
	* @return 接收完成状况
 **/
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		    							   
	//SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV64); //spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    
	
/**
  * @brief  该函数初始化NRF24L01到RX模式
	* @note		设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
	*					当CE变高后,即进入RX模式,并可以接收数据了
 **/
void NRF24L01_RX_Mode(void)
{
  NRF_CE(0); 
	//SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV8);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）  
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF_CE(1); //CE为高,进入接收模式 
}						 
 
/**
  * @brief  该函数初始化NRF24L01到TX模式
	* @note		设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
	*					PWR_UP,CRC使能，当CE变高后,即进入RX模式,并可以接收数据了，CE为高大于10us,则启动发送。
 **/
void NRF24L01_TX_Mode(void)
{														 
	NRF_CE(0);	
	//SPI_SetSpeed(NRF_SPIx, LL_SPI_BAUDRATEPRESCALER_DIV8);//spi速度为10.5Mhz（24L01的最大SPI时钟为10Mhz）  
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF_CE(1);//CE为高,10us后启动发送
}
