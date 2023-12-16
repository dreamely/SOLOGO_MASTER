
#define	LORA_ADDRESS_0			0x00
#define	LORA_ADDRESS_1			0x10
#define	LORA_ADDRESS_2			0x20
#define	LORA_ADDRESS_3			0x30
#define	LORA_ADDRESS_4			0x40
#define	LORA_ADDRESS_5			0x50
#define	LORA_ADDRESS_6			0x60
#define	LORA_ADDRESS_7			0x70
#define	LORA_ADDRESS_8			0x80
#define	LORA_ADDRESS_9			0x90
#define	LORA_ADDRESS_10			0xA0
#define	LORA_ADDRESS_11			0xB0
#define	LORA_ADDRESS_12			0xC0
#define	LORA_ADDRESS_13			0xD0
#define	LORA_ADDRESS_14			0xE0
#define	LORA_ADDRESS_15			0xF0

#define	LORA_SERIAL_MAX			8
#define	LORA_DEVICE_MAX			16

#define LORA_RECV_MODE			0
#define LORA_TRANS_MODE			1
#define LORA_RECV_ING				2

#define EUI64_SIZE 					8

enum {
	GET_SERIAL = 0,
	CHANGE_TRX,
};

enum {
  MY_UNICAST,
  UNKNOWN_DEVICE = 0xFF,
};


typedef struct {
	uint8_t lora_step;
	uint8_t trx_mode;
	int rssi;
	uint32_t time;
} LORA_MODE;

extern LORA_MODE loraMode;
extern uint8_t loraBindingTable[BINDING_TABLE_SIZE];
extern uint8_t usedBindingCount;

//********************************************************************
void LORA_BuffInit(void);
uint8_t LORA_Reset(uint8_t argc, char **argv);
void LORA_Get_Address(void);
uint8_t LORA_Get_Command (char *sBuf);
void LORA_ServiceFunction(void);
uint8_t LORA_Test(uint8_t argc, char **argv);
uint8_t LORA_DispAddress(uint8_t argc, char **argv);
void LORA_GetSerial(void);
void LORA_TransmitMode(void);
void LORA_ReceiveMode(void);
void LORA_SendRequestTemp(uint8_t index);
void LORA_ChangeConsole(void);
uint8_t LORA_SensorStatus(uint8_t argc, char **argv);
uint8_t LORASET_Terminal_Services(uint8_t argc, char **argv);
uint8_t LORA_MacInput(uint8_t argc, char **argv);
void LORA_CommandStep(void);
uint8_t FindMatchingEui(uint8_t *desteui);

//********************************************************************
extern uint8_t LORA_REG_DEVICE[16][8];

