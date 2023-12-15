
#define __UART_H__

#include "main.h"

COM_RING_BUFF Com1Buf,Com2Buf,Com3Buf;

extern UART_HandleTypeDef huart1,huart2, huart3;

//**************************************************************************************
#if 0
int _write( int32_t file , uint8_t *ptr , int32_t len )
{
	for(int16_t i=0; i<len; ++i) {
		HAL_UART_Transmit(&huart1, ptr++, 1, 100);
	}

	return len;
}
#endif

typedef struct {
	char *CmdStr;                     		// ��� ���ڿ�	
	uint8_t (*func)(uint8_t argc, char **argv);	
} TCommand_t;

TCommand_t Cmds[] = {
  { "MSG"  , CommonDebugFlagOn  }, 
	{ "RST"  , SoftReset				  },
	{ "WHILE", WatchDogReset			},
	{ "?"    , helpMenu           }, 
	{ "INFO" , compileInfo        },
	{ "EET",   EEPROM_GETSET			},
	{ "RTC"  , RTC_TimeGetSet 		},
	{ "GET"  , LORA_DispAddress		},
	{ "CONFIG"  , LORASET_Terminal_Services 		},

	{ "LORA" , LORA_Test 		      },
	{ "SENSOR",LORA_SensorStatus  },
	{ "MAC",	 LORA_MacInput      },

	{ NULL   , NULL }
};

uint32_t gMsgLevel = 0;


void vPutHex(uint8_t port, uint8_t number)
{
  uint8_t i;
  
  i = (number >> 4) & 0x0f;       //16^1
  if(i <= 9)
    vPutChar(port, i + '0');
  else
    vPutChar(port, i - 10 + 'A');

  i = number & 0x0f;
  if(i <= 9)
    vPutChar(port, i + '0');       //16^0
  else
    vPutChar(port, i - 10 + 'A');
}

//******************************************************
//  vPutChar()
//******************************************************
void vPutChar(unsigned char port, unsigned char data)
{
	unsigned char msg[1];

	msg[0] = data;

  switch(port) {
    case 1:
			HAL_UART_Transmit(&huart1, msg, 1, 100); 
      break;

		case 2:
			HAL_UART_Transmit(&huart2, msg, 1, 100); 
			break;

    case 3:
			HAL_UART_Transmit(&huart3, msg, 1, 100); 
      break;

  }
}


uint16_t Ascii_4_int(uint8_t *data)
{
	uint16_t num = 0;
	uint8_t i, val[4];

	for(i=0; i<4; i++) {
		val[i] = AsciiToHex(data[i]);

		//vPrintf(UART1_SERIAL, "data[%d] = %02X  val[%d] = %d\r\n", 
		//		i, data[i], i, val[i]);
	}

	num  = (uint16_t)val[0] << 12;
	num |= (uint16_t)val[1] << 8;
	num |= (uint16_t)val[2] << 4;
	num |= val[3];

	//vPrintf(UART1_SERIAL, "num = %d\r\n", num);

	return num;
}

uint8_t AsciiToHex(uint8_t hex)
{
	if(hex == 0x20) return 0;
	else if(hex >= 0x30 && hex <= 0x39) {
		return hex - '0';
	}
	else {
		return hex - 0x37;
	}
}

uint8_t Ascii_2_Hex(uint8_t num1, uint8_t num2)
{
	uint8_t hex;

	hex = (num1 - '0') << 4;
	hex |= (num2 - '0');

	return hex;
}


