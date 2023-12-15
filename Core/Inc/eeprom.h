
#define		DEBUG_PRINT0										0

#define 	E2PROM_CMD_WRITE								0xA0
#define 	E2PROM_CMD_WRITE_P1							0xA2
#define 	I2C2_OWN_ADDRESS7								0xA0

#define 	EEPROM_TEST_LEN									16
#define 	EEPROM_TEST_SECTOR							10

void I2C_EEPROM_BLOCK_WRITE(uint8_t block, uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void I2C_EEPROM_BLOCK_READ(uint8_t block, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t EEPROM_GETSET(uint8_t argc, char **argv);


