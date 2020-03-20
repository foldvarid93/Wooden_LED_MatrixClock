#define F_CPU 16000000U
#include <util/delay.h>
#include <util/twi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "bitfields.h"

#pragma GCC diagnostic ignored "-Wchar-subscripts"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmain"
#pragma GCC diagnostic ignored "-Wparentheses"

#define DIN 	portb.bit.b3//GPIObits.GP0
#define CLK 	portb.bit.b5//GPIObits.GP1
#define CS 		portb.bit.b2//GPIObits.GP2


unsigned char 	AKTUALIS;
unsigned char 	KOVETKEZO;
unsigned char	BEMENET;
unsigned char	i;
unsigned int 	h;
unsigned const SZEGMENS[]={	
	0x00,0x01,0x21,0x7F,0xFF,0x01,0x01,0x00, //1
	0xFF,0xFF,0x30,0x18,0x0C,0x06,0xFF,0xFF, //N
	0X00,0x43,0xC7,0x8D,0x99,0xF1,0x63,0x00, //2
	0x00,0x42,0xC3,0x91,0x91,0xFF,0x6E,0x00, //3
	0x00,0x1C,0x3C,0x6C,0xDF,0x9F,0x00,0x00, //4
	0X00,0xF2,0xB1,0xB1,0xB1,0xBF,0x9E,0x00, //5
	0x00,0x7E,0xFF,0x99,0x99,0xDF,0x4E,0x00, //6
};
unsigned char SEG[8] = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

void SPI2(unsigned char ADDR, unsigned char CMD){
	CS=0;
	unsigned char i;
	for (i = 0; i < 8; i++) {
		if (ADDR & 0x80)
		DIN=1;
		else
		DIN=0;
		ADDR <<= 1;
		CLK=1;
		_delay_us(5);
		CLK=0;
	}
	for (i = 0; i < 8; i++) {
		if (CMD & 0x80)
		DIN=1;
		else
		DIN=0;
		CMD <<= 1;
		CLK=1;
		_delay_us(1);
		CLK=0;
	}
	CS=1;
}
void SPI(unsigned char ADDR, unsigned char CMD){
	CS=0;
	uint16_t DATA=ADDR;
	DATA<<=8;
	DATA|=CMD;
	for (uint8_t i = 0; i < 16; i++) {
		if (DATA & 0x8000)
		DIN=1;
		else
		DIN=0;
		DATA <<= 1;
		CLK=1;
		_delay_us(5);
		CLK=0;
	}
	CS=1;
}
void SPIKIIRAS(void){
	SPI(0x01,SEG[0]);
	SPI(0x02,SEG[1]);
	SPI(0x03,SEG[2]);
	SPI(0x04,SEG[3]);
	SPI(0x05,SEG[4]);
	SPI(0x06,SEG[5]);
	SPI(0x07,SEG[6]);
	SPI(0x08,SEG[7]);
}
void BEFORGATAS(void){
	if (KOVETKEZO>AKTUALIS){
		unsigned char j;
		for (j =0; j < 9; j++) {
			unsigned char i;
			for (i =0; i < 8;i++) {
				h=SZEGMENS[(KOVETKEZO*8)+i];
				h=h<<8;
				h|=SZEGMENS[(AKTUALIS*8)+i];
				SEG[i]=h>>j&0xff;
			}
			SPIKIIRAS();
			SPIKIIRAS();
			SPIKIIRAS();
			SPIKIIRAS();
		}
	}

	else if (AKTUALIS>KOVETKEZO){
		unsigned char j;
		for (j =0; j < 9; j++) {
			unsigned char i;
			for (i =0; i < 8;i++) {
				h=SZEGMENS[(AKTUALIS*8)+i];
				h=h<<8;
				h|=SZEGMENS[(KOVETKEZO*8)+i];
				SEG[i]=h>>(8-j)&0xff;
			}
						SPIKIIRAS();
						SPIKIIRAS();
						SPIKIIRAS();
						SPIKIIRAS();
		}
	}
	AKTUALIS=KOVETKEZO;
}
void spi_init_master (void)
{
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //Enable SPI, Set as Master
	//Prescaler: Fosc/16, Enable Interrupts
}

void spi_tranceiver (unsigned char data)
{
	SPDR = data;                       //Load data into the buffer
	while(!(SPSR & (1<<SPIF) ));       //Wait until transmission complete
}
void main(void){
	DDRB=0XFF;
	//spi_init_master();
	//spi_tranceiver(0x0c);
	//spi_tranceiver(0x01);
	
	SPI(0x0c,0x01);
	SPI(0x09,0x00);
	SPI(0x0b,0x07);
	SPI(0x0a,0x01);
	AKTUALIS=0;
	while (1) {
		for(uint8_t i=0;i!=6;i++){	
			KOVETKEZO++;
			if (KOVETKEZO!=AKTUALIS) BEFORGATAS();
			_delay_ms(1000);
		}
		for(uint8_t i=0;i!=6;i++){
			KOVETKEZO--;
			if (KOVETKEZO!=AKTUALIS) BEFORGATAS();
			_delay_ms(1000);
		}		
	}
}