uint16_t Ascii_4_hex(uint8_t *data)
{
	uint16_t temp;
	uint16_t num;
	
	//vPrintf(UART1_SERIAL, "%x %x %x %x : ", data[0],data[1],data[2],data[3]);

	if(data[0] >= 0x30 && data[0] <= 0x39) {
		temp = data[0] - '0';
		num = temp << 12;
	}
	else {
		temp = data[0] - 0x37;
		num = temp << 12;
	}
	//vPrintf(UART1_SERIAL, "%x num = %2x\r\n", temp, num);

	if(data[1] >= 0x30 && data[1] <= 0x39) {
		temp = data[1] - '0';
		num |= temp << 8;
	}
	else {
		temp = data[1] - 0x37;
		num |= temp << 8;
	}
	//vPrintf(UART1_SERIAL, "%x num = %2x\r\n", temp, num);

	if(data[2] >= 0x30 && data[2] <= 0x39) {
		temp = data[2] - '0';
		num |= temp << 4;
	}
	else {
		temp = data[2] - 0x37;
		num |= temp << 4;
	}
	//vPrintf(UART1_SERIAL, "%x num = %2x\r\n", temp, num);


	if(data[3] >= 0x30 && data[3] <= 0x39) {
		temp = data[3] - '0';
		//vPrintf(UART1_SERIAL, "%x ", temp);
		num |= temp;
	}
	else {
		temp = data[3] - 0x37;
		//vPrintf(UART1_SERIAL, "%x ", temp);
		num |= temp;
	}

	return num;
	//vPrintf(UART1_SERIAL, "%2x %d\r\n", temp, num);
}

void Hex_ascii(uint8_t *data, uint8_t num)
{
	if(num < 10) data[0] = num + '0';
	else {
		 data[0] = num + 0x37;
	}
}

void Hex_2_ascii(uint8_t *data, uint8_t num)
{
	uint8_t temp;
	
	temp = (num >> 4);
	if(temp < 10) data[0] = temp + '0';
	else {
		 data[0] = temp + 0x37;
	}

	temp = (num & 0x0F);
	if(temp < 10) data[1] = temp + '0';
	else {
		 data[1] = temp + 0x37;
	}
}

void Hex_4_ascii(uint8_t *data, uint16_t num)
{
	uint8_t temp;
	
	//vPrintf(UART1_SERIAL, "num = %d\r\n", num);

	temp = ((num&0xF000) >> 12);
	if(temp < 10) data[0] = temp + '0';
	else {
		 data[0] = temp + 0x37;
	}

	temp = ((num&0x0F00) >> 8);
	if(temp < 10) data[1] = temp + '0';
	else {
		 data[1] = temp + 0x37;
	}

	temp = ((num&0x00F0) >> 4);
	if(temp < 10) data[2] = temp + '0';
	else {
		 data[2] = temp + 0x37;
	}

	temp = (num & 0x000F);
	if(temp < 10) data[3] = temp + '0';
	else {
		 data[3] = temp + 0x37;
	}
}

//----------------------------------------------------------------------------//
//--���� : 16���� ��ġ�� ���ڿ��� ���������� �ٲ۴�. -------------------------//
#define my_ishex(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
uint8_t my_htoi(char *Str, int16_t *ret)
{
	int16_t i, j;
	char a;
	
	j = strlen(Str);
	*ret = 0;
	
	for ( i = 0 ; i < j  ; i++ ) {
		if (!my_ishex(*Str)) return FALSE;	// Return 0 (FALSE)
		a = ((*Str&0x30) == 0x30  ? (*Str&0x0F) : (*Str&0x0F)+9);
		*ret = ((*ret)<<4) + a;
		Str++;	   
    }    
    return TRUE;	// Return 1 (TRUE)
}
//----------------------------------------------------------------------------//
uint8_t my_htol(char *Str, int32_t *ret)
{
	int i, j;
	char a;
	
	j = strlen(Str);	
	*ret = 0;
	
	for ( i = 0 ; i < j  ; i++ ) {
		if (!my_ishex(*Str)) return FALSE;	// Return 0 (FALSE)
		a = ((*Str&0x30) == 0x30  ? (*Str&0x0F) : (*Str&0x0F)+9);
		*ret = ((*ret)<<4) + a;		
		Str++;	   
  }    	
  return TRUE;	// Return 1 (TRUE)
}


