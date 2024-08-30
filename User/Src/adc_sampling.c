#include "adc_sampling.h"
//ch(1-6Chnnal),times(读取次数)
uint32_t ADC_Get_Average(uint8_t ch,uint8_t times)
{
	ADC_ChannelConfTypeDef sConfig;		//通道初始化
	uint32_t value_sum=0;
	uint8_t i;
	switch(ch)							//选择ADC通道
	{
		case 0:sConfig.Channel = ADC_CHANNEL_0;break;
		case 1:sConfig.Channel = ADC_CHANNEL_1;break;
		case 2:sConfig.Channel = ADC_CHANNEL_2;break;
		case 3:sConfig.Channel = ADC_CHANNEL_3;break;
		case 4:sConfig.Channel = ADC_CHANNEL_4;break;
		case 5:sConfig.Channel = ADC_CHANNEL_5;break;
		case 6:sConfig.Channel = ADC_CHANNEL_6;break;
	}
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;		//采用周期239.5周期
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(&hadc1,&sConfig);
	for(i=0;i<times;i++)
	{
		HAL_ADC_Start(&hadc1);								//启动转换
		HAL_ADC_PollForConversion(&hadc1,30);				//等待转化结束
		value_sum += HAL_ADC_GetValue(&hadc1);				//求和
		HAL_ADC_Stop(&hadc1);								//停止转换
	}
	return value_sum/times;									//返回平均值
}

