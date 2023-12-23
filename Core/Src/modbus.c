#include "main.h"

MODBUS modbus;
extern TIM_HandleTypeDef htim3;

uint16_t Reg[] = {
		0x0001,
		0x0012,
		0x0013, 
		0x0004,
		0x0025,
		0x0036,
		0x0007,
		0x0008
};
//****************************************************************************************

void Modbus_Init(void)
{
	modbus.myadd = 0x01;
	modbus.timrun = 0;
	modbus.slave_add = 0x01;
}

//****************************************************************************************
// 타이머인터럽트에서 데이터 수신 후 8ms 가 되면 modbus.reflag 가 1이 된다
// 이때 받은데이터의 crc 를 검사한다 
//****************************************************************************************
void Modbus_Event(void)
{
	uint16_t crc, revcrc;

	if(modbus.reflag == 0) {
		return;
	}

	RX_LedOnOff(TRUE);

	crc = modbusCRC16(&modbus.revbuf[0], modbus.recount-2);
	
	revcrc = modbus.revbuf[modbus.recount-2]*256 + modbus.revbuf[modbus.recount-1];

	//Printf_("crc %04X, revcrc %04X\r\n", crc, revcrc);
	
	if(crc == revcrc) {
		//match address
		if(modbus.revbuf[0] == modbus.myadd) {
			//function check
			switch(modbus.revbuf[1]) {
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				//func 3, Read Holding Register 여러 word 읽기
				case 3:
					Modbus_Func3();
					break;
				//func 4, Read Input Register 여러 wrod 읽기
				case 4:
					break;
				case 5:
					break;
				//func 6, Write single Holding register
				case 6:
					Modbus_Func6();
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
				//func 16, Write multiple Holding register
				case 16:
					Modbus_Func16();
					break;
			}
		}
		else if(modbus.revbuf[0] != modbus.myadd) {
			if(gMsgLevel & DEBUG_FLAG_0005) {
				Printf_("MODBUS : not my address\r\n");
			}
		}
	}

	RX_LedOnOff(FALSE);

	modbus.recount = 0;
	modbus.reflag = 0;
}

//****************************************************************************************
// FUNC 3
// 1번 주소 데이터 1개 가져 오기
// REQUEST  : 01 03 00 01 00 01 D5 CA
// 0x200 주소 데이터 3개 가져오기
// RESPONSE : 01 03 02 00 03 F8 45
//****************************************************************************************
void Modbus_Func3(void)
{
	uint16_t Regadd, Reglen, crc;
	uint8_t i, j, loarIndex;
	uint8_t *arrayData;

	TX_LedOnOff(TRUE);

	//응답 주소는 받은 주소와 동일
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];
	//응답 길이는 받은 데이터 상위 + 하위에다가 
	Reglen = modbus.revbuf[4]*256 + modbus.revbuf[5];

	//Printf_("MODBUS func 3 %d %% 8 = %d\r\n", Regadd, Regadd % 8);

	//응답할 주소는 총 16번 어드레스 00078 과 3000 번지 
	if((Regadd % 8 != 0) && Regadd != 3000) {
		Modbus_Send_Error(INVALID_REG_CMD);
		return;
	}
	//사용할 레지스터 개수는 총 4, (8바이트)보다 크거나 작으면  
	else if(Reglen != 4 && Reglen != 48) {
		//Printf_("MODBUS Reg len %d\r\n", Reglen);

		Modbus_Send_Error(COUNT_IS_INVALID_CMD);
		return;
	}

	i = 0;
	
	modbus.sendbuf[i++] = modbus.myadd;
	modbus.sendbuf[i++] = READ_HOLDING_REG;
	//응당길이 * 2 개를 해야 바이트 개수
	modbus.sendbuf[i++] = ((Reglen*2)%256);

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Request 0x03 address is %04X, reg count is %d\r\n", Regadd, Reglen);
	}

#if 0
	//보낼 데이터 
	for(j=0; j<Reglen; j++) {
		//저장된 요청 주소의 데이터 상위 바이트 
		modbus.sendbuf[i++] = Reg[Regadd + j] / 256;
		//저장된 요청 주소의 데이터 하위 바이트 
		modbus.sendbuf[i++] = Reg[Regadd + j] % 256;
	}
