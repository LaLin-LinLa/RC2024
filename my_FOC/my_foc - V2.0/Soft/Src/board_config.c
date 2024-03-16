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
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);	//�жϷ���4:0
	bsp_led_init();
//	bsp_usart0_init();
	bsp_adc0_1_init();
	bsp_pwm_init();
	bsp_spi0_init();
}

/**
  * @brief  LED��ʼ��
  * @retval ��
  */
static void bsp_led_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
	gpio_bit_reset(GPIOB, GPIO_PIN_2);
}
/**
  * @brief  ADC0_1��ʼ��
  * @retval ��
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
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);	// VBUS			ADC0ע��ͨ��0�ⲿ����T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);	// IB				ADC0ע��ͨ��1�ⲿ����T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);	// IC				ADC1ע��ͨ��2�ⲿ����T0_CH3
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);	// M0_Temp	ADC0����ͨ��3�������

	adc_mode_config(ADC_DAUL_INSERTED_PARALLEL);											//ADC������ע�벢��ģʽ
	/*ADC0ģʽ����*/
	adc_deinit(ADC0);
	adc_dma_mode_enable(ADC0);
	adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);		//����ADC0����ģʽ
	adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);					//����ADC0ɨ��ģʽ
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);							//ADC0���ݶ��뷽ʽ��LSB
	/*ADC0ͨ������*/
	adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);											//����ADC0������ͨ������Ϊ1
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_7POINT5);		//����ADC0������0ͨ��3����ʱ��Ϊ7.5������
	adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);											//����ADC0ע����ͨ������Ϊ2
	adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_7POINT5);	//����ADC0ע����0ͨ��0����ʱ��Ϊ7.5������
	adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);	//����ADC0ע����1ͨ��1����ʱ��Ϊ7.5������
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);	//ADC0�������ⲿ����Ϊ�������
	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);															//ADC0�������ⲿ����ʹ��	
	adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T0_CH3);	//ADC0ע�����ⲿ����CH3�¼�
	adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);
	/*ADC1ģʽ����*/
	adc_deinit(ADC1);
  adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);							//ADC1���ݶ��뷽ʽ��LSB
	/*ADC1ͨ������*/
	adc_channel_length_config(ADC1, ADC_INSERTED_CHANNEL, 1);																			//����ADC1ע����ͨ������Ϊ1
	adc_inserted_channel_config(ADC1, 0, ADC_CHANNEL_2, ADC_SAMPLETIME_7POINT5);									//����ADC1ע����0ͨ��2����ʱ��Ϊ7.5������
	adc_external_trigger_source_config(ADC1, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T0_CH3);	//ADC1ע�����ⲿ����CH3�¼�
	adc_external_trigger_config(ADC1, ADC_INSERTED_CHANNEL, ENABLE);

	/*ADC�ж�*/
	nvic_irq_enable(ADC0_1_IRQn, ADC0_1_NVIC, 0);
	adc_interrupt_enable(ADC0, ADC_INT_EOIC);		//����ע��ת�������ж�
    
  /*DMA_ADC0_CH3*/
	dma_deinit(DMA0, DMA_CH0);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
	dma_init_struct.memory_addr = (uint32_t)(adc_tmp);
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;		/*�����ַ������*/
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;			/*�洢����ַ����*/
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;		
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;  
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.number = 1;
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	dma_init(DMA0, DMA_CH0, &dma_init_struct);
	dma_circulation_enable(DMA0, DMA_CH0);
  dma_channel_enable(DMA0, DMA_CH0);
	 
	/*ADC0ʹ��*/
	// adc_enable(ADC0);
	// adc_calibration_enable(ADC0);		//ADC0��λУ׼
	// adc_dma_mode_enable(ADC0);			//����ADC0 DMA����
	// adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);	//����ADC0����ͨ���������
	/*ADC1ʹ��*/
	// adc_enable(ADC1);
	// adc_calibration_enable(ADC1);		//ADC1��λУ׼
}

/**
  * @brief  PWM��ʼ��
  * @retval ��
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

  /*��ʱ����������*/
	timer_deinit(TIMER0);	
	timer_struct_para_init(&timer_initpara);				
	timer_initpara.prescaler         = 0;														//Ԥ��Ƶֵ
	timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_UP;			//����ģʽ�����Ķ���ģʽ
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;						//�����������ϼ���
	timer_initpara.period            = HALF_PWM_PERIOD_CYCLES-1;			//����
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;						//ʱ�ӷ�Ƶ����
	timer_initpara.repetitioncounter = 0;														//��װ��ֵ
	timer_init(TIMER0, &timer_initpara);	
	/*��ʱ��ͨ������*/
	timer_channel_output_struct_para_init(&timer_ocinitpara);				//��ʼ����ʱ�����ͨ������
  timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;								//��ʱ��ͨ�����״̬������
	timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;							//��ʱ������ͨ�����״̬������
  timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;					//ͨ��������ԣ���
	timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;				//����״̬��ͨ��������ԣ���
  timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;				//����״̬��ͨ���������
	timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;				//����״̬�»���ͨ���������
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);
	timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocinitpara);
	
	/*CH0*/
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);				//���ö�ʱ��ͨ������Ƚ�ֵ
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);					//���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);		//ʹ��ͨ�����Ӱ�ӼĴ���
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
	/*��ʱ��������ֹ����*/
	timer_break_struct_para_init(&timer_breakpara);
	timer_breakpara.runoffstate      = TIMER_ROS_STATE_DISABLE;				//��ֹ����ģʽ�µġ��ر�״̬��
	timer_breakpara.ideloffstate     = TIMER_IOS_STATE_DISABLE;				//��ֹ����ģʽ�µġ��ر�״̬��
	timer_breakpara.deadtime         = 0;															//����ʱ��
	timer_breakpara.breakpolarity    = TIMER_BREAK_POLARITY_LOW;			//��ֹ�źż��ԣ���
	timer_breakpara.outputautostate  = TIMER_OUTAUTO_DISABLE;					//�ر��Զ����
	timer_breakpara.protectmode      = TIMER_CCHP_PROT_OFF;						//�رջ����Ĵ�������
	timer_breakpara.breakstate       = TIMER_BREAK_DISABLE;						//�ر���ֹ
	timer_break_config(TIMER0, &timer_breakpara);