//******************************************************
//  ��簪�� ��Ʈ������ ����, 1����Ʈ ��� -> 2����Ʈ ����
//******************************************************
int hex_convert_hexstring(uint8_t *hexData, uint8_t len, uint8_t *string)
{
    int i = 0;
    int idx = 0;
    
    for(i=0; i<len; i++) {
        string[idx++] = (*(hexData+i)) >> 4 & 0x0F;
        string[idx++] = (*(hexData+i)) & 0x0F;
    }
    
    for(i=0; i<idx; i++) {
        if(string[i] >= 10) {
            string[i] = string[i] - 10+'A';
        }
        else {
            string[i] = string[i] + '0';
        }
    }
    
    return idx;
}

//******************************************************
//  ��� ��Ʈ�� 2����Ʈ�� 1����Ʈ ���� ��ȯ
//******************************************************
int hexstring_conver_hex(uint8_t *string, uint8_t len, uint8_t *hexData)
{
  int idx = 0;
  int i = 0;
  
  for(i=0; i<len; i++) {
      if(*(string+i) >= 'A') {
          *(string+i) = *(string+i) -'A'+10;
      }
      else {
          *(string+i) = *(string+i) - '0';
      }
  }
  
  i = 0;
  
  for(idx = 0; idx < len/2; idx++) {
      hexData[idx] = *(string+ i++) <<4 | *(string+ i++) & 0x0F;
  }
  
  return idx;
}

//******************************************************
// UART ���� �ʱ�ȭ
//******************************************************
void UartInit(void)
{
	UART1_para_init();
	UART2_para_init();
	UART3_para_init();
}


void UART1_para_init(void)
{
  Com1Buf.head = 0;
  Com1Buf.tail = 0;
  Com1Buf.buffering_cnt = 0;
  Com1Buf.rx_buf[0] = '\0';
  Com1Buf.rx_buf[1] = '\0';
}

uint16_t UART1_getc(void)
{
  uint16_t ch;

  if(Com1Buf.buffering_cnt > 0)	
  {
    Com1Buf.buffering_cnt--;
    ch = Com1Buf.rx_buf[Com1Buf.head++];
    
    if(Com1Buf.head >= LINE_BUFFER_MAX)
      Com1Buf.head = 0;

		//vPrintf(UART1_SERIAL, "ch %c\r\n", ch);

    return(ch & 0x00FF);
  }
  return(0xFFFF);	
}

void processUart1Input(void)
{   
#if 0
	uint8_t cmd;
	uint16_t ch;

  static  uint8_t stx_flag1 = 0;
  static  uint8_t stx_over1 = 0;
  static  uint8_t complete_message1 = 0;
  static  uint8_t bufferLine1 = 0;
  static  uint8_t lineBuffer1[SENSOR_BUFFER_MAX];

	ch = UART1_getc();
	if(ch == 0xFFFF) return;

	cmd = (uint8_t)ch;

	//vPrintf(UART2_SERIAL, "%c[%x] ", cmd, cmd);
	//vPrintf(UART2_SERIAL, "%c", cmd);

	if(ch == SENSOR_STX) {
		if(stx_flag1 == 0) {
			// dirty byte ...
			if(bufferLine1 > 0)
				bufferLine1 = 0;

			stx_flag1 = 1;
			lineBuffer1[bufferLine1++] = ch;

			return;
		}

		// in the message,,, check duplicate STX
		if(stx_over1 == 0) {
			stx_over1 = 1;
			lineBuffer1[bufferLine1++] = ch;
			return;
		}
		else {
			stx_over1 = 0;
			lineBuffer1[bufferLine1++] = ch;
			return;
		}
	}

	if((bufferLine1 == SENSOR_BUFFER_MAX) ||  // over maxbuffer
		 (bufferLine1 == 0)) { 	 // none read STX
		// rebuffering...
		bufferLine1 = 0;
		stx_flag1 = 0;
		stx_over1 = 0;
		complete_message1 = 0;
		return;
	}
	if(stx_over1 == 1 && bufferLine1 < 3) { 			// in the message,, founded STX
		// rebuffering...
		bufferLine1 = 0;
		if(lineBuffer1[0] == SENSOR_STX) lineBuffer1[bufferLine1++] = SENSOR_STX;

		stx_flag1 = 1;
		stx_over1 = 0;
		complete_message1 = 0;

		//vPrintf(1, "stx over\r\n");
		return;
	}

	lineBuffer1[bufferLine1++] = ch;

	// until read [LEN] field
	if(bufferLine1 < 3)
		return;

	if(lineBuffer1[0] == SENSOR_STX && lineBuffer1[bufferLine1-1] == SENSOR_ETX) {

		LCD_RevOk = TRUE;
		LCD_RevLen = bufferLine1;
		memcpy(LCD_REV_BUFFER, &lineBuffer1, bufferLine1);

		complete_message1 = 1;

	}

	if(complete_message1 == 0) {
		return;
	}
		

	stx_flag1 = 0;
	stx_over1 = 0;
	bufferLine1 = 0;
	lineBuffer1[0] = '\0';
	lineBuffer1[1] = '\0';
	complete_message1 = 0;
#endif	
}

