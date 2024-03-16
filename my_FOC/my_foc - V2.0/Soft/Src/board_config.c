#include "board_config.h"
#include "pwm_cur.h"
#include "usart.h"

static void bsp_led_init(void);
static void bsp_adc0_1_init(void);
static void bsp_pwm_init(void);
static void bsp_usart0_init(void);
static void	bsp_spi0_init(void);

void BSP_Init(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);	//中断分组4:0
	bsp_led_init();
//	bsp_usart0_init();
	bsp_adc0_1_init();
	bsp_pwm_init();
	bsp_spi0_init();
}

/**
  * @brief  LED初始化
  * @retval 无
  */
static void bsp_led_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
	gpio_bit_reset(GPIOB, GPIO_PIN_2);
}
/**
  * @brief  ADC0_1初始化
  * @retval 无
  */
static void bsp_adc0_1_init(void)
{
	dma_parameter_struct dma_init_struct = {0};
	
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_ADC0);
	rcu_periph_clock_enable(RCU_ADC1);
  rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV2);
	rcu_periph_clock_enable(RCU_DMA0);

	/*ADC_CH*/
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);	// VBUS			ADC0注入通道0外部触发T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);	// IB				ADC0注入通道1外部触发T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);	// IC				ADC1注入通道2外部触发T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);	// M0_Temp	ADC0规则通道3软件触发

	adc_mode_config(ADC_DAUL_INSERTED_PARALLEL);											//ADC运行在注入并行模式
	/*ADC0模式配置*/
	adc_deinit(ADC0);
	adc_dma_mode_enable(ADC0);
	adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);		//开启ADC0连续模式
	adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);					//开启ADC0扫描模式
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);							//ADC0数据对齐方式：LSB
	/*ADC0通道配置*/
	adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);											//配置ADC0规则组通道长度为1
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_7POINT5);		//配置ADC0规则组0通道3采样时间为7.5个周期
	adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);											//配置ADC0注入组通道长度为2
	adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_7POINT5);	//配置ADC0注入组0通道0采样时间为7.5个周期
	adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);	//配置ADC0注入组1通道1采样时间为7.5个周期
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);	//ADC0规则组外部触发为软件触发
	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);															//ADC0规则组外部触发使能	
	adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T0_CH3);	//ADC0注入组外部触发CH3事件
	adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);
	/*ADC1模式配置*/
	adc_deinit(ADC1);
  adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);							//ADC1数据对齐方式：LSB
	/*ADC1通道配置*/
	adc_channel_length_config(ADC1, ADC_INSERTED_CHANNEL, 1);																			//配置ADC1注入组通道长度为1
	adc_inserted_channel_config(ADC1, 0, ADC_CHANNEL_2, ADC_SAMPLETIME_7POINT5);									//配置ADC1注入组0通道2采样时间为7.5个周期
	adc_external_trigger_source_config(ADC1, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T0_CH3);	//ADC1注入组外部触发CH3事件
	adc_external_trigger_config(ADC1, ADC_INSERTED_CHANNEL, ENABLE);

	/*ADC中断*/
	nvic_irq_enable(ADC0_1_IRQn, ADC0_1_NVIC, 0);
	adc_interrupt_enable(ADC0, ADC_INT_EOIC);		//开启注入转换结束中断
    
  /*DMA_ADC0_CH3*/
	dma_deinit(DMA0, DMA_CH0);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
	dma_init_struct.memory_addr = (uint32_t)(adc_tmp);
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;		/*外设地址不自增*/
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;			/*存储器地址自增*/
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;		
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;  
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.number = 1;
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	dma_init(DMA0, DMA_CH0, &dma_init_struct);
	dma_circulation_enable(DMA0, DMA_CH0);
  dma_channel_enable(DMA0, DMA_CH0);
	 
	/*ADC0使能*/
	// adc_enable(ADC0);
	// adc_calibration_enable(ADC0);		//ADC0复位校准
	// adc_dma_mode_enable(ADC0);			//开启ADC0 DMA传输
	// adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);	//开启ADC0规则通道软件触发
	/*ADC1使能*/
	// adc_enable(ADC1);
	// adc_calibration_enable(ADC1);		//ADC1复位校准
}

/**
  * @brief  PWM初始化
  * @retval 无
  */
static void bsp_pwm_init(void)
{
  timer_parameter_struct        timer_initpara    = {0};
	timer_oc_parameter_struct     timer_ocinitpara  = {0};
	timer_break_parameter_struct  timer_breakpara   = {0};  

  rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_TIMER0);

  /*PWM_CH*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);		//Timer0_CH0
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);		//Timer0_CH1
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);	//Timer0_CH2
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);	//Timer0_CH0N
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);	//Timer0_CH1N
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);	//Timer0_CH2N

  /*定时器参数配置*/
	timer_deinit(TIMER0);	
	timer_struct_para_init(&timer_initpara);				
	timer_initpara.prescaler         = 0;														//预分频值
	timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_UP;			//对齐模式：中心对齐模式
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;						//计数方向：向上计数
	timer_initpara.period            = HALF_PWM_PERIOD_CYCLES-1;			//周期
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;						//时钟分频因子
	timer_initpara.repetitioncounter = 0;														//重装载值
	timer_init(TIMER0, &timer_initpara);	
	/*定时器通道配置*/
	timer_channel_output_struct_para_init(&timer_ocinitpara);				//初始化定时器输出通道参数
  timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;								//定时器通道输出状态：开启
	timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;							//定时器互补通道输出状态：开启
  timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;					//通道输出极性：高
	timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;				//空闲状态下通道输出极性：高
  timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;				//空闲状态下通道输出：低
	timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;				//空闲状态下互补通道输出：低
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocinitpara);
	
	/*CH0*/
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);				//配置定时器通道输出比较值
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);					//配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);		//使能通道输出影子寄存器
	/*CH1*/
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 0);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_ENABLE);
	/*CH2*/
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 0);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE);
	/*CH3*/
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3,(HALF_PWM_PERIOD_CYCLES-5));
	timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);
	/*定时器死区终止配置*/
	timer_break_struct_para_init(&timer_breakpara);
	timer_breakpara.runoffstate      = TIMER_ROS_STATE_DISABLE;				//禁止运行模式下的“关闭状态”
	timer_breakpara.ideloffstate     = TIMER_IOS_STATE_DISABLE;				//禁止空闲模式下的“关闭状态”
	timer_breakpara.deadtime         = 0;															//死区时间
	timer_breakpara.breakpolarity    = TIMER_BREAK_POLARITY_LOW;			//终止信号极性：低
	timer_breakpara.outputautostate  = TIMER_OUTAUTO_DISABLE;					//关闭自动输出
	timer_breakpara.protectmode      = TIMER_CCHP_PROT_OFF;						//关闭互补寄存器保护
	timer_breakpara.breakstate       = TIMER_BREAK_DISABLE;						//关闭终止
	timer_break_config(TIMER0, &timer_breakpara);