//  timer_primary_output_config(TIMER0, ENABLE);
//  timer_auto_reload_shadow_enable(TIMER0);

	// timer_enable(TIMER0);
}
/**
  * @brief  USART0��ʼ��
  * @retval ��
  */
static void bsp_usart0_init(void)
{
	dma_parameter_struct dma_init_struct = {0};
	
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_GPIOB);
	/*GPIO����*/
	gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
  gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	/*USART0_rx*/
	dma_deinit(DMA0, DMA_CH4);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;			/* ���赽�洢������ */
	dma_init_struct.memory_addr = (uint32_t)Usart0_Rx_Buff;		/* �洢������ַ */
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;	/* �洢����ַ���� */
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;			/* �洢��λ��Ϊ8λ */
	dma_init_struct.number = USART0_RX_LEN;			/* �������ݸ��� */
	dma_init_struct.periph_addr = ((uint32_t)0x40013804);				/* �������ַ����USART���ݼĴ�����ַ */
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;	/* �����ַ�̶����� */
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;	/* ��������λ��Ϊ8λ */
	dma_init_struct.priority = DMA_PRIORITY_HIGH;							/* ������ȼ�Ϊ��*/
	dma_init(DMA0, DMA_CH4, &dma_init_struct);
	dma_circulation_disable(DMA0, DMA_CH4);			              /* DMAѭ��ģʽ���ã���ʹ��ѭ��ģʽ */
	dma_memory_to_memory_disable(DMA0, DMA_CH4);							/* DMA�洢�����洢��ģʽģʽ���ã���ʹ�ô洢�����洢��ģʽ*/
	//nvic_irq_enable(DMA0_Channel2_IRQn, DMA0_CH4_NVI, 0);			/* DMA0 ͨ��4 �ж����ȼ����ò�ʹ�� */
  // dma_interrupt_enable(DMA0, DMA_CH2, DMA_INT_FTF);					/* ʹ�� DMA0 ͨ��4 �������*/
	// dma_interrupt_disable(DMA0, DMA_CH2, DMA_INT_HTF|DMA_INT_ERR);/* �ر� DMA0 ͨ��4 �봫�䡢��������ж� */
  dma_channel_enable(DMA0, DMA_CH4);												 /* ʹ�� DMA0 ͨ��4 */
	
	/*USART0_tx*/
	dma_deinit(DMA0, DMA_CH3);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;			/* �洢�������跽�� */
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;	/* �洢����ַ���� */
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;		/* �洢��λ��Ϊ8λ */
//	dma_init_struct.number = UART0_TX_LEN;						/* �������ݸ��� */
	dma_init_struct.periph_addr = ((uint32_t)0x40013804);					/* �������ַ����USART���ݼĴ�����ַ */
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;	/* �����ַ�̶����� */
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;	/* ��������λ��Ϊ8λ */
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;						/* ������ȼ�Ϊ�е�*/
	dma_init(DMA0, DMA_CH3, &dma_init_struct);
	dma_circulation_disable(DMA0, DMA_CH3);			              /* DMAѭ��ģʽ���ã���ʹ��ѭ��ģʽ */
	dma_memory_to_memory_disable(DMA0, DMA_CH3);							/* DMA�洢�����洢��ģʽģʽ���ã���ʹ�ô洢�����洢��ģʽ*/
	dma_channel_disable(DMA0, DMA_CH3);
	
	nvic_irq_enable(USART0_IRQn, USART0_NVIC, 0);
	usart_interrupt_enable(USART0, USART_INT_IDLE);		//ʹ�ܴ��ڿ����ж�
	
	/*USART0����*/
	usart_deinit(USART0);
	usart_baudrate_set(USART0, USART0_baudrate);
	usart_word_length_set(USART0, USART_WL_8BIT);
  usart_stop_bit_set(USART0, USART_STB_1BIT);	
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);			/* USART DMA ����ʹ�� */
	usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);			/* USART DMA ����ʹ�� */
//	usart_enable(USART0);
}
/**
  * @brief 	SPI0��ʼ��
  * @retval ��
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
