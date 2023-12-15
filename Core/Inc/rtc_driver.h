  
#define HT1381_SECOND_ADDR					0x80
#define HT1381_MIN_ADDR							0x82
#define HT1381_HOUR_ADDR						0x84
#define HT1381_DATE_ADDR						0x86
#define HT1381_MONTH_ADDR						0x88
#define HT1381_DAY_ADDR							0x8A
#define HT1381_YEAR_ADDR						0x8C
#define HT1381_PROT_ADDR						0x8E
#define HT1381_BUST_WRITE_ADDR			0xBE
#define HT1381_BUST_READ_ADDR				0xBF


#define HT1381_SCL_PIN							GPIO_PIN_0
#define HT1381_IO_PIN								GPIO_PIN_1
#define HT1381_RST_PIN							GPIO_PIN_2
#define HT1381_PORT									GPIOD

#define HT1381_SCL_HIGH							HAL_GPIO_WritePin(HT1381_PORT, HT1381_SCL_PIN, GPIO_PIN_SET)
#define HT1381_SCL_LOW							HAL_GPIO_WritePin(HT1381_PORT, HT1381_SCL_PIN, GPIO_PIN_RESET)
#define HT1381_IO_HIGH							HAL_GPIO_WritePin(HT1381_PORT, HT1381_IO_PIN, GPIO_PIN_SET)
#define HT1381_IO_LOW								HAL_GPIO_WritePin(HT1381_PORT, HT1381_IO_PIN, GPIO_PIN_RESET)
#define HT1381_RST_HIGH							HAL_GPIO_WritePin(HT1381_PORT, HT1381_RST_PIN, GPIO_PIN_SET)
#define HT1381_RST_LOW							HAL_GPIO_WritePin(HT1381_PORT, HT1381_RST_PIN, GPIO_PIN_RESET)
#define HT1381_READ_IO							HAL_GPIO_ReadPin(HT1381_PORT, HT1381_IO_PIN)

#define HT1381_DELAY								1

#define HT1381_TIME_10_YEAR					0
#define HT1381_TIME_1_YEAR					1
#define HT1381_TIME_MONTH						2
#define HT1381_TIME_DAY							3
#define HT1381_TIME_HOUR						4
#define HT1381_TIME_MIN							5
#define HT1381_TIME_SEC							6

typedef struct {
	uint8_t		cmd;	// Control Reg.
	uint16_t	yea;	// Year   
	uint16_t	mon;	// Month  
	uint16_t	day;	// Date   
	uint16_t	hou;	// Hour   
	uint16_t	min;	// Minute 
	uint16_t	sec;	// Second 
	uint16_t	msec;	// MilliSecond 
	//uint32_t	TimeCount;	 // 시간계산용 
} RTCDATA_t, *pRTCDATA_t;

extern RTCDATA_t HRtcData;
extern uint16_t YEAR_1000;

void HT1381_GetDataTimeMsec(void);
void HT1381_SetTime(void);
uint8_t RTC_TimeGetSet(uint8_t argc, char **argv);

void SDA_IN_mode(void);
void SDA_OUT_mode(void);
void Get_HT1381(void);
void HT_Send16bits(uint8_t data1, uint8_t data2);
uint8_t HT_Read8bits(uint8_t adr);


