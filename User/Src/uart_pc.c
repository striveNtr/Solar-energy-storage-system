#include "uart_pc.h"
void uart_pc_func(void){

	if(recv_end_flag == 1)  //接收完成标志
	{
		//DMA_Usart_Send(rx_buffer, rx_len);
		//memset(rx_buffer,0,rx_len);
		//rx_len = 0;//清除计数
		recv_end_flag = 0;//清除接收结束标志位
	//			for(uint8_t i=0;i<rx_len;i++)
	//				{
	//					rx_buffer[i]=0;//清接收缓存
	//				}
		HAL_UART_Receive_DMA(&huart2,rx_buffer,BUFFER_SIZE);//重新打开DMA接收z
	}
}