#endif

  //3000 번지 센서 데이터 요청이 아니면
	if(Regadd != 3000) {
		//바이트 개수는 요청레지스터 개수 * 2
		//요청번지는 요청주소를 8 로 나눈 버퍼인덱스 값 (로라 저장된 인덱스 번호)
		loarIndex = Regadd/8;

#if 0
		if(gMsgLevel & DEBUG_FLAG_0005) {

			Printf_("[%02d]  : ", loarIndex);

			for(j=0; j<LORA_SERIAL_MAX; j++) {
				Printf_("%02X ", LORA_REG_DEVICE[loarIndex][j]);
			}
			Printf_("\r\n");
		}
#endif

		for(j=0; j<Reglen*2; j++) {
			modbus.sendbuf[i++] = LORA_REG_DEVICE[loarIndex][j];
		}
	}
	//3000 번지 센서데이터 요청이면
	else {
		//데이터값 복사하고 
		arrayData = (void *)&ARRAY_TEMP;
#if 0
		if(gMsgLevel & DEBUG_FLAG_0005) {
			Printf_("Array data size %d\r\n", sizeof(ARRAY_TEMP));

			for(j=0; j<sizeof(ARRAY_TEMP); j++) {
				Printf_("%02X ", arrayData[j]);
			}
			Printf_("\r\n");
		}
#endif
		for(j=0; j<sizeof(ARRAY_TEMP); j++) {
			modbus.sendbuf[i++] = arrayData[j];
		}

	}

	crc = modbusCRC16(modbus.sendbuf, i);

	modbus.sendbuf[i++] = crc / 256;
	modbus.sendbuf[i++] = crc % 256;

	for(j=0; j<i; j++) {
		Modbus_Send_Byte(modbus.sendbuf[j]);

		//Printf_("modbus send\r\n");
	}

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Send data %d : ", i);
		for(j=0; j<i; j++) {
			Printf_("%02X ", modbus.sendbuf[j]);
		}
		Printf_("\r\n");
	}

	TX_LedOnOff(FALSE);

}

//****************************************************************************************
// FUNC 6
// 싱글 레지스터 쓰기 
// REQUEST  : 01 06 0004 0005 0808
// RESPONSE : 01 06 0004 0005 0808
//****************************************************************************************
void Modbus_Func6(void)
{
	uint16_t Regadd;
	uint16_t val;
	uint16_t i, crc, j;

	TX_LedOnOff(TRUE);

	//응답 주소는 받은 주소와 동일
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];

	i=0;

	//응답 개수 : 레지스터에 써질 데이터 개수 word
	val = modbus.revbuf[4]*256 + modbus.revbuf[5];
	//보낼 데이터에 응답 개수를 넣고
	Reg[Regadd] = val;

	modbus.sendbuf[i++] = modbus.myadd;
	modbus.sendbuf[i++] = WRITE_SINGLE_REG;
	//응답 주소 상
	modbus.sendbuf[i++] = Regadd/256;
	//응답 주소 하
	modbus.sendbuf[i++] = Regadd%256;
	//응답 개수 상
	modbus.sendbuf[i++] = val/256;
	//응답 개수 하
	modbus.sendbuf[i++] = val%256;

	crc = modbusCRC16(modbus.sendbuf, i);

	modbus.sendbuf[i++] = crc / 256;
	modbus.sendbuf[i++] = crc % 256;

	for(j=0; j<i; j++) {
		Modbus_Send_Byte(modbus.sendbuf[j]);
	}

	TX_LedOnOff(FALSE);

}

//****************************************************************************************
// FUNC 16
// 싱글 레지스터 쓰기 
// REQUEST  : 01 10 0005 0002 04 0102 0304 929F
// RESPONSE : 01 10 0005 0002 51C9
//****************************************************************************************
void Modbus_Func16(void)
{
	uint16_t Regadd;
	uint16_t Reglen;
	uint16_t i, crc, j;
	uint8_t index, add[8];

	TX_LedOnOff(TRUE);

	//응답 주소는 받은 주소와 동일
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];
	//응답 길이는 받은 데이터 상위 + 하위에다가 
	Reglen = modbus.revbuf[4]*256 + modbus.revbuf[5];

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Request 0x10 address is %04X, reg len is %d\r\n", Regadd, Reglen);
	}

	//응답할 주소는 총 16번 어드레스 00078 까지 
	if((Regadd % 8 != 0) || Regadd > 120 ) {
		Modbus_Send_Error(INVALID_REG_CMD);
		return;
	}
	//사용할 레지스터 개수는 총 4, (8바이트)보다 크거나 작으면	
	else if(Reglen != 4) {
		Modbus_Send_Error(COUNT_IS_INVALID_CMD);
		return;
	}

#if 0
	//저장될 데이터 버퍼 
	for(i=0; i<Reglen; i++) {
		Reg[Regadd+i] = modbus.revbuf[7+i*2]*256 + modbus.revbuf[8+i*2];
		Printf_("Reg[%d+%d] = %04X\r\n", Regadd, i, Reg[Regadd+i]);
	}
#endif

	//응답 데이터는 수신 받은 레지스터 개수까지 그대로 전달 
	modbus.sendbuf[0] = modbus.revbuf[0];
	modbus.sendbuf[1] = modbus.revbuf[1];
	modbus.sendbuf[2] = modbus.revbuf[2];
	modbus.sendbuf[3] = modbus.revbuf[3];
	modbus.sendbuf[4] = modbus.revbuf[4];
	modbus.sendbuf[5] = modbus.revbuf[5];

	crc = modbusCRC16(modbus.sendbuf, 6);

	modbus.sendbuf[6] = crc / 256;
	modbus.sendbuf[7] = crc % 256;

	for(j=0; j<8; j++) {
		Modbus_Send_Byte(modbus.sendbuf[j]);
	}

	//EEPROM 에 로라 어드레스 저장
	index = ((uint8_t)Regadd/8) * 8;

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Index : %d\r\n", index);
	}

	memcpy(add, &modbus.revbuf[7], 8);

	if(gMsgLevel & DEBUG_FLAG_0005) {
		//저장될 데이터 버퍼 
		for(i=0; i<8; i++) {
			//Reg[Regadd+i] = modbus.revbuf[7+i*2]*256 + modbus.revbuf[8+i*2];
			Printf_("Reg[%d+%d] = %02X\r\n", index, i, add[i]);
		}
	}

	I2C_EEPROM_BLOCK_WRITE(0, &add, index, 8);
	//라이팅 했으면 EEPROM 다시 읽어와야 한다
	LORA_Get_Address();

	loraGetBinding(BINDING_TABLE_SIZE);

	TX_LedOnOff(FALSE);

}