#if 1
void UART2_para_init(void)
{
  Com2Buf.head = 0;
  Com2Buf.tail = 0;
  Com2Buf.buffering_cnt = 0;
  Com2Buf.rx_buf[0] = '\0';
  Com2Buf.rx_buf[1] = '\0';
}

uint16_t UART2_getc(void)
{
  uint16_t ch;
  if(Com2Buf.buffering_cnt > 0)	
  {
    Com2Buf.buffering_cnt--;
    ch = Com2Buf.rx_buf[Com2Buf.head++];
    
    if(Com2Buf.head >= LINE_BUFFER_MAX)
      Com2Buf.head = 0;

		//vPrintf(UART1_SERIAL, "%02X ", ch);

    return(ch & 0x00FF);
  }
  return(0xFFFF);	
}
#endif


void processUart2Input(void)
{   
	uint8_t  i, msg[1];
	uint16_t crc, revCRC, temp;

	static	uint8_t stx_flag2 = 0;
	static	uint8_t stx_over2 = 0;
	static	uint8_t complete_message2 = 0;
	static	uint8_t bufferLine2 = 0;
	static	uint8_t lineBuffer2[LINE_BUFFER_MAX];

	uint16_t ch;
	uint8_t cmd;

	if((ch = UART2_getc()) == 0xFFFF) return;

	cmd = (uint8_t)ch;

	//vPrintf(UART1_SERIAL, "%02X %c", cmd, cmd);
#if 0	
	if (cmd == CMD_STX) {
		if(stx_flag2 == 0) {
			// dirty byte ...
			if(bufferLine2 > 0)
				bufferLine2 = 0;

			stx_flag2 = 1;
			lineBuffer2[bufferLine2++] = cmd;

			//vPrintf(0, "stx\r\n");
			return;
		}

		// in the message,,, check duplicate STX
		if(stx_over2 == 0) {
			stx_over2 = 1;
			lineBuffer2[bufferLine2++] = cmd;
			return;
		}
		else {
			stx_over2 = 0;
			lineBuffer2[bufferLine2++] = cmd;
			return;
		}
	}

	if((bufferLine2 == LINE_BUFFER_MAX) ||	// over maxbuffer
		 (bufferLine2 == 0)) {		// none read STX
		// rebuffering...
		bufferLine2 = 0;
		stx_flag2 = 0;
		stx_over2 = 0;
		complete_message2 = 0;
		return;
	}

	if(stx_over2 == 1 && bufferLine2 < 3) { 			// in the message,, founded STX
		// rebuffering...
		bufferLine2 = 0;
		lineBuffer2[bufferLine2++] = CMD_STX;
		stx_flag2 = 1;
		stx_over2 = 0;
		complete_message2 = 0;
		return;
	}

	lineBuffer2[bufferLine2++] = cmd;

	// until read [LEN] field
	if(bufferLine2 < 3)
		return; 


	//*********************************************************
	// SET 0 ~ 10, XXX
	//*********************************************************
	if(lineBuffer2[0] == CMD_STX && lineBuffer2[1] == 'E' && lineBuffer2[2] == 'T' &&
		lineBuffer2[bufferLine2-1] == '\n') {


		complete_message2 = 1;
	}	
	else {
		complete_message2 = 0;
	} 	

	if(complete_message2 == 0) {
		return;
	}

	stx_flag2 = 0;
	stx_over2 = 0;
	bufferLine2 = 0;
	lineBuffer2[0] = '\0';
	lineBuffer2[1] = '\0';
	complete_message2 = 0;	
#endif

}


