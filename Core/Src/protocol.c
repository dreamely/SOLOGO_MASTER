#include "main.h"


//RESPONSE_TEMP responseTemp[16];
_ARRAY_TEMP ARRAY_TEMP;
REQUEST_TEMP requestTemp;


#if 0
//**************************************************************
// AT Ä¿¸Çµå
//**************************************************************
uint8_t AT_command(uint8_t argc, char **argv)
{
	uint16_t num, value;

	if(argc == 3) {
		Printf_("cmd = %s\r\n", argv[1]);

		//+SET=VC
		if(strncmp(argv[1], SET_VIRTUAL_CMD, 7) == 0) {
			value = atoi(argv[2]);

			Printf_("value = %d\r\n", value);

			if(value <= 1) {
				if(value == 0) {
					monitoringVirtual = FALSE;
				}
				else {
					monitoringVirtual = TRUE;
				}

				Printf_("Master request virtual alert %d\r\n", value);
			}			

		}
	}

	return TRUE;
}
#endif

