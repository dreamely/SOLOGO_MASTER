
/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10) 
#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10) 

uint8_t Settimebuf[7],Gettimebuf[7];

RTCDATA_t HRtcData;
uint16_t YEAR_1000;


/* BIT BANGING SERIAL COM */
void HT_Send16bits(uint8_t data1, uint8_t data2)
{
	uint8_t i;
	
	SDA_OUT_mode();

	data1 = data1 | 0x80;
	data1 = data1 & 0x8E;
	
  HT1381_RST_HIGH;

	HAL_3usDelay(HT1381_DELAY); 

	for(i=0;i<8;i++)
	{
		HT1381_SCL_LOW;
		if(data1 & 1){ HT1381_IO_HIGH;} else {HT1381_IO_LOW;}
		data1>>=1;

		HAL_3usDelay(HT1381_DELAY); 

		HT1381_SCL_HIGH;
		HAL_3usDelay(HT1381_DELAY); 

	}

	for(i=0;i<8;i++)
	{
		HT1381_SCL_LOW;
		if(data2 & 1){ HT1381_IO_HIGH;} else {HT1381_IO_LOW;}
		data2>>=1;

		HAL_3usDelay(HT1381_DELAY); 
		HT1381_SCL_HIGH;
		HAL_3usDelay(HT1381_DELAY); 

	}
	HT1381_SCL_LOW;
  HT1381_RST_LOW;
	//SDA_SCL_IN;
	SDA_IN_mode();
}

//void HT_SetTime(uint8_t *pSecDa) 

uint8_t HT_Read8bits(uint8_t adr)
{
	uint8_t i,a=0;
	
	SDA_OUT_mode();

	HT1381_SCL_LOW; 
	adr=adr | 0x81;
	adr=adr & 0x8F;
	HT1381_RST_HIGH;
	for(i=0;i<8;i++)
	{ 
		HT1381_SCL_LOW;
		if(adr & 1) { HT1381_IO_HIGH;} else {HT1381_IO_LOW;}
		adr>>=1;
		HT1381_SCL_HIGH;
	}

	SDA_IN_mode();
	a=0;
	for(i=0;i<7;i++)
	{
		HT1381_SCL_LOW;
		HAL_3usDelay(HT1381_DELAY); 
		HAL_3usDelay(HT1381_DELAY); 
		HT1381_SCL_HIGH;
		if(HT1381_READ_IO) { a|=128; };
		a>>=1;
	}
	HT1381_SCL_LOW;
	HAL_3usDelay(HT1381_DELAY); 
	HAL_3usDelay(HT1381_DELAY); 
	HT1381_SCL_HIGH;

	if(HT1381_READ_IO) { a|=128; };	
	HT1381_RST_LOW;
	//SDA_SCL_IN;
	
	return a;
}

void SDA_IN_mode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = HT1381_IO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(HT1381_PORT, &GPIO_InitStruct);
}

void SDA_OUT_mode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = HT1381_IO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(HT1381_PORT, &GPIO_InitStruct);
}

void Get_HT1381(void) 
{
  uint8_t i;
  uint8_t Addr = 0x81; //Read add is last bit 0x01

  for (i=0;i<7;i++)
  {
    Gettimebuf[i] = HT_Read8bits(Addr);
    Addr += 2;
  }
} 


void HT1381_GetDataTimeMsec(void)
{
	Get_HT1381();

	HRtcData.sec = BCD2BIN(Gettimebuf[0]);
	HRtcData.min = BCD2BIN(Gettimebuf[1]);
	HRtcData.hou = BCD2BIN(Gettimebuf[2]);
	HRtcData.day = BCD2BIN(Gettimebuf[3]);
	HRtcData.mon = BCD2BIN(Gettimebuf[4]);
	//HRtcData.yea = BCD2BIN(Gettimebuf[6]) + 2000;

	HRtcData.yea = BCD2BIN(Gettimebuf[6]) + YEAR_1000 * 100;

	HRtcData.msec = microGetSystemTime();

	//vPrintf(UART1_SERIAL, "TIME : %02d-%02d-%02d %02d:%02d:%02d.%03d \n", 
	//	pRtcData->yea, pRtcData->mon , pRtcData->day  ,
	//    pRtcData->hou , pRtcData->min , pRtcData->sec, pRtcData->msec);

}

