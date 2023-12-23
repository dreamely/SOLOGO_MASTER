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
// Ÿ�̸����ͷ�Ʈ���� ������ ���� �� 8ms �� �Ǹ� modbus.reflag �� 1�� �ȴ�
// �̶� ������������ crc �� �˻��Ѵ� 
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
				//func 3, Read Holding Register ���� word �б�
				case 3:
					Modbus_Func3();
					break;
				//func 4, Read Input Register ���� wrod �б�
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
// 1�� �ּ� ������ 1�� ���� ����
// REQUEST  : 01 03 00 01 00 01 D5 CA
// 0x200 �ּ� ������ 3�� ��������
// RESPONSE : 01 03 02 00 03 F8 45
//****************************************************************************************
void Modbus_Func3(void)
{
	uint16_t Regadd, Reglen, crc;
	uint8_t i, j, loarIndex;
	uint8_t *arrayData;

	TX_LedOnOff(TRUE);

	//���� �ּҴ� ���� �ּҿ� ����
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];
	//���� ���̴� ���� ������ ���� + �������ٰ� 
	Reglen = modbus.revbuf[4]*256 + modbus.revbuf[5];

	//Printf_("MODBUS func 3 %d %% 8 = %d\r\n", Regadd, Regadd % 8);

	//������ �ּҴ� �� 16�� ��巹�� 00078 �� 3000 ���� 
	if((Regadd % 8 != 0) && Regadd != 3000) {
		Modbus_Send_Error(INVALID_REG_CMD);
		return;
	}
	//����� �������� ������ �� 4, (8����Ʈ)���� ũ�ų� ������  
	else if(Reglen != 4 && Reglen != 48) {
		//Printf_("MODBUS Reg len %d\r\n", Reglen);

		Modbus_Send_Error(COUNT_IS_INVALID_CMD);
		return;
	}

	i = 0;
	
	modbus.sendbuf[i++] = modbus.myadd;
	modbus.sendbuf[i++] = READ_HOLDING_REG;
	//������� * 2 ���� �ؾ� ����Ʈ ����
	modbus.sendbuf[i++] = ((Reglen*2)%256);

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Request 0x03 address is %04X, reg count is %d\r\n", Regadd, Reglen);
	}

#if 0
	//���� ������ 
	for(j=0; j<Reglen; j++) {
		//����� ��û �ּ��� ������ ���� ����Ʈ 
		modbus.sendbuf[i++] = Reg[Regadd + j] / 256;
		//����� ��û �ּ��� ������ ���� ����Ʈ 
		modbus.sendbuf[i++] = Reg[Regadd + j] % 256;
	}
