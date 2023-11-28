/*
 * TEMP_HUMIDITY.c
 *
 * Created: 2022-09-08 12:14:21 AM
 * Author : masoud
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "twi_master.h"

/*****************************/
//#define SERIAL_SEND
#ifdef SERIAL_SEND
#include "stdio.h"
#endif

#define DP (1 << 7)	//Dot point is active low
#define SEGMENT_NUM		4

#define DATA_REGISTER_EMPTY (1<<UDRE0)
#define RX_COMPLETE (1<<RXC0)
#define FRAMING_ERROR (1<<FE0)
#define PARITY_ERROR (1<<UPE0)
#define DATA_OVERRUN (1<<DOR0)

/*****************************/
const uint8_t seven_seg[10] = {0x3f | DP , 0x06 | DP , 0x5b| DP , 0x4f| DP , 0x66|DP , 0x6d|DP , 0x7d|DP , 0x07|DP , 0x7f|DP , 0x6f|DP};
/*****************************/
float temp = 0 , humidity = 0;
volatile uint8_t data[SEGMENT_NUM] = {0, 0 , 0 , 0};
uint8_t idx = 0;
uint16_t num = 0 , seconds = 0;
volatile uint8_t flag = 0;
/*****************************/
ISR(TIMER2_OVF_vect){
	PORTA = DP;
	PORTC = ~( 1 << idx);
	PORTA = seven_seg[ data[idx]  ];
	idx = (idx + 1 ) % SEGMENT_NUM;
	num++;
	if( num == 500){
		flag = 1;
		seconds++;
		num = 0;
	}
}
/*ISR(TIMER0_OVF_vect){
	asm("cli");
	PORTA = DP;
	PORTC = ~( 1 << idx);
	PORTA = seven_seg[ data[idx]  ];
	idx = (idx + 1 ) % SEGMENT_NUM;
	asm("sei");
}
ISR(TIMER1_COMPA_vect){
	flag = 1;
	data[0] = (int)temp / 10;
	data[0] %= 10;
	data[1] = (int)temp % 10;

	data[2] = (int)humidity / 10;
	data[2] %= 10;
	data[3] = (int)humidity % 10;
}*/
/******************************/
#ifdef SERIAL_SEND
char may_getchar(void)
{
	char status,data;
	while (1)
	{
		while (((status=UCSR1A) & RX_COMPLETE)==0);
		data=UDR1;
		if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
		return data;
	}
}

void my_putchar(char c)
{
	while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
	UDR1=c;
}
void serial_transmit(char *buff , uint8_t len){
	for(uint8_t i = 0 ; i < len ; i++){
		my_putchar(buff[i]);
	}
}
#endif
/****************************/
int main(void)
{
	DDRE = (1 << 5);
    DDRA = 0xff;
	PORTA = DP;
	PORTC = 0x0f;
	DDRC = 0x0f;
	/*// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 250.000 kHz
	// Mode: Normal top=0xFF
	// OC0 output: Disconnected
	// Timer Period: 1.024 ms
	ASSR=0<<AS0;
	TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (1<<CS01) | (1<<CS00);
	TCNT0=0x00;
	OCR0=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 31.250 kHz
	// Mode: CTC top=OCR1A
	// OC1A output: Disconnected
	// OC1B output: Disconnected
	// OC1C output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 s
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: On
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
	TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x7A;
	OCR1AL=0x11;
	OCR1BH=0x00;
	OCR1BL=0x00;
	OCR1CH=0x00;
	OCR1CL=0x00;
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (1<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
	ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);*/
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: 125.000 kHz
	// Mode: Normal top=0xFF
	// OC2 output: Disconnected
	// Timer Period: 2.048 ms
	TCCR2=(0<<WGM20) | (0<<COM21) | (0<<COM20) | (0<<WGM21) | (0<<CS22) | (1<<CS21) | (1<<CS20);
	TCNT2=0x00;
	OCR2=0x00;
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=(0<<OCIE2) | (1<<TOIE2) | (0<<TICIE1) | (1<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
	ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);
	#ifdef SERIAL_SEND
	// USART1 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	// USART1 (Double Speed Mode)
	// USART1 Baud Rate: 115200
	UCSR1A=(0<<RXC1) | (0<<TXC1) | (0<<UDRE1) | (0<<FE1) | (0<<DOR1) | (0<<UPE1) | (1<<U2X1) | (0<<MPCM1);
	UCSR1B=(0<<RXCIE1) | (0<<TXCIE1) | (0<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1) | (0<<UCSZ12) | (0<<RXB81) | (0<<TXB81);
	UCSR1C=(0<<UMSEL1) | (0<<UPM11) | (0<<UPM10) | (0<<USBS1) | (1<<UCSZ11) | (1<<UCSZ10) | (0<<UCPOL1);
	UBRR1H=0x00;
	UBRR1L=0x08;
	#endif

	uint8_t buff[3];
	buff[0] = 0xe6;
	buff[1] = 0xBB;
	tw_init(TW_FREQ_100K , 0);
	tw_master_transmit(0x40, buff , 2 , 0 );
	_delay_ms(10);
	flag = 1;
	asm("sei");
	sleep_enable();
    while (1) 
    {
	if( flag == 1){
		buff[0] = 0xe3;
		tw_master_transmit(0x40, buff , 1 , 0 );
		tw_master_receive(0x40 , buff , 3 );
		temp = -46.85 + 175.72 * ((uint16_t)(buff[0] << 8 | buff[1]) / 65536.0);
		buff[0] = 0xe5;
		tw_master_transmit(0x40, buff , 1 , 0 );
		tw_master_receive(0x40 , buff , 3 );
		humidity = -6 + 125.00 * ((uint16_t)(buff[0] << 8 | buff[1])/ 65536.0);
		flag = 0;
		#ifdef SERIAL_SEND
		char str[50];
		uint8_t len = sprintf(str,"temp = %4.1f , humidity = %4.1f\n" , temp , humidity);
		serial_transmit(str , len);
		#endif
		data[0] = (int)temp / 10;
		data[0] %= 10;
		data[1] = (int)temp % 10;

		data[2] = (int)humidity / 10;
		data[2] %= 10;
		data[3] = (int)humidity % 10;
		//_delay_ms(500);
		}
		if(seconds == 8) PORTE = 1 << 5;
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_cpu();
	}
}

