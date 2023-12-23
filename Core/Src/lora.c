
#include "main.h"

LORA_MODE loraMode;
uint8_t cmdStep = 0;
uint8_t loraBindingTable[BINDING_TABLE_SIZE];
uint8_t usedBindingCount;
uint8_t sendDataCountdown = SEND_DATA_RATE;

//****************************************************************************************
// 8����Ʈ 16�� ���� ���
//****************************************************************************************
uint8_t LORA_REG_DEVICE[16][8];
//****************************************************************************************
uint8_t status;
uint8_t switch_trx_mode = LORA_TRANS_MODE;

//********************************************************************************************
char LORA_GET_SERIAL[] = "AT+SN=";
char LORA_STATUS_OK[] = "OK";
char LORA_TX_OK1[] = "+EVT";
char LORA_TX_OK2[] = "TXP2P";
char LORA_TX_OK3[] = "DONE";

char LORA_RX_OK1[] = "RXP2P";
char LORA_RX_OK2[] = "RECEIVE";
char LORA_RX_OK3[] = "TIMEOUT";


//****************************************************************************************
// ��ϵ� ����̽� �������� 
//****************************************************************************************
uint8_t LORA_Reset(uint8_t argc, char **argv)
{
	//vPrintf(UART3_SERIAL, "ATZ\r\n");
	HAL_GPIO_WritePin(LORA_RESET_GPIO_Port, LORA_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LORA_RESET_GPIO_Port, LORA_RESET_Pin, GPIO_PIN_SET);

	return TRUE;
}

//****************************************************************************************
// ���� �ʱ�ȭ 
//****************************************************************************************
void LORA_BuffInit(void)
{
	uint8_t i;

	memset(LORA_REG_DEVICE, 0, sizeof(LORA_REG_DEVICE));
	memset(&ARRAY_TEMP, 0, sizeof(ARRAY_TEMP));

	for (i=0; i<BINDING_TABLE_SIZE; i++) {
		ticksSinceLastHeard[i] = MISSCOUNT_240_SEC;

		//Printf_("ticksSinceLastHeard[%d] = %d\r\n", i, ticksSinceLastHeard[i]);
	}

	LORA_Get_Address();

	LORA_Reset(0,0);

	loraMode.lora_step = CHANGE_TRX;
	loraMode.trx_mode = LORA_TRANS_MODE;

}

//****************************************************************************************
// ��ϵ� ����̽� �������� 
//****************************************************************************************
void LORA_Get_Address(void)
{
	uint8_t i;

	for(i=0; i<16; i++) {
		HAL_WatchDogReload();

		I2C_EEPROM_BLOCK_READ(0, &LORA_REG_DEVICE[i][0], i*8, 8);
	}
}


//********************************************************************************************
// ���� : ���ڿ� �Է��� �޴´�. ( CR�� �Էµɶ����� )
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//********************************************************************************************
uint8_t LORA_Get_Command (char *sBuf)
{
	static uint8_t buf[100];
	static uint8_t i = 0;	    
	uint16_t cmd;  
	uint8_t ch;		

	cmd = UART3_getc();
	if(cmd == 0xFFFF) return FALSE;

	ch = (uint8_t)cmd;

	switch( ch ) {    	

		case LF:		 
		//case ENTER: 	 

			//���� + 1 �� �ؾ� ����ü �Լ� ȣ���ϴ� ������ �����
			//cmd + argv
			memcpy(sBuf, buf, i+1);	
			memset(buf , 0x00, 100);

			//�빮�� ��ȯ,����ü �Լ��� ȣ���Ҷ� ����ü �Լ��� �빮�ڷ� �����س���
			//���⼭ ��ü�� �빮�ڷ� �ٲ�
			//�ܼ� ���񽺴� ����� ������ �ϱ� ���ؼ� ����ϱ� ������
			ChangeUpperChar(sBuf);
			i=0; 
			return TRUE;

		default : 
			if(gMsgLevel & DEBUG_FLAG_0002) {
				Printf_("%c", (char)ch);
			}
			*(buf+i) = (char)ch;
			if ( ++i > 100 ) i = 99;		
			break;
	}     		
	
	//watchdogReload();	

	return FALSE;
}