//****************************************************************************************
// Modbus_Send_Byte
//****************************************************************************************
void Modbus_Send_Byte(uint8_t Modbus_byte)
{
	vPrintf(UART2_SERIAL, "%c", Modbus_byte);
}


//****************************************************************************************
// 에러코드 응답
//****************************************************************************************
void Modbus_Send_Error(uint8_t errorCode)
{
	uint16_t Regadd;
	uint16_t Reglen;
	uint16_t i, crc, j;

	i=0;

	//응답 데이터는 수신 받은 레지스터 개수까지 그대로 전달 
	modbus.sendbuf[i++] = modbus.myadd;
	modbus.sendbuf[i++] = RESPONSE_CMD;
	modbus.sendbuf[i++] = errorCode;

	crc = modbusCRC16(modbus.sendbuf, 3);

	modbus.sendbuf[i++] = crc / 256;
	modbus.sendbuf[i++] = crc % 256;

	for(j=0; j<i; j++) {
		Modbus_Send_Byte(modbus.sendbuf[j]);
	}
	
}

#if 0
void Host_Read03_slave(uint8_t slave, uint16_t StartAddr, uint16_t num)
{
	int j;
	uint16_t crc;

	modbus.slave_add = slave;
	modbus.Host_Txbuf[0] = slave;
	modbus.Host_Txbuf[1] = 0x03;
	modbus.Host_Txbuf[2] = StartAddr/256;
	modbus.Host_Txbuf[3] = StartAddr%256;
	modbus.Host_Txbuf[4] = num/256;
	modbus.Host_Txbuf[5] = num%256;
	crc = Modbus_CRC16(&modbus.Host_Txbuf[0], 6);
	modbus.Host_Txbuf[6] = crc/256;
	modbus.Host_Txbuf[7] = crc%256;

	for(j=0; j<8; j++) {
		Modbus_Send_Byte(modbus.Host_Txbuf[j]);
}

void HOST_ModbusRX(void)
{
	uint16_t crc, revcrc;

	if(modbus.reflag == 0) {
		return;
	}

	crc = Modbus_CRC16(&
}
#endif

#if 0
uint16_t CheckSum(uint8_t *buf, uint16_t len)
{
	uint8_t temp;
	uint16_t i;
	uint16_t crc = 0xFFFF;

	for(i=0; i<len; i++) {
		temp = *buf++ ^ crc;
		crc >>= 8;
		crc ^= crc_table[temp];
	}

	return crc;
}
#endif

//****************************************************************
// 체크섬
//****************************************************************
uint8_t checkSumXOR(uint8_t *msg, uint8_t len)
{
	uint8_t i, XOR;

	XOR = 0;

	for(i=0; i<len; i++) {
		XOR ^= msg[i];
	}

	return XOR;
}

//****************************************************************************************
// 상위 하위 순서 교환
//****************************************************************************************
unsigned short byteSwap(unsigned short n) {
  return ((n >> 8) | (n << 8));
}


unsigned short modbusCRC16(unsigned char *puchMsg, int usDataLen)
{
	int i;
	unsigned short crc, flag, convertCrc;
	
	crc = 0xFFFF;

	while(usDataLen--) {
		crc ^= *puchMsg++;
		for(i=0; i<8; i++) {
			flag = crc & 0x0001;
			crc >>= 1;
			if(flag) crc ^= POLYNORMIAL;
		}
	}

	//모드버스 CRC = 하위 + 상위
	convertCrc = byteSwap(crc);

	return convertCrc;
}


//****************************************************************************************
// 1ms 타이며 
// 데이터 수신 후 8ms 후 Modbus_Event 에서 CRC 계산을 수행 
//****************************************************************************************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3) {

		if(modbus.timrun != 0) {

			modbus.timeout++;
			//마지막 데이터 수신하고 8ms 지나면 데이터를 수신하지 않는다 
			//만약 if(modbus.timeout >= 8) 로 설정하고 115200 으로 데이터 요청하면 리부팅 됨,
			//그래서 && modbus.reflag != 0 를 함께 조건으로 해야 함
			//타이머 인터럽트만 루틴을 돌아야 하는데 리부팅 되는게 이해 안됨

			if(modbus.timeout >= 8 && modbus.reflag != 0) {

				modbus.timrun = 0;
				modbus.reflag = 1;

				//Printf_("Timer interrupt\r\n");

			}

		}

	}
}