void UART3_para_init(void)
{
  Com3Buf.head = 0;
  Com3Buf.tail = 0;
  Com3Buf.buffering_cnt = 0;
  Com3Buf.rx_buf[0] = '\0';
  Com3Buf.rx_buf[1] = '\0';
}

uint16_t UART3_getc(void)
{
  uint16_t ch;

  if(Com3Buf.buffering_cnt > 0)	
  {
    Com3Buf.buffering_cnt--;
    ch = Com3Buf.rx_buf[Com3Buf.head++];
    
    if(Com3Buf.head >= LINE_BUFFER_MAX)
      Com3Buf.head = 0;

		if(gMsgLevel & DEBUG_FLAG_0001) {
			Printf_("%c", ch);
		}

    return(ch & 0x00FF);
  }
  return(0xFFFF);	
}

//*******************************************************
//üũ��
//*******************************************************
uint8_t CheckSum(uint8_t *data, uint8_t len)
{
	uint8_t SUM = 0;
	uint8_t i;

	for(i=0; i<len; i++) {
		SUM ^= data[i];
	}

	return SUM;
}


//******************************************************
//  vPrintf()
//******************************************************
void vPrintf(unsigned char port, const char * string, ...)
{
  va_list args;
  char buffer[LINE_BUFFER_MAX];
	char len, i;

  va_start(args, string);

	len = vsprintf(buffer, string, args);
  va_end(args);

	for(i=0; i<len; i++)
		vPutChar(port, buffer[i]);
}

void Printf_(const char *format, ...)  
{
  va_list arg;  
  char str[512];  
  int len, i;  

  va_start(arg, format);  
  len = vsprintf(str, format, arg);            

#if 0
	HAL_UART_Transmit_IT(&huart1, str, len);
#else
	for(i=0; i<len; i++)
		vPutChar(UART1_SERIAL, str[i]);
#endif
}

//*********************************************************************
//����Ʈ ����
//*********************************************************************
uint8_t SoftReset(uint8_t argc, char **argv)
{
	HAL_NVIC_SystemReset();

	return TRUE;
}

//*********************************************************************
//��ġ�� ����
//*********************************************************************
uint8_t WatchDogReset(uint8_t argc, char **argv)
{
	while(1);

	return TRUE;
}


//*********************************************************************
//������ ���� ������ ����ϰ� ������Ʈ���� ȣ���ϴ� �Լ�
//*********************************************************************
uint8_t compileInfo(uint8_t argc, char **argv)
{
#if 1
	Printf_("****************************************************\r\n");
  Printf_("      SOLOGO LORA MASTER    \r\n");
	Printf_("****************************************************\r\n");

	Printf_(" compiled on\t\t: ");
	Printf_(__DATE__);
	Printf_("\r\n");
	Printf_(" compiled time\t\t: ");
	Printf_(__TIME__);
	Printf_("\r\n");
	Printf_("\r\n");

	Printf_("****************************************************\r\n");

	Printf_("\r\n");
#endif
	return TRUE;
}