//  timer_primary_output_config(TIMER0, ENABLE);
//  timer_auto_reload_shadow_enable(TIMER0);

	// timer_enable(TIMER0);
}
/**
  * @brief  USART0初始化
  * @retval 无
  */
static void bsp_usart0_init(void)
{
	dma_parameter_struct dma_init_struct = {0};
	
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_GPIOB);
	/*GPIO配置*/
	gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
  gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	/*USART0_rx*/
	dma_deinit(DMA0, DMA_CH4);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;			/* 外设到存储器方向 */
	dma_init_struct.memory_addr = (uint32_t)Usart0_Rx_Buff;		/* 存储器基地址 */
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;	/* 存储器地址自增 */
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;			/* 存储器位宽为8位 */
	dma_init_struct.number = USART0_RX_LEN;			/* 传输数据个数 */
	dma_init_struct.periph_addr = ((uint32_t)0x40013804);				/* 外设基地址，即USART数据寄存器地址 */
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;	/* 外设地址固定不变 */
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;	/* 外设数据位宽为8位 */
	dma_init_struct.priority = DMA_PRIORITY_HIGH;							/* 软件优先级为高*/
	dma_init(DMA0, DMA_CH4, &dma_init_struct);
	dma_circulation_disable(DMA0, DMA_CH4);			              /* DMA循环模式配置，不使用循环模式 */
	dma_memory_to_memory_disable(DMA0, DMA_CH4);							/* DMA存储器到存储器模式模式配置，不使用存储器到存储器模式*/
	//nvic_irq_enable(DMA0_Channel2_IRQn, DMA0_CH4_NVI, 0);			/* DMA0 通道4 中断优先级设置并使能 */
  // dma_interrupt_enable(DMA0, DMA_CH2, DMA_INT_FTF);					/* 使能 DMA0 通道4 传输完成*/
	// dma_interrupt_disable(DMA0, DMA_CH2, DMA_INT_HTF|DMA_INT_ERR);/* 关闭 DMA0 通道4 半传输、传输错误中断 */
  dma_channel_enable(DMA0, DMA_CH4);												 /* 使能 DMA0 通道4 */
	
	/*USART0_tx*/
	dma_deinit(DMA0, DMA_CH3);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;			/* 存储器到外设方向 */
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;	/* 存储器地址自增 */
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;		/* 存储器位宽为8位 */
//	dma_init_struct.number = UART0_TX_LEN;						/* 传输数据个数 */
	dma_init_struct.periph_addr = ((uint32_t)0x40013804);					/* 外设基地址，即USART数据寄存器地址 */
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;	/* 外设地址固定不变 */
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;	/* 外设数据位宽为8位 */
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;						/* 软件优先级为中等*/
	dma_init(DMA0, DMA_CH3, &dma_init_struct);
	dma_circulation_disable(DMA0, DMA_CH3);			              /* DMA循环模式配置，不使用循环模式 */
	dma_memory_to_memory_disable(DMA0, DMA_CH3);							/* DMA存储器到存储器模式模式配置，不使用存储器到存储器模式*/
	dma_channel_disable(DMA0, DMA_CH3);
	
	nvic_irq_enable(USART0_IRQn, USART0_NVIC, 0);
	usart_interrupt_enable(USART0, USART_INT_IDLE);		//使能串口空闲中断
	
	/*USART0配置*/
	usart_deinit(USART0);
	usart_baudrate_set(USART0, USART0_baudrate);
	usart_word_length_set(USART0, USART_WL_8BIT);
  usart_stop_bit_set(USART0, USART_STB_1BIT);	
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);			/* USART DMA 接收使能 */
	usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);			/* USART DMA 发送使能 */
//	usart_enable(USART0);
}
/**
  * @brief 	SPI0初始化
  * @retval 无
  */
static void	bsp_spi0_init(void)
{
	rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);			//PA4/ENC_CS
	gpio_bit_set(GPIOA, GPIO_PIN_4);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);	//ENC_SCK/PA5, ENC_MOSI/PA7
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);		//ENC_MISO/PA6
	spi_parameter_struct spi_init_struct = {0};
	spi_i2s_deinit(SPI0);
  spi_struct_para_init(&spi_init_struct);

	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_8;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI0, &spi_init_struct);
	spi_enable(SPI0);	
	
}
