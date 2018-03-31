#ifndef MYBUS_H_
#define MYBUS_H_


#define STX 0x55
#define ETX 0xAA

#define MYBUS_STX 0
#define MYBUS_SRC0 1
#define MYBUS_SRC1 2
#define MYBUS_DEST0 3
#define MYBUS_DEST1 4
#define MYBUS_MSG_NUMBER 5
#define MYBUS_FUNCT 6
#define MYBUS_FLAGS 7
#define MYBUS_DATALEN 8
#define MYBUS_DATA	9
#define MYBUS_CRC1 10
#define MYBUS_CRC2 11
#define MYBUS_ETX 12
#define MYBUS_END 13

//extern unsigned char app_version_data[100];
typedef struct
{
	unsigned long tx_msg_cnt;
	unsigned long rx_msg_cnt;
	unsigned long rx_total_msg_cnt;
	unsigned long rx_crc_err_cnt;
	unsigned long rx_etx_err_cnt;
	unsigned long rx_timeout_cnt;
}t_mybus_stats;

typedef struct
{
	char type;
	char state;
	int cnt;
	int maxlen;
	unsigned short src;
	unsigned short dest;
	unsigned char msg_number;
	unsigned char funct;
	unsigned char flags;
	unsigned char datalen;
	unsigned char * data;
	unsigned short crc;
	unsigned short crc_calc;
	t_mybus_stats stats;
} t_mybus;


extern unsigned short mod_addr;// = BL_485_ADDR;
extern t_mybus_stats mybus_stats;

int mybus_rx_init(t_mybus * dev,int len);
int mybus_rx_event(t_mybus * dev,char rxchar);

int mybus_tx_data(t_mybus * dev,int(*fputchar)(char));


void mybus_init(void);
void mybus_main(void);


#endif
