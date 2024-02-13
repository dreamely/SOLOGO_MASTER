#include "main.h"



uint16_t ticksSinceLastHeard[BINDING_TABLE_SIZE];

//********************************************************************
// 메인 어플리케이션 틱 
//********************************************************************
void applicationTick(void)
{
	uint32_t time;
	static uint32_t lastBlinkTime = 0;
	static uint8_t indexCheckCount = 0;

	uint8_t i;

	time = microGetSystemTime();

	if(time - lastBlinkTime >= TIME_250_MS) {

		lastBlinkTime = time;

		HeartBeatToggle();

		//vPrintf(UART2_SERIAL, "time %d\r\n", time);

		//*************************************************************************
		//매 1초마다 등록된 바인딩 체크, 5초*16 = 80초*3= 240초
		if(indexCheckCount++ > 3) {
			indexCheckCount = 0;

			for (i=0; i<BINDING_TABLE_SIZE; i++) {
				if(ARRAY_TEMP.responseTemp[i].type != 0) {
					if (ticksSinceLastHeard[i] != MISSCOUNT_240_SEC) {

						if (++ticksSinceLastHeard[i] >= MISSCOUNT_240_SEC) {
							Printf_("EVENT STATUS : too long since last heard, [%d]\r\n", i);
							ticksSinceLastHeard[i] = MISSCOUNT_240_SEC;
							memset(&ARRAY_TEMP.responseTemp[i], 0, sizeof(ARRAY_TEMP.responseTemp[i]));
						}
					}

					//만약 타입이 RTD =2, 를 벗어나 쓰레기 값이 들어가 있으면 센서가 죽었다고 판단
					//Printf_("index %d, type %02X\r\n", i, ARRAY_TEMP.responseTemp[i].type);
				}
			}
		}
	}

	LORA_CommandStep();

}


//********************************************************************
// 등록된 바인딩 테이블 체크, 최초+터치에서 등록시
//********************************************************************
uint8_t loraGetBinding(uint8_t index)
{
	uint8_t i, j, k;

	usedBindingCount = 0;
	memset(loraBindingTable, 0, sizeof(loraBindingTable));

  if (index > BINDING_TABLE_SIZE) {
    return BINDING_INDEX_OUT_OF_RANGE;
	}

	k = 0;

	for(j=0; j<index; j++) {
		//만약 버퍼내용이 0 으로 채워져 있다면 등록되지 않음
		for(i=0; i<8; i++) {
			if(LORA_REG_DEVICE[j][i] != 0) {
				usedBindingCount++;
				loraBindingTable[k++] = j;
				break;
			}
		}
	}

	Printf_("Alive count %d, Binding index : ", usedBindingCount);
	for(i=0; i<usedBindingCount; i++) {
		Printf_("%d ", loraBindingTable[i]);
	}
	Printf_("\r\n");

  
  return TRUE;
} 


