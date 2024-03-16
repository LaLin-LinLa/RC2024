#include "drv_sram.h"
#include "drv_LCD.h"

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	


/**
	* @brief  在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
	* @param	pBuffer:字节指针
	* @param	WriteAddr:要写入的地址
	* @param	n:要写入的字节数
	* @retval None
	*/
void FSMC_SRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

/**
	* @brief  在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
	* @param	pBuffer:字节指针
	* @param	ReadAddr:要读出的起始地址
	* @param	n:要读出的字节数
	* @retval None
	*/
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);
		ReadAddr++;
	}
} 


void FSMC_SRAM_Test(uint16_t x,uint16_t y)
{
    uint8_t writeData = 0xf0, readData;
    uint16_t cap=0;
    uint32_t addr;  
    addr = 1024;    //从1KB位置开始计算    
    LCD_ShowString(x,y,239,y+16,16,"ExSRAM Cap:   0KB"); 

    while(1){
        FSMC_SRAM_WriteBuffer(&writeData, addr, 1);
        FSMC_SRAM_ReadBuffer(&readData,addr,1);

        if(readData == writeData){  //检查读出的数据是否与写入的数据一样
            cap++;      //如果相同表示写入/读出成功，容量加1（单位KB）
            addr += 1024;   //地址加1024
            readData = 0;
            if(addr > 256 * 1024)   //IS62WV12816的容量为256KB
                break;   
        }
        else
            break;  
    }
    LCD_ShowxNum(x+11*8,y,cap,4,16,0);  //LCD上显示内存容量
//    printf("SRAM Menmory Size:%dKB\r\n",cap);
}