#endif

  //3000 ���� ���� ������ ��û�� �ƴϸ�
	if(Regadd != 3000) {
		//����Ʈ ������ ��û�������� ���� * 2
		//��û������ ��û�ּҸ� 8 �� ���� �����ε��� �� (�ζ� ����� �ε��� ��ȣ)
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
	//3000 ���� ���������� ��û�̸�
	else {
		//�����Ͱ� �����ϰ� 
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
// �̱� �������� ���� 
// REQUEST  : 01 06 0004 0005 0808
// RESPONSE : 01 06 0004 0005 0808
//****************************************************************************************
void Modbus_Func6(void)
{
	uint16_t Regadd;
	uint16_t val;
	uint16_t i, crc, j;

	TX_LedOnOff(TRUE);

	//���� �ּҴ� ���� �ּҿ� ����
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];

	i=0;

	//���� ���� : �������Ϳ� ���� ������ ���� word
	val = modbus.revbuf[4]*256 + modbus.revbuf[5];
	//���� �����Ϳ� ���� ������ �ְ�
	Reg[Regadd] = val;

	modbus.sendbuf[i++] = modbus.myadd;
	modbus.sendbuf[i++] = WRITE_SINGLE_REG;
	//���� �ּ� ��
	modbus.sendbuf[i++] = Regadd/256;
	//���� �ּ� ��
	modbus.sendbuf[i++] = Regadd%256;
	//���� ���� ��
	modbus.sendbuf[i++] = val/256;
	//���� ���� ��
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
// �̱� �������� ���� 
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

	//���� �ּҴ� ���� �ּҿ� ����
	Regadd = modbus.revbuf[2]*256 + modbus.revbuf[3];
	//���� ���̴� ���� ������ ���� + �������ٰ� 
	Reglen = modbus.revbuf[4]*256 + modbus.revbuf[5];

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Request 0x10 address is %04X, reg len is %d\r\n", Regadd, Reglen);
	}

	//������ �ּҴ� �� 16�� ��巹�� 00078 ���� 
	if((Regadd % 8 != 0) || Regadd > 120 ) {
		Modbus_Send_Error(INVALID_REG_CMD);
		return;
	}
	//����� �������� ������ �� 4, (8����Ʈ)���� ũ�ų� ������	
	else if(Reglen != 4) {
		Modbus_Send_Error(COUNT_IS_INVALID_CMD);
		return;
	}

#if 0
	//����� ������ ���� 
	for(i=0; i<Reglen; i++) {
		Reg[Regadd+i] = modbus.revbuf[7+i*2]*256 + modbus.revbuf[8+i*2];
		Printf_("Reg[%d+%d] = %04X\r\n", Regadd, i, Reg[Regadd+i]);
	}
#endif

	//���� �����ʹ� ���� ���� �������� �������� �״�� ���� 
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

	//EEPROM �� �ζ� ��巹�� ����
	index = ((uint8_t)Regadd/8) * 8;

	if(gMsgLevel & DEBUG_FLAG_0005) {
		Printf_("Index : %d\r\n", index);
	}

	memcpy(add, &modbus.revbuf[7], 8);

	if(gMsgLevel & DEBUG_FLAG_0005) {
		//����� ������ ���� 
		for(i=0; i<8; i++) {
			//Reg[Regadd+i] = modbus.revbuf[7+i*2]*256 + modbus.revbuf[8+i*2];
			Printf_("Reg[%d+%d] = %02X\r\n", index, i, add[i]);
		}
	}

	I2C_EEPROM_BLOCK_WRITE(0, &add, index, 8);
	//������ ������ EEPROM �ٽ� �о�;� �Ѵ�
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
// �����ڵ� ����
//****************************************************************************************
void Modbus_Send_Error(uint8_t errorCode)
{
	uint16_t Regadd;
	uint16_t Reglen;
	uint16_t i, crc, j;

	i=0;

	//���� �����ʹ� ���� ���� �������� �������� �״�� ���� 
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
// üũ��
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
// ���� ���� ���� ��ȯ
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

	//������ CRC = ���� + ����
	convertCrc = byteSwap(crc);

	return convertCrc;
}


//****************************************************************************************
// 1ms Ÿ�̸� 
// ������ ���� �� 8ms �� Modbus_Event ���� CRC ����� ���� 
//****************************************************************************************
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3) {

		if(modbus.timrun != 0) {

			modbus.timeout++;
			//������ ������ �����ϰ� 8ms ������ �����͸� �������� �ʴ´� 
			//���� if(modbus.timeout >= 8) �� �����ϰ� 115200 ���� ������ ��û�ϸ� ������ ��,
			//�׷��� && modbus.reflag != 0 �� �Բ� �������� �ؾ� ��
			//Ÿ�̸� ���ͷ�Ʈ�� ��ƾ�� ���ƾ� �ϴµ� ������ �Ǵ°� ���� �ȵ�

			if(modbus.timeout >= 8 && modbus.reflag != 0) {

				modbus.timrun = 0;
				modbus.reflag = 1;

				//Printf_("Timer interrupt\r\n");

			}

		}

	}
}