void HT1381_SetTime(void)
{
	uint8_t high, low;

	high = (HRtcData.yea / 10) << 4;
	low  = (HRtcData.yea % 10);

	//high = (HRtcData.yea / 100) << 4;
	//low  = (HRtcData.yea % 100);

	Settimebuf[0] = high|low; //년

	//Printf_("year %d, 10y %d setbuf %02X high %d, low %d\r\n", 
	//		HRtcData.yea, HRtcData.yea / 100, Settimebuf[0], high, low);


	high = (HRtcData.mon / 10) << 4;
	low  = (HRtcData.mon % 10);
	Settimebuf[1] = high|low; //월

	high = (HRtcData.day / 10) << 4;
	low  = (HRtcData.day % 10);
	Settimebuf[2] = high|low; //일

	high = (HRtcData.hou / 10) << 4;
	low  = (HRtcData.hou % 10);
	Settimebuf[3] = high|low; //시 

	high = (HRtcData.min / 10) << 4;
	low  = (HRtcData.min % 10);
	Settimebuf[4] = high|low; //분

	high = (HRtcData.sec / 10) << 4;
	low  = (HRtcData.sec % 10);
	Settimebuf[5] = high|low; //초

	HT_Send16bits(0x8E,0x00);	// write enable
	HAL_3usDelay(HT1381_DELAY); 

	HT_Send16bits(HT1381_SECOND_ADDR,Settimebuf[5]);
	HT_Send16bits(HT1381_MIN_ADDR,Settimebuf[4]);
	HT_Send16bits(HT1381_HOUR_ADDR,Settimebuf[3]);
	HT_Send16bits(HT1381_DATE_ADDR,Settimebuf[2]);
	HT_Send16bits(HT1381_MONTH_ADDR,Settimebuf[1]);
	HT_Send16bits(HT1381_YEAR_ADDR,Settimebuf[0]);

	HAL_3usDelay(HT1381_DELAY); 

	HT_Send16bits(0x8E,0x01); // write disable
	HAL_3usDelay(HT1381_DELAY); 

}

uint8_t RTC_TimeGetSet(uint8_t argc, char **argv)
{
	RTCDATA_t RtcData;
	uint8_t i;
	int	Data, buf[6];


	Printf_("Argument (%d) \n", argc);

  switch( argc )  {

		case  1 : // 명령어만  주어 졌을때 출력 (표시)						
			HT1381_GetDataTimeMsec();
			//HT_send16bits(0x8E,0x00); // write enable

			Printf_("TIME : %02d-%02d-%02d %02d:%02d:%02d.%03d \n", 
				HRtcData.yea, HRtcData.mon, HRtcData.day,
					HRtcData.hou, HRtcData.min, HRtcData.sec, HRtcData.msec);

			break;	

		case  7 :
			for ( i = 1 ; i < 7 ; i++) {
				//Printf_("value[%d] %d\r\n", i, atoi(argv[i]));
			
				if (!my_atoi(argv[i], &Data)) { 		
					//Printf( "Argument 1 Error!\n");
					Printf_("Argument (%d) Error!\n", i+1);
					return FALSE;	/* Return 0 (FALSE)*/
				}				
				buf[i-1] = Data;				
				//Printf_(" %d -> buf[i-1] %d \n", i, buf[i-1] );
			}	
			
			HRtcData.yea  = buf[0];
			HRtcData.mon  = buf[1];
			HRtcData.day  = buf[2];
			HRtcData.hou  = buf[3];
			HRtcData.min  = buf[4];
			HRtcData.sec  = buf[5];

	  	Printf_("REV TIME -> %02d-%02d-%02d %02d:%02d:%02d \n", 
				buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);			
			
			//한번에 세팅이 안됨
			HT1381_SetTime();		
			HAL_3usDelay(100);
			HT1381_SetTime();		
			break;

	}

	return TRUE;
}


