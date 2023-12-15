
#define NULL_BINDING											0
#define SINK_BINDING_INDEX								0
#define BINDING_TABLE_SIZE								16
#define BINDING_INDEX_OUT_OF_RANGE				0xFF

#define SEND_DATA_RATE										20		//5 sec / 0.25 = 20	
//#define SEND_DATA_RATE										4		// 1 sec / 0.25 = 4
#define MISSCOUNT_240_SEC									240

extern uint16_t ticksSinceLastHeard[BINDING_TABLE_SIZE];
extern uint8_t sendDataCountdown;

void applicationTick(void);
uint8_t loraGetBinding(uint8_t index);