//*********************************************************************
//���� �޴� ���
//*********************************************************************
uint8_t helpMenu(uint8_t argc, char **argv)
{
	compileInfo(0,0);

	Printf_(" HELP / ?             : Help\r\n");

	Printf_(" MSG  2               : Display receive lora real msg\r\n");
	Printf_(" MSG  3               : Display lora analyze msg\r\n");
	Printf_(" MSG  4               : Display command step\r\n");
	Printf_(" MSG  5               : Display modbus msg\r\n");

	Printf_(" EET R 0~1            : Read flash block 0\r\n");
	Printf_(" EET E 0~1            : Fill block 0 with 0x00\r\n");
	Printf_(" EET F 0~1            : Fill block 0 with 0xFF\r\n");
	Printf_(" EET W 0~1            : Fill block 0 0 ~ 15\r\n");
	Printf_(" RTC [Y M D H M S]    : Rtc Time Set\r\n");
	Printf_(" RTC                  : Read RTC time\r\n");
	Printf_(" GET                  : Display lora serial number\r\n");
	Printf_(" CONFIG               : Lora config\r\n");
	Printf_(" SENSOR               : Display sensor info\r\n");
	Printf_(" MAC X XXXXXXXX       : Write lora eui\r\n");

	Printf_(" RST                  : Soft reset\r\n");
	Printf_(" WHILE                : Watch dog test\r\n");

	Printf_("****************************************************\r\n");

	return TRUE;
}

//********************************************************************************************
//���� �Է�
//********************************************************************************************
uint8_t GetCh(uint8_t *ch)
{				
	uint16_t cmd;
	uint8_t  temp;

  //watchdogReload();

	cmd = UART1_getc();

	if(cmd != 0xFFFF) {
		temp = (uint8_t)cmd;

		ch = &temp;

		//vPrintf(UART1_SERIAL, "temp = %c, ch = %c\n", temp, *ch);

		return TRUE;
	}

	return FALSE;	
}

//********************************************************************************************
// ���� : ���ڿ� �Է��� �޴´�. ( CR�� �Էµɶ����� )
// �Ű� : ���� 
// ��ȯ : ���� 
// ���� : ���� 
//********************************************************************************************
uint8_t Get_Command (char *sBuf)
{
	static uint8_t buf[50];
	static uint8_t i = 0;	    
	uint16_t cmd;  
	uint8_t ch;		

	cmd = UART1_getc();
	if(cmd == 0xFFFF) return FALSE;

	ch = (uint8_t)cmd;

	switch( ch ) {    	

		//�齺���̽��� ó���ϸ鼭 ���ڱ����� �߻��� �� �����Ƿ� ���� 
    //�ʱ�ȭ ��Ű�� �� �Է� �ϰ� �Ѵ�
		case BS:      
		case ESC:      
			i=0;     
			Printf_("\r\n");         
			Printf_("SOLOGO MASTER >");        
			break;                   

		case ENTER:		 
			//���� + 1 �� �ؾ� ����ü �Լ� ȣ���ϴ� ������ �����
			//cmd + argv
			memcpy(sBuf, buf, i+1);	
			memset(buf , 0x00, 50);

			//�빮�� ��ȯ,����ü �Լ��� ȣ���Ҷ� ����ü �Լ��� �빮�ڷ� �����س���
			//���⼭ ��ü�� �빮�ڷ� �ٲ�
			//�ܼ� ���񽺴� ����� ������ �ϱ� ���ؼ� ����ϱ� ������
			ChangeUpperChar(sBuf);
			i=0; 
			return TRUE;

		default : 
			vPrintf(UART1_SERIAL,"%c", (char)ch);
			*(buf+i) = (char)ch;
			if ( ++i > 49 ) i = 48;									
			break;
	}     		
	
	//watchdogReload();	

	return FALSE;
}
//--���� : ��ҹ��� ���ڿ��� �ٲ۴�. -----------------------------------------//
void ChangeUpperChar( char *Str )
{
	while( *Str ){ 
		my_toupper( Str ); 
		Str++; 
	}
}
//----------------------------------------------------------------------------//
//Str �� ������ ���ڸ� my_toupper �� �̿��ؼ� a ~ z ���� �ԷµǸ� �� ���� A ~ Z �� ��ȯ��
void my_toupper( char *Str )
{  
  if (('a' <= *Str) && ('z' >= *Str)) {
		*Str = *Str - 0x20; 
  } 
}
//********************************************************************************************
//���� ���ڸ� argc, argv ������ ����
//STM32F407 ������ ���� ���۵�
//********************************************************************************************
#if 0
uint8_t parseString(char* line, char*** argv) {

  char* buffer;
  uint8_t argc;

  buffer = (char*) malloc(strlen(line) * sizeof(char));
  strcpy(buffer,line);
  (*argv) = (char**) malloc(MAX_TOKS * sizeof(char**));

  argc = 0;  
  (*argv)[argc++] = strtok(buffer, DELIMITERS);
  while ((((*argv)[argc] = strtok(NULL, DELIMITERS)) != NULL) &&
	 (argc < MAX_TOKS)) ++argc;

  return argc;
}
#endif
//----------------------------------------------------------------------------//
char* strtok_r(char *s1, const char *s2, char **lasts)
{
  char *ret;
 
  if (s1 == NULL)
    s1 = *lasts;
  while(*s1 && strchr(s2, *s1))
    ++s1;
  if(*s1 == '\0')
    return NULL;
  ret = s1;
  while(*s1 && !strchr(s2, *s1))
    ++s1;
  if(*s1)
    *s1++ = '\0';
  *lasts = s1;
  return ret;
}

