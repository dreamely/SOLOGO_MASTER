
#define 			READ_ADDRESS								1000
#define 			WRITE_RELAY_ADDRESS					1100
#define				DEVICE_WRITE_ADD						1120
#define 			WRITE_FAULT_ADDRESS					1200

#define 			CMD_REQUEST_TEMP						0x04
#define 			CMD_RESPONSE_TEMP						0x05
#define 			LEN_REQUEST_TEMP						0x0A
#define 			LEN_RESPONSE_TEMP						0x10

#if 0
//데이터 전송시 더미 데이터 안들어가게 하려면 __packed 를 해야 한다 
typedef __packed struct {
  uint8_t   stx0;
  uint8_t   stx1;
  uint8_t   len;
  uint8_t   address;
  uint8_t   cmd;
  uint16_t  cnt_address;
  float     current[SENSOR_NUM];
	uint8_t   relay[SENSOR_NUM];
	uint8_t   xor;
} SLAVE_STATUS;
#else

typedef struct {
  uint8_t   len;
  uint8_t   cmd;
  uint8_t   address[8];
} __attribute__((packed))REQUEST_TEMP;

typedef struct {
  uint16_t  type;			// 1:TC, 2:RTD
  uint16_t  bat_rssi;	// RSSI 0 ~ 255, 배터리 0 ~ 100 %
  int16_t   temp1;		//온도*10
  int16_t   temp2;		//온도*10
} __attribute__((packed))RESPONSE_TEMP;

typedef struct {  
	RESPONSE_TEMP responseTemp[16];
} __attribute__((packed))_ARRAY_TEMP;

extern _ARRAY_TEMP ARRAY_TEMP;

#endif


extern REQUEST_TEMP requestTemp;
//extern RESPONSE_TEMP responseTemp[16];


void ResponseStatus(void);
//uint8_t AT_command(uint8_t argc, char **argv);