//********************************************************************************************
//���ο��� �ܼ� �Է� ����
//********************************************************************************************
void LORA_ServiceFunction(void)
{
  uint8_t  fargc;
  char     *fargv[32];
  char     fReadBuffer[64];
  uint8_t  fcmdlp;	

	uint8_t hex_string[100], i, index;
	uint16_t tempData;

  if(LORA_Get_Command(fReadBuffer)) {

		if(gMsgLevel & DEBUG_FLAG_0002) {
			//Printf_( "....\r\n");
			Printf_( "\r\n");
		}

    fargc = Parse_Args( fReadBuffer, fargv );

    //�ַܼ� �ζ� �Է��� ������
		if (fargc) {	
			//fcmdlp ���ڿ� 0 ������ ������� �˻�
			fcmdlp = 0;

			if(gMsgLevel & DEBUG_FLAG_0003) {
				Printf_("argc %d\r\n", fargc);
				for(i=0; i<fargc; i++) {
					Printf_("argv[%d] %s, ", i, fargv[i]);
				}
				Printf_("\r\n");
			}

#if 0
			if(strncmp(fReadBuffer, LORA_GET_SERIAL, 6) == 0) {
				Printf_("Lora get serial number ok\r\n");

				hexstring_conver_hex(&fReadBuffer[6], 16, hex_string);
	
				for(i=0; i<8; i++) {
					Printf_("%02X ", hex_string[i]);
				}
				Printf_("\r\n");
			}
#endif
			//Printf_("fargc %d, loraMode.trx_mode %d\r\n", fargc, loraMode.trx_mode);

			//������ ����
			if(fargc == 3) { 
				//if(loraMode.lora_step == CHANGE_TRX && loraMode.trx_mode == LORA_TRANS_MODE) {
				if(strcmp(fargv[0], LORA_TX_OK1) == 0 && strcmp(fargv[1], LORA_TX_OK2) == 0 
					&& strcmp(fargv[2], LORA_TX_OK3) == 0) {

					//cmdStep = 1;

				}
			}

			//���Ÿ�� Ÿ�Ӿƿ�, �۽� �� �� ���� 
			else if(fargc == 4) {
				if(cmdStep != 1) {
					//Printf_("LORA_RECV_ING\r\n");

					if(strcmp(fargv[0], LORA_TX_OK1) == 0 && strcmp(fargv[1], LORA_RX_OK1) == 0 
						&& strcmp(fargv[2], LORA_RX_OK2) == 0 && strcmp(fargv[3], LORA_RX_OK3) == 0) {
						
						//Printf_("Change TX mode\r\n");
					}
				}
			}
			//������ ����
			else if(fargc == 5) {
				if(strcmp(fargv[0], LORA_TX_OK1) == 0 && strcmp(fargv[1], LORA_RX_OK1) == 0) {
					loraMode.rssi = atoi(fargv[2]);

					//�����ʹ� �� 30 ���� ����
					hexstring_conver_hex(&fargv[4][0], 30, hex_string);

					if(gMsgLevel & DEBUG_FLAG_0003) {
						Printf_("RSSI : %d, ", loraMode.rssi);

						//��簪���� /2 = 15����Ʈ
						for(i=0; i<15; i++) {
							Printf_("%02X ", hex_string[i]);
						}
						Printf_("\r\n");
					}


					if(hex_string[0] == LEN_RESPONSE_TEMP && hex_string[1] == CMD_RESPONSE_TEMP) {
						//��ϵ� EUI �� ��ġ�ϴ��� Ȯ��
						index = FindMatchingEui(&hex_string[2]);

						if(index != UNKNOWN_DEVICE) {
							ARRAY_TEMP.responseTemp[index].type = (uint16_t)hex_string[10];
							memcpy(&ARRAY_TEMP.responseTemp[index].temp1, &hex_string[11], 2);
							memcpy(&ARRAY_TEMP.responseTemp[index].temp2, &hex_string[13], 2);

							//Printf_("1. type = %04X\r\n", ARRAY_TEMP.responseTemp[index].type);

							//��Ʋ��������� ����, ���� �Ƚ�Ŵ
							//�迭 ����ȯ �����Ű�� ��/�� ��������
							//tempData = byteSwap(ARRAY_TEMP.responseTemp[index].type);
							//ARRAY_TEMP.responseTemp[index].type = tempData;

							//Printf_("2. type = %04X\r\n", ARRAY_TEMP.responseTemp[index].type);

							tempData = byteSwap(ARRAY_TEMP.responseTemp[index].temp1);
							ARRAY_TEMP.responseTemp[index].temp1 = tempData;

							tempData = byteSwap(ARRAY_TEMP.responseTemp[index].temp2);
							ARRAY_TEMP.responseTemp[index].temp2 = tempData;

							//���ε� ���̺� �ð� ����
							ticksSinceLastHeard[index] = 0;

							if(gMsgLevel & DEBUG_FLAG_0003) {
								Printf_("Matching EUI index %d\r\n", index);
								Printf_("type %04X, temp1 %04X, temp2 %04X\r\n", 
									ARRAY_TEMP.responseTemp[index].type,
									ARRAY_TEMP.responseTemp[index].temp1, 
									ARRAY_TEMP.responseTemp[index].temp2);
							}
						}

					}

				}
			}

		}

  }
}

