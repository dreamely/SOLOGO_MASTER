#include "main.h"



uint16_t ticksSinceLastHeard[BINDING_TABLE_SIZE];

//********************************************************************
// ���� ���ø����̼� ƽ 
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
		//�� 1�ʸ��� ��ϵ� ���ε� üũ, 5��*16 = 80��*3= 240��
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

					//���� Ÿ���� RTD =2, �� ��� ������ ���� �� ������ ������ �׾��ٰ� �Ǵ�
					//Printf_("index %d, type %02X\r\n", i, ARRAY_TEMP.responseTemp[i].type);
				}
			}
		}
	}

	LORA_CommandStep();

}


//********************************************************************
// ��ϵ� ���ε� ���̺� üũ, ����+��ġ���� ��Ͻ�
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
		//���� ���۳����� 0 ���� ä���� �ִٸ� ��ϵ��� ����
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