//--���� : 10���� ��ġ�� ���ڿ��� ���������� �ٲ۴�. -------------------------//
#define my_isdigit(c) (c >= '0' && c <= '9')
uint8_t my_atoi(char *Str, int *ret)
{
	int i, j;
	
	j = strlen(Str);
	*ret = 0;
	
	for ( i = 0 ; i < j  ; i++ ) {
		if (!my_isdigit(*Str)) return FALSE;	// Return 0 (FALSE)
        *ret = *ret * 10 + *(Str++) - '0';				
    }    
    return TRUE;								// Return 1 (TRUE)
}

static const char *delim = " \f\n\r\t\v:.";
uint8_t Parse_Args(char *cmdline, char **argv)
{
	
	char *last;
	char *tok;
	int argc = 0;
	
	argv[argc] = NULL;
	
	for (tok = strtok_r(cmdline, delim, &last); tok; tok = strtok_r(NULL, delim, &last)) 
    {
		argv[argc++] = tok;
	}	
	
	return (uint8_t)argc;
}

//********************************************************************************************
//���ο��� �ܼ� �Է� ����
//********************************************************************************************
char Terminal_Services_Function(void)
{
	static uint8_t	sExecStep = 0;	
	static char		fReadBuffer[64];
	static uint8_t  argc;
	static char		*argv[32];
	static uint8_t  cmdlp;	
		
	switch (sExecStep)
	{   
	case 0:	
		Printf_("SOLOGO MASTER >");	sExecStep++; return FALSE;
	case 1:			
		if (!Get_Command(fReadBuffer)) return FALSE;
		Printf_("\r\n");
		argc = Parse_Args( fReadBuffer, argv );
		
		//vPrintf(UART1_SERIAL, "\r\n TEST : %d %s-%s-%s-%s \r\n", cmdlp, argv[0], argv[1], argv[2], Cmds[cmdlp].CmdStr);   
		
    //�ַܼ� �Է��� ������
		if (argc) {					

			//Cmds ���ڿ� 0 ������ ������� �˻�
			cmdlp = 0;

			//������ NULL ���� ������ While ���� ��������
			while(Cmds[cmdlp].CmdStr)	{
				if( strcmp( argv[0], Cmds[cmdlp].CmdStr ) == 0 ) {					
					Cmds[cmdlp].func( argc, argv );
					Printf_("\r\n");
					sExecStep = 0; 
					return TRUE;		
				}
				cmdlp++;
			}
		}		
		
		sExecStep = 0; return TRUE;
	default:
		sExecStep = 0; 
		return FALSE;
	}		
}