//********************************************************************************************
//���ο��� �ܼ� �Է� ����
//********************************************************************************************
void LORA_ChangeConsole(void)
{
	uint16_t cmd;

	cmd = UART1_getc();

	if(cmd != 0xFFFF) {
		vPrintf(UART3_SERIAL, "%c", (uint8_t)cmd);

		ExitTimerSet();
	}
}

//*************************************************************************
//�ֿܼ��� �ζ� �Ķ���� ���� ����
//*************************************************************************
uint8_t LORASET_Terminal_Services(uint8_t argc, char **argv)
{
  uint8_t  fargc;
  char     *fargv[32];
  char     fReadBuffer[64];
  uint8_t  fcmdlp;		
	uint16_t cmd;

 
  //Printf_("LORA Configure Set and View ( ESC KEY : Q ) \r\n");
  Printf_("LORA Configure Set and exit after 1 minute if not input\r\n");
  Printf_("LORA_Config > \r\n");

  ExitTimerSet();

  while(1) {      

		HAL_WatchDogReload();
		UART_RX_DMA_Handler();

		//�ܼ� �Է� ������ �ð� Ȯ��
    if(ExitTimerChekck()) {
			//������ �����ٸ� ������ Ȥ�� ���Ÿ��� �����ؾ� �Ѵ�
      Printf_("LORA Loop exit~~\r\n");
      break;  
    }

		cmd = UART3_getc();
	  //�ܼ� �Է� ������ 5�� ����

		if(cmd != 0xFFFF) {
			Printf_("%c", (uint8_t)cmd);
			ExitTimerSet();
		}

		LORA_ChangeConsole();

  }
  return TRUE;
}

//**************************************************************************************
// �ø���ѹ� �������� 
//**************************************************************************************
void LORA_GetSerial(void)
{
	vPrintf(UART3_SERIAL, "AT+SN=?\r\n");
}

//**************************************************************************************
// �۽Ÿ��� ���� 
//**************************************************************************************
void LORA_TransmitMode(void)
{
	vPrintf(UART3_SERIAL, "AT+PRECV=0\r\n");
}

//**************************************************************************************
// ���Ÿ��� ���� 4�ʴ��
//**************************************************************************************
void LORA_ReceiveMode(void)
{
	//if(gMsgLevel & DEBUG_FLAG_0004) {
	//	Printf_("Lora chnage rev mode for 4 second\r\n");
	//}
	//HAL_Delay(100);

	vPrintf(UART3_SERIAL, "AT+PRECV=4000\r\n");
}


