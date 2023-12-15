
#define LINE_BUFFER_MAX         					200
//#define SENSOR_BUFFER_MAX         				20

//#define SENSOR_STX												'<'
//#define SENSOR_ETX												'>'

#define UART1_SERIAL            					1
#define UART2_SERIAL            					2
#define UART3_SERIAL            					3

//#define 			VER 												"2021.04 Ver 0.0"


typedef struct {
  uint16_t   head;
  uint16_t   tail;
  uint16_t   buffering_cnt;
  uint16_t   rx_buf[LINE_BUFFER_MAX];
  uint16_t   tx_buf[LINE_BUFFER_MAX];
  uint16_t   TxHead;
  uint16_t   TxTail;
	uint32_t   revTime;
	uint8_t    revFlag;

} COM_RING_BUFF;

/***************************************************************************************************/
//--CONTOROL  CHARACTER-------------------------------------------------------//
#define        	NUL					0x00 
#if 0
#define        	SOH					0x01 
#define        	STX        	0x02 
#define        	ETX        	0x03 
#define        	EOT        	0x04 
#define        	ENQ        	0x05 
#define        	ACK        	0x06 
#define        	BEL        	0x07 
#define        	BS         	0x08 
#define        	HT         	0x09 
#define        	LF         	0x0a 
#define        	VT         	0x0b 
#define        	FF         	0x0c 
#define        	CR         	0x0d 
#define        	SO         	0x0e 
#define        	SI         	0x0f 
#define        	DLE        	0x10 
#define        	DC1        	0x11 
#define        	DC2        	0x12 
#define        	DC3        	0x13 
#define        	DC4        	0x14 
#define        	NAK        	0x15 
#define        	SYN        	0x16 
#define        	ETB        	0x17 
#define        	CAN        	0x18 
#define        	EM         	0x19 
#define        	SUB        	0x1a 
#endif
#define        	BS         	0x08 
#define        	ENTER      	0x0d 
#define        	ESC        	0x1b 
#define        	BACK	    	0xE0
#define        	LF         	0x0a 

#if 0
#define        	FS         	0x1c 
#define        	GS         	0x1d 
#define        	RS         	0x1e 
#define        	US         	0x1f 
#define        	DEL        	0x7f  
#define		   	TIMEOUT    		(-2)
#endif
/*****************************************************************************************************/
#define MAX_STRING_LENGTH 								255
#define MAX_TOKS 													100
#define DELIMITERS 												" \t"
/*****************************************************************************************************/

typedef enum {                  
  DEBUG_FLAG_0000 = 0x0000,           // 0x0000
  DEBUG_FLAG_0001 = 0x0001,           // 0x0001		
  DEBUG_FLAG_0002 = 0x0002,           // 0x0002		
  DEBUG_FLAG_0003 = 0x0004,           // 0x0004		
  DEBUG_FLAG_0004 = 0x0008,           // 0x0008		
  DEBUG_FLAG_0005 = 0x0010,           // 0x0010		
  DEBUG_FLAG_0006 = 0x0020,           // 0x0020		
  DEBUG_FLAG_0007 = 0x0040,           // 0x0040		
  DEBUG_FLAG_0008 = 0x0080,           // 0x0080		
  DEBUG_FLAG_0009 = 0x0100,           // 0x0100
  DEBUG_FLAG_0010 = 0x0200,           // 0x0200		
  DEBUG_FLAG_0011 = 0x0400,           // 0x0400		
  DEBUG_FLAG_0012 = 0x0800,           // 0x0800		
  DEBUG_FLAG_0013 = 0x1000,           // 0x1000		
  DEBUG_FLAG_0014 = 0x2000,           // 0x2000		
  DEBUG_FLAG_0015 = 0x4000,           // 0x4000		
  DEBUG_FLAG_0016 = 0x8000,           // 0x8000	

  DEBUG_FLAG_0020 = 0x0020,           // 0x0020		
  DEBUG_FLAG_0040 = 0x0040,           // 0x0040		
  DEBUG_FLAG_0080 = 0x0080,           // 0x0080		
  DEBUG_FLAG_0100 = 0x0100,           // 0x0100		
  DEBUG_FLAG_0200 = 0x0200,           // 0x0200		
  DEBUG_FLAG_0400 = 0x0400,           // 0x0400		
  DEBUG_FLAG_0800 = 0x0800,           // 0x0800		
  DEBUG_FLAG_1000 = 0x1000,           // 0x1000		
  DEBUG_FLAG_2000 = 0x2000,           // 0x2000		
  DEBUG_FLAG_4000 = 0x4000,           // 0x4000		
  DEBUG_FLAG_8000 = 0x8000,           // 0x8000		

	DEBUG_FLAG_ALL  = 0xFFFF
	
} D_FLAG_t;


extern uint32_t gMsgLevel;

/*****************************************************************************************************/

void vPutHex(uint8_t port, uint8_t number);
uint16_t Ascii_4_int(uint8_t *data);
uint8_t AsciiToHex(uint8_t hex);
uint8_t Ascii_2_Hex(uint8_t num1, uint8_t num2);
void Hex_4_ascii(uint8_t *data, uint16_t num);
void Hex_2_ascii(uint8_t *data, uint8_t num);
uint8_t my_htoi(char *Str, int16_t *ret);
uint8_t my_htol(char *Str, int32_t *ret);

int hex_convert_hexstring(uint8_t *hexData, uint8_t len, uint8_t *string);
int hexstring_conver_hex(uint8_t *string, uint8_t len, uint8_t *hexData);

void Printf_(const char *format, ...);  

void vPutChar(uint8_t port, uint8_t data);
void vPrintf(uint8_t port, const char * string, ...);
void ChangeUpperChar( char *Str );
void my_toupper( char *Str );
uint8_t my_atoi(char *Str, int *ret);
uint8_t Parse_Args(char *cmdline, char **argv);

void UartInit(void);
void UART1_para_init(void);
//void processUart1Input(void);

void UART2_para_init(void);
uint16_t UART2_getc(void);
//void processUart2Input(void);

void UART3_para_init(void);
uint16_t UART3_getc(void);


uint8_t CheckSum(uint8_t *data, uint8_t len);

uint8_t compileInfo(uint8_t argc, char **argv);
void MSGPrint_(uint32_t flag, const char *format, ...);
uint8_t CommonDebugFlagOn(uint8_t argc, char **argv);
char Terminal_Services_Function(void);
uint8_t helpMenu(uint8_t argc, char **argv);

uint8_t SoftReset(uint8_t argc, char **argv);
uint8_t WatchDogReset(uint8_t argc, char **argv);