//********************************************************************************************
//  ����� ����Ʈ
//********************************************************************************************
void MSGPrint_(uint32_t flag, const char *format, ...)
{
  va_list args;  
  char str[100] = {0,};  
  int len, i;  
      
  if ( (gMsgLevel & flag) == 0x0000 ) return; 
  
  va_start(args, format);  
  len = vsprintf(str, format, args);    

	//vPrintf(UART1_SERIAL, "gMsgLevel %02X, flag %02X\n", gMsgLevel, flag);        

	if( flag & gMsgLevel) { 
		//vPrintf(UART1_SERIAL,"DAON TECH SENSOR > ");   
		for(i=0; i<len; i++)
			vPutChar(UART1_SERIAL, str[i]);
	}

  va_end(args);
}
//********************************************************************************************
uint8_t CommonDebugFlagOn(uint8_t argc, char **argv)
{
	if (argc < 1) return FALSE;	
	if (argc == 1) {
		Printf_("DebugFlag %s(0x%04X)\r\n", (gMsgLevel == 0x0000)?"OFF": "O N", gMsgLevel );
		Printf_("-DEBUG_FLAG_0001 %s \r\n", (gMsgLevel & DEBUG_FLAG_0001)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0002 %s \r\n", (gMsgLevel & DEBUG_FLAG_0002)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0003 %s \r\n", (gMsgLevel & DEBUG_FLAG_0003)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0004 %s \r\n", (gMsgLevel & DEBUG_FLAG_0004)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0005 %s \r\n", (gMsgLevel & DEBUG_FLAG_0005)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0006 %s \r\n", (gMsgLevel & DEBUG_FLAG_0006)?"O N":"OFF");		
		Printf_("-DEBUG_FLAG_0007 %s \r\n", (gMsgLevel & DEBUG_FLAG_0007)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0008 %s \r\n", (gMsgLevel & DEBUG_FLAG_0008)?"O N":"OFF");		
		Printf_("-DEBUG_FLAG_0009 %s \r\n", (gMsgLevel & DEBUG_FLAG_0009)?"O N":"OFF");
		Printf_("-DEBUG_FLAG_0010 %s \r\n", (gMsgLevel & DEBUG_FLAG_0010)?"O N":"OFF");
    Printf_("-DEBUG_FLAG_0011 %s \r\n", (gMsgLevel & DEBUG_FLAG_0011)?"O N":"OFF");
	  Printf_("-DEBUG_FLAG_0012 %s \r\n", (gMsgLevel & DEBUG_FLAG_0012)?"O N":"OFF");		
	  Printf_("-DEBUG_FLAG_0013 %s \r\n", (gMsgLevel & DEBUG_FLAG_0013)?"O N":"OFF");		
	  Printf_("-DEBUG_FLAG_0014 %s \r\n", (gMsgLevel & DEBUG_FLAG_0014)?"O N":"OFF");		
	  Printf_("-DEBUG_FLAG_0015 %s \r\n", (gMsgLevel & DEBUG_FLAG_0015)?"O N":"OFF");
	  Printf_("-DEBUG_FLAG_0016 %s \r\n", (gMsgLevel & DEBUG_FLAG_0016)?"O N":"OFF");		
	}
	if (argc == 2) {
		if (argv[1][0] == 'N') {
			Printf_("DebugFlag OFF \r\n");
			gMsgLevel = DEBUG_FLAG_0000;	
		}
		else if (argv[1][0] == 'Y') {
			Printf_("DebugFlag ON \r\n");
			gMsgLevel |=  DEBUG_FLAG_ALL;
		}
		else {
			uint8_t	flag;

			flag = atoi(argv[1]);

			if ( 0 < flag  && flag <= 16 ) {
				flag--;

				if(gMsgLevel & (0x0001 << flag)) 
					gMsgLevel &= ~(0x0001 << flag); // OFF 
				else 
					gMsgLevel |=  (0x0001 << flag); // O N 

				Printf_("-DEBUG_FLAG_%02d %s \r\n", flag+1, (gMsgLevel & (0x0001 << flag))? "O N":"OFF");
			}
			else {
				Printf_("DebugFlag Input ERR data %d  \r\n", flag);
			}	
		}
	}
	return TRUE;
}