//**************************************************************************************
// �µ����� ��û
//**************************************************************************************
void LORA_SendRequestTemp(uint8_t index)
{
	uint8_t i, string_len;
	uint8_t *arrayData, status;
	uint8_t hex_string[20];

	char send_string[] = {
		'A','T','+','P','S','E','N','D','=',
		'0','0', //len
		'0','0', //cmd
		'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',
		'\r','\n'};


	uint32_t time;

	requestTemp.len = LEN_REQUEST_TEMP;
	requestTemp.cmd = CMD_REQUEST_TEMP;

	memcpy(&requestTemp.address, &LORA_REG_DEVICE[index][0], 8);

	//for(i=0; i<sizeof(requestTemp.address); i++) {
	//	Printf_("%02X",requestTemp.address[i]);
	//}
	//Printf_("\r\n");

	//�����Ͱ� �����ϰ� 
	arrayData = (void *)&requestTemp;

	//1����Ʈ ��縦 2���� ���ڿ��� ��ȯ�ϱ� ������ �����ҹ��� x 2
	string_len = hex_convert_hexstring(arrayData, 10*2, hex_string);

	//Printf_("AT+PSEND=%s\r\n", hex_string);

	memcpy(&send_string[9], hex_string, 10*2);

	if(gMsgLevel & DEBUG_FLAG_0003) {
		for(i=0; i<sizeof(send_string); i++)
			Printf_("%c",send_string[i]);
	}

	for(i=0; i<sizeof(send_string); i++)
		vPrintf(UART3_SERIAL,"%c",send_string[i]);


}


//**************************************************************************************
// �ζ� �׽�Ʈ
//**************************************************************************************
uint8_t LORA_Test(uint8_t argc, char **argv)
{
	uint16_t value;

	if(argc == 2) {

		value = atoi(argv[1]);

		if(value == 0) {
			Printf_("Get lora serial number\r\n");
			LORA_GetSerial();
		}
		//�۽Ÿ��� ���� 
		else if(value == 1) {
			Printf_("Change lora transmit mode\r\n");
			LORA_TransmitMode();
		}
		//������ �۽�
		else if(value == 2) {
			Printf_("Send lora request temp\r\n");
			LORA_SendRequestTemp(0);
		}

	}

	return TRUE;
}

//**************************************************************************************
// �ζ� �׽�Ʈ
//**************************************************************************************
uint8_t LORA_MacInput(uint8_t argc, char **argv)
{
	uint8_t len, msg[8], index;

	if(argc < 3) return FALSE;

	//********************************************************
	//�ڽ��� MAC ����
	//********************************************************
	len = strlen(argv[2]);
	Printf_("len %d\r\n", len);

	if(len != 16) {
		Printf_("Data len is not 16\r\n");
		return FALSE;
	}

	hexstring_conver_hex(argv[2], 16, &msg);
	index = atoi(argv[1]) * 8;

	Printf_(" Index %d, MAC : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", index,
		msg[0], msg[1], msg[2], msg[3], msg[4], msg[5],
		msg[6], msg[7]);		

	I2C_EEPROM_BLOCK_WRITE(0, &msg, index, 8);

	//������ ������ EEPROM �ٽ� �о�;� �Ѵ�
	LORA_Get_Address();

	loraGetBinding(BINDING_TABLE_SIZE);

	return TRUE;
}


