
/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t EEPROM_ADDRESS;


void I2C_EEPROM_BLOCK_WRITE(uint8_t block, uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
	uint16_t status, i;

	if(block == 0) {
		EEPROM_ADDRESS = E2PROM_CMD_WRITE;
	}
	else {
		EEPROM_ADDRESS = E2PROM_CMD_WRITE_P1;
	}
	
	//연속으로 쓰게 되면 동작이 안됨 
	//status = HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDRESS, WriteAddr, 1, pBuffer, NumByteToWrite,100);

	for(i=0; i<NumByteToWrite; i++) {
		status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, WriteAddr+i, 1, &pBuffer[i], 1, 1000);
		
		//딜레이가 반드시 필요 함 
		HAL_Delay(1);
		//Printf_("Write status %02X\r\n", status);
		HAL_WatchDogReload();

	}
	
}



void I2C_EEPROM_BLOCK_READ(uint8_t block, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	uint16_t status, i;

	if(block == 0) {
		EEPROM_ADDRESS = E2PROM_CMD_WRITE;
	}
	else {
		EEPROM_ADDRESS = E2PROM_CMD_WRITE_P1;
	}

	
	//연속으로 읽게 되면 동작이 안됨 
	//status = HAL_I2C_Mem_Read(&hi2c2, EEPROM_ADDRESS, ReadAddr, 1, pBuffer, NumByteToRead, 100);

	//반드시 하나씩 읽어야 함
	for(i=0; i<NumByteToRead; i++) {
		status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr+i, 1, &pBuffer[i], 1, 1000);
		
		HAL_WatchDogReload();
		//Printf_("Read status %02X\r\n", status);
	}	

	//Printf_("Read status %02X\r\n", status);

}

//***************************************************************
// EEPROM TEST
//***************************************************************
uint8_t EEPROM_GETSET(uint8_t argc, char **argv)
{
	uint8_t msg[256];
	uint16_t i,j;
	uint16_t add, value;
	
	//IWDG_ReloadCounter();

	switch( argv[1][0] ) {
		case 'R':
			Printf_("//----- AT24C04 read buffer-----\r\n");	
			Printf_("       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
			Printf_("---------------------------------------------------------\r\n");

			if(argc == 3) {
				if(argv[2][0] == '0') {
					I2C_EEPROM_BLOCK_READ(0, msg, 0, 256);
					//I2C_EEPROM_BLOCK_READ(0, msg, 0, EEPROM_TEST_LEN);

					for ( i = 0 ; i < 16 ; i++ ) {
						Printf_("0x%02X : ", i*16);

						for ( j = 0 ; j < 16 ; j++ ) {	
							Printf_("%02X ", msg[j + i*16]);
						}
						Printf_("\r\n");
					}
				}
				else if(argv[2][0] == '1') {
					I2C_EEPROM_BLOCK_READ(1, msg, 0, 256);

					for ( i = 0 ; i < 16 ; i++ ) {
						Printf_("0x%02X : ", i*16);

						for ( j = 0 ; j < 16 ; j++ ) {	
							Printf_("%02X ", msg[j + i*16]);
						}
						Printf_("\r\n");
					}
				}
			}
			else if(argc == 4) {
				add = atoi(argv[3]);
				
				I2C_EEPROM_BLOCK_READ(0, msg, add, 1);
				
				Printf_("read add %d : %02X\r\n", add, msg[0]);
			}
			break;

		case 'W':		
			for(i=0; i<256; i++) {
				msg[i] = i;
			}

			if(argc == 3) {

				if(argv[2][0] == '0') {
					I2C_EEPROM_BLOCK_WRITE(0, msg, 0, 256);
					
					//I2C_EEPROM_BLOCK_WRITE(0, msg, 0, EEPROM_TEST_LEN);
					Printf_("//----- AT24C04 page 0 fill with 0~254\r\n");	
				}
				else if(argv[2][0] == '1') {
					I2C_EEPROM_BLOCK_WRITE(1, msg, 0, 256);
					
					//I2C_EEPROM_BLOCK_WRITE(1, msg, 0, EEPROM_TEST_LEN);
					Printf_("//----- AT24C04 page 1 fill with 0~254\r\n");	
				}
			}
			else if(argc == 5) {
				add = atoi(argv[3]);
				value = atoi(argv[4]);

				msg[0] = (uint8_t)value;
				
				I2C_EEPROM_BLOCK_WRITE(0, msg, add, 1);
				
				Printf_("write add %d : %02X\r\n", add, msg[0]);
			}
			break;

		case 'F':		
			for(i=0; i<256; i++) {
				msg[i] = 0xFF;
			}

			if(argv[2][0] == '0') {
				I2C_EEPROM_BLOCK_WRITE(0, msg, 0, 256);
				
				//I2C_EEPROM_BLOCK_WRITE(0, msg, 0, EEPROM_TEST_LEN);
				Printf_("//----- AT24C04 page 0 fill with FF\r\n");	
			}
			else if(argv[2][0] == '1') {
				I2C_EEPROM_BLOCK_WRITE(1, msg, 0, 256);
				
				//I2C_EEPROM_BLOCK_WRITE(1, msg, 0, EEPROM_TEST_LEN);
				Printf_("//----- AT24C04 page 1 fill with FF\r\n");	
			}

			break;

		case 'E':		
			for(i=0; i<sizeof(msg); i++) {
				msg[i] = 0;
			}

			if(argv[2][0] == '0') {
				I2C_EEPROM_BLOCK_WRITE(0, msg, 0, 256);
				
				//I2C_EEPROM_BLOCK_WRITE(0, msg, 0, EEPROM_TEST_LEN);
				Printf_("//----- AT24C04 page 0 fill with 00\r\n");	
			}
			else if(argv[2][0] == '1') {
				I2C_EEPROM_BLOCK_WRITE(1, msg, 0, 256);
				
				//I2C_EEPROM_BLOCK_WRITE(1, msg, 0, EEPROM_TEST_LEN);
				Printf_("//----- AT24C04 page 1 fill with 00\r\n");	
			}

			break;
	}

	//IWDG_ReloadCounter();

	return TRUE;
}