//**************************************************************************************
// �ζ� �ø��� ��ȣ
//**************************************************************************************
uint8_t LORA_DispAddress(uint8_t argc, char **argv)
{
	uint16_t i,j;

	LORA_Get_Address();

	Printf_("*************************************************************\r\n");
	Printf_(" INDEX     SERIAL NUMBER          INUSE   TIME\r\n");
	Printf_("*************************************************************\r\n");

	for(i=0; i<LORA_DEVICE_MAX; i++) {

		Printf_("[%02d]  : ", i);

		for(j=0; j<LORA_SERIAL_MAX; j++) {
			Printf_("%02X ", LORA_REG_DEVICE[i][j]);
		}

		if(ticksSinceLastHeard[i] != MISSCOUNT_240_SEC) {
			Printf_("  TRUE ");
			Printf_("     %03d\r\n", ticksSinceLastHeard[i]);
		}
		else {
			Printf_("  FALSE");
			Printf_("    FFFF\r\n");
		}

		//Printf_("\r\n");
	}
	Printf_("*************************************************************\r\n");

	return TRUE;
}

//**************************************************************************************
// �ζ� ���� ����
//**************************************************************************************
uint8_t LORA_SensorStatus(uint8_t argc, char **argv)
{
	uint16_t i,j;


	Printf_("*************************************************************\r\n");
	Printf_(" INDEX  TYPE  TEMP1   TEMP2\r\n");
	Printf_("*************************************************************\r\n");

	for(i=0; i<LORA_DEVICE_MAX; i++) {
		Printf_("  %02d", i);
		//���� �����Ǿ� ����
		if(ARRAY_TEMP.responseTemp[i].type == 1) {
			Printf_("     %02d", 0x01);
		}
		else if(ARRAY_TEMP.responseTemp[i].type == 2) {
			Printf_("     %02d", 0x02);
		}
		else {
			Printf_("     %02d", 0x00);
		}

		//Printf_("     %02d", ARRAY_TEMP.responseTemp[i].type);
		Printf_("    %d.%d", ARRAY_TEMP.responseTemp[i].temp1/10, ARRAY_TEMP.responseTemp[i].temp1%10);
		Printf_("     %d.%d", ARRAY_TEMP.responseTemp[i].temp2/10, ARRAY_TEMP.responseTemp[i].temp2%10);
		Printf_("\r\n");
	}

	Printf_("*************************************************************\r\n");

	return TRUE;
}

//**************************************************************************************
// ��ġ�ϴ� EUI �˻�
//**************************************************************************************
uint8_t FindMatchingEui(uint8_t *desteui)
{
	uint8_t i, j, temp;
	
	temp = FALSE;

	for(j=0; j<16; j++) {
		if(memcmp(&LORA_REG_DEVICE[j][i], desteui, EUI64_SIZE) == 0) {	
			//Printf_("Same eui\r\n");	
			temp = TRUE;	
			break;			
		} 		
		else {				 
			//Printf_("Not same eui\r\n");	
		}
	}

	if(temp == TRUE) {
		//Printf_("matching index %d\r\n", j);
	 	return j;
	}

	return UNKNOWN_DEVICE;
}


//**************************************************************************************
// �ζ� ���� ����
//**************************************************************************************
void LORA_CommandStep(void)
{
	static uint8_t aliveIndex = 0;

	uint32_t time;
	static uint32_t lastStepTimeOut = 0;
	
	time = microGetSystemTime();

	if(time - lastStepTimeOut >= TIME_1_SEC) {
		lastStepTimeOut = time;

		if(gMsgLevel & DEBUG_FLAG_0004) {
			Printf_("cmdStep = %d\r\n", cmdStep);
		}

		switch(cmdStep++) {
			//****************************************************************
			//�� 5�ʸ��� �۽�
			//****************************************************************
			case 0:
				if(usedBindingCount != 0) {
					if(aliveIndex < usedBindingCount) {
						if(gMsgLevel & DEBUG_FLAG_0003) {
							Printf_("Send lora index %d\r\n", loraBindingTable[aliveIndex]);
						}
						LORA_SendRequestTemp(loraBindingTable[aliveIndex]);
					}

					if(++aliveIndex == usedBindingCount) {
						aliveIndex = 0;
					}
				}

				break;

			//****************************************************************
			//���Ÿ�� 4�� ��ȯ
			//****************************************************************
			case 1:
				LORA_ReceiveMode();
				break;
		}

		if(cmdStep > 5) cmdStep = 0;
	}
}


