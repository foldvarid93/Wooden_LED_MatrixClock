#define F_CPU 16000000U
#include <util/delay.h>
#include <util/twi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "characters.h"
#include "bitfields.h"
/*********************************///
#pragma GCC diagnostic ignored "-Wchar-subscripts"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-variable"
/*********************************///
#define		SDAT		portb.bit.b0
#define		SCLK		portb.bit.b1
#define		STROBE		portb.bit.b2
#define		ODAT		portb.bit.b3
#define		OCLK		portb.bit.b4
/*********************************///
//#define		SET			portb.bit.b3
//#define		INCR		portb.bit.b1
//#define		DECR		portb.bit.b2
#define		RTC_ADDR	0xD0
#define		SEC_REG		0x00
/*********************************///
typedef struct RTC_DATA{
	unsigned char	sec;
	unsigned char	min;
	unsigned char	hour;
	unsigned char	day;
	unsigned char	date;
	unsigned char	month;
	unsigned char	year;
}RTC_DATA;
typedef struct TIME{
	unsigned char hour_tens;
	unsigned char hour_singles;
	unsigned char min_tens;
	unsigned char min_singles;
	unsigned char sec_tens;
	unsigned char sec_singles;
}TIME;
typedef struct DATE{
	unsigned char day;
	unsigned char date_tens;
	unsigned char date_singles;
	unsigned char month_tens;
	unsigned char month_singles;
	const char year_thousands;
	const char year_hundreds;
	unsigned char year_tens;
	unsigned char year_singles;
}DATE;
struct {
	unsigned char TIME_SET : 1;
	unsigned char DATE_TIME :1;
} MASK;
struct {
	unsigned char B0 :1;
	unsigned char B1 :1;
	unsigned char B2 :1;
	unsigned char B3 :1;
	unsigned char B4 :1;
	unsigned char B5 :1;
	unsigned char B6 :1;
	unsigned char B7 :1;
} MASKBYTE;
/*********************************///
unsigned char minutes=0;
unsigned char IT=0, PRESS_LENGTH=0,TIMER_LENGTH=0;
unsigned char data[48];
unsigned char datatmp[256];
RTC_DATA	rtcdata;//={0,0,0,6,2,8,15};//sec,min,hour,day,date,month,year
TIME	 	time[2] ={{0,0,0,0,0,0},{0,0,0,0,0,0}};
DATE		date={0,0,0,0,0,2,0,0,0};
const unsigned char	PROGMEM szoveg[] ={"A mai dátum: "};
const unsigned char PROGMEM	weekdays[7][10]={{"hétfõ"},{"kedd"},{"szerda"},{"csütörtök"},{"péntek"},{"szombat"},{"vasárnap"}};
const unsigned char	PROGMEM months[12][12]={{"január"},{"február"},{"március"},{"április"},{"május"},{"június"},{"július"},
{"augusztus"},{"szeptember"},{"október"},{"november"},{"december"}};
/*********************************///
void fill_datatmp(char adat,char kezd){
	for(char i=0;i<6;i++){datatmp[kezd+i]=pgm_read_byte(&(characters[adat][i]));}
}
void copy(char target_from, char target_to, char source_from){
	for(char i=target_from;i<target_to;i++){
		data[i]=datatmp[i+source_from];
	}
}
void fill_display(TIME* time,char kezd){
	fill_datatmp(time->hour_tens+'0',kezd+0);
	fill_datatmp(time->hour_singles+'0',kezd+6);
	fill_datatmp(':',60);	
	fill_datatmp(time->min_tens+'0',kezd+18);
	fill_datatmp(time->min_singles+'0',kezd+24);
	fill_datatmp(':',78);		
	fill_datatmp(time->sec_tens+'0',kezd+36);
	fill_datatmp(time->sec_singles+'0',kezd+42);
}
void shift(const char a){
	STROBE=0;
	for(char i=0;i<6;i++){
		int tmp=data[((5-i)*8)+a];
		for(char j=0;j<8;j++){
			if (tmp &0x80){// 0x01){
				SDAT=0;//SDAT=1;
			}
			else SDAT=1;//SDAT=0
			SCLK=1;
			SCLK=0;
			tmp<<=1;
		}
	}
	for(int i=0;i<6;i++){
		for(char j=8;j>=1;j--){
			if(j==a+1){ODAT=1;}
			else ODAT=0;
			OCLK=1;
			OCLK=0;
		}
	}
	STROBE=1;
}
void clear(char from, char to){
	for(char i=from;i<to;i++){
		data[i]=0xFF;
	}
}
/*********************************///I2C functions
void TWIStart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}
void TWIStop(void){
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}
void TWIWrite(uint8_t u8data){
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}
char TWIReadACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
char TWIReadNACK(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
char TWIGetStatus(void){
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	return status;
}
/*********************************///
char atalakit(char tmp){
	char tizes=0;
	while(tmp>=10){
		tizes++;
		tmp-=10;
	}
	tizes<<=4;
	tizes&0x70;
	tmp&=0x0F;
	return(tizes|tmp);
}
void felbont(unsigned char tmp, unsigned char* ten, unsigned char* single){
	*single=(tmp&0x0F);
	tmp>>=4;
	*ten=(tmp&0x07);
}
/*********************************///RTC functions
unsigned char readrtcreg(unsigned char ADDR){
	unsigned char data;
	TWIStart();
	TWIWrite(RTC_ADDR);
	TWIWrite(ADDR);
	TWIStart();
	TWIWrite(RTC_ADDR+0x01);
	data=TWIReadNACK();
	TWIStop();
	return data;
}
void rtcread_time(TIME* time){
/*
	TWIStart();
	TWIWrite(RTC_ADDR);
	TWIWrite(SEC_REG);
	TWIStart();
	TWIWrite(RTC_ADDR+0x01);
	rtcdata.sec=TWIReadACK();
	rtcdata.min=TWIReadACK();
	rtcdata.hour=TWIReadNACK();
	TWIStop();*/
	rtcdata.sec=readrtcreg(0x00);
	rtcdata.min=readrtcreg(0x01);
	rtcdata.hour=readrtcreg(0x02);	
	felbont(rtcdata.hour,&time->hour_tens,&time->hour_singles);
	felbont(rtcdata.min,&time->min_tens,&time->min_singles);
	felbont(rtcdata.sec,&time->sec_tens,&time->sec_singles);
}
void rtcread_date(DATE* date){
/*
	TWIStart();
	TWIWrite(RTC_ADDR);
	TWIWrite(SEC_REG+3);
	TWIStart();
	TWIWrite(RTC_ADDR+0x01);	
	rtcdata.day=TWIReadACK();
	rtcdata.date=TWIReadACK();
	rtcdata.month=TWIReadACK();
	rtcdata.year=TWIReadNACK();
	TWIStop();*/
	rtcdata.day=readrtcreg(0x03);
	rtcdata.date=readrtcreg(0x04);
	rtcdata.month=readrtcreg(0x05);
	rtcdata.year=readrtcreg(0x06);
	date->day=rtcdata.day;
	felbont(rtcdata.date,&date->date_tens,&date->date_singles);
	felbont(rtcdata.month,&date->month_tens,&date->month_singles);
	felbont(rtcdata.year,&date->year_tens,&date->year_singles);	
}
void rtcread(TIME* time, DATE* date){
	rtcread_time(time);
	rtcread_date(date);
}
void rtcwritetime(void){
	TWIStart();
	TWIWrite(RTC_ADDR);
	TWIWrite(SEC_REG);
	TWIWrite(0);
	TWIWrite(atalakit(12));
	TWIWrite(atalakit(9));
	TWIStop();	
}
void rtcwritedate(void){
	TWIStart();
	TWIWrite(RTC_ADDR);
	TWIWrite(SEC_REG+3);
	TWIWrite(2);
	TWIWrite(atalakit(23));
	TWIWrite(atalakit(12));
	TWIWrite(atalakit(15));
	TWIStop();	
}
void first_read(){
	rtcread(&time[1],&date);
	fill_display(&time[1],48);
	copy(0,48,48);
}
/*********************************///Text functions
void text_out(const unsigned char *text){
	unsigned char length=0, temp, i=0,kar_elt=0,b=8;	//length=strlength(text);
	while(text[length]!='\0'){length++;}
	if(length<8){b=length;}
	for(char j=0;j<b;j++){
		temp=text[j];
		for(char k=0;k<6;k++,i++){
			data[i]=pgm_read_byte(&(characters[temp][k]));
		}
	}
	if(length<=8){
		for(;i<48;i++){data[i]=0xff;}
		_delay_ms(3000);
		return;
	}
	for(kar_elt=8;kar_elt<length;kar_elt++){
		temp=text[kar_elt];
		for(char j=0;j<6;j++){
			for(char l=0;l<47;l++){data[l]=data[l+1];}
			data[47]=pgm_read_byte(&(characters[temp][j]));
			_delay_ms(70);
		}
	}
	return;
}
void const_text_out(const unsigned char* text){
	for(unsigned char i=0;pgm_read_byte(&text[i])!='\0'&&i<0xFF;i++){datatmp[i]=pgm_read_byte(&text[i]);}
	text_out(datatmp);
}
/*********************************///Time functions
char concat(char b, char a){
	a>>=1;
	if(b&0x01){a|=0x80;}
	else{a&=0x7F;}
	return a;
}
void time_out(void){
	MASKBYTE.B0=MASKBYTE.B1=MASKBYTE.B2=MASKBYTE.B3=MASKBYTE.B4=MASKBYTE.B5=0;
	rtcread_time(&time[0]);
	fill_display(&time[0],0);	
	
	if (time[0].sec_singles!=time[1].sec_singles) MASKBYTE.B0=1;
	if (time[0].sec_tens!=time[1].sec_tens) MASKBYTE.B1=1;
	if (time[0].min_singles!=time[1].min_singles) MASKBYTE.B2=1;
	if (time[0].min_tens!=time[1].min_tens) MASKBYTE.B3=1;
	if (time[0].hour_singles!=time[1].hour_singles) MASKBYTE.B4=1;
	if (time[0].hour_tens!=time[1].hour_tens) MASKBYTE.B5=1;
	
	if(!MASKBYTE.B0&&!MASKBYTE.B1&&!MASKBYTE.B2&&!MASKBYTE.B3&&!MASKBYTE.B4&&!MASKBYTE.B5){
		//ez helyett vagy kéne 
	}
	else{
		for(int j=0;j<8;j++){
			if(MASKBYTE.B0){for(char i=0;i<6;i++){datatmp[90+i]=concat(datatmp[42+i],datatmp[90+i]);}}
			if(MASKBYTE.B1){for(char i=0;i<6;i++){datatmp[84+i]=concat(datatmp[36+i],datatmp[84+i]);}}
			if(MASKBYTE.B2){for(char i=0;i<6;i++){datatmp[72+i]=concat(datatmp[24+i],datatmp[72+i]);}}
			if(MASKBYTE.B3){for(char i=0;i<6;i++){datatmp[66+i]=concat(datatmp[18+i],datatmp[66+i]);}}
			if(MASKBYTE.B4){for(char i=0;i<6;i++){datatmp[54+i]=concat(datatmp[6+i],datatmp[54+i]);}}
			if(MASKBYTE.B5){for(char i=0;i<6;i++){datatmp[48+i]=concat(datatmp[i],datatmp[48+i]);}}
			for(char i=0;i<48;i++){datatmp[i]>>=1;}
			copy(0,48,48);
			_delay_ms(35);
		}
	}
	time[1]=time[0];
}
void set_time(void){
	time[0].hour_tens=minutes/60;
	time[0].min_tens=minutes%60;
	time[0].hour_singles=(time[0].hour_tens%10)+'0';
	time[0].hour_tens=(time[0].hour_tens/10)+'0';
	time[0].min_singles=(time[0].min_tens%10)+'0';
	time[0].min_tens=(time[0].min_tens/10)+'0';
	
	for(char i=0;i<48;i++){
		if (i==9)i=36;
		datatmp[i]=0xFF;
	}
	fill_datatmp(time[0].hour_tens,9);
	fill_datatmp(time[0].hour_singles,15);
	fill_datatmp(':',21);
	fill_datatmp(time[0].min_tens,27);
	fill_datatmp(time[0].min_singles,33);
	copy(0,48,0);
}
/*********************************///Date functions
void date_out(void){
	unsigned char i=0;
	rtcread_date(&date);
	for(i=0;i<8;i++){datatmp[i]=' ';}
	for(unsigned char j=0;pgm_read_byte(&szoveg[j])!='\0';i++,j++){datatmp[i]=pgm_read_byte(&szoveg[j]);}
	datatmp[i++]=date.year_thousands+'0';
	datatmp[i++]=date.year_hundreds+'0';
	datatmp[i++]=date.year_tens+'0';
	datatmp[i++]=date.year_singles+'0';
	datatmp[i++]='.';
	datatmp[i++]=date.month_tens+'0';
	datatmp[i++]=date.month_singles+'0';
	datatmp[i++]='.';
	datatmp[i++]=date.date_tens+'0';
	datatmp[i++]=date.date_singles+'0';
	datatmp[i++]='.';
	for(char j=0;pgm_read_byte(&weekdays[date.day][j])!='\0';i++,j++){
		datatmp[i]=pgm_read_byte(&weekdays[date.day][j]);
	}
	for(char j=0;j<8;i++,j++){datatmp[i]=' ';}
	datatmp[i]='\0';
	text_out(datatmp);
}
/*********************************///
void TWIInit(void){	//set SCL to 400kHz
	TWSR = 0x00;
	TWBR = 0x0C;
	TWCR = (1<<TWEN);	//enable TWI
}
void init_avr(void){
	DDRB=0xFF;
	DDRC=0xFF;
	DDRD=0b11111011;	//DDRD &= ~(1 << DDD2);     // Clear the PD2 pin	// PD2 (PCINT0 pin) is now an input
	PORTD |= (1 << PORTD2);    // turn On the Pull-up	// PD2 is now an input with pull-up enabled
	EICRA |= (1 << ISC01);    // set INT0 to trigger on ANY logic change
	EIMSK |= (1 << INT0);     // Turns on INT0
}	
void Init_Timer0(void){
	TCCR0A|=0B00000010;//setWGM01
	TCCR0A&=0B11111110;//clearWGM00
	TCCR0B|=0B00000011;//setCS01
	TCCR0B&=0B11110011;//clearWGM02, clearCS00, clear CS02		
	TIMSK0 |= (1 << TOIE0);
	OCR0A=0xFF;
}
void Init_Timer1(void){
    OCR1A = 15624;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12) | (1 << CS10);
}
/*********************************///
void main(void){
	init_avr();	
	Init_Timer0();
	sei();
	TWIInit();
	//TWIStart();
	//TWIWrite(RTC_ADDR);
	//TWIWrite(SEC_REG+7);
	//TWIWrite(0x10);
	//TWIStop();	
	//rtcwritetime();
	//rtcwritedate();
	first_read();
    while (1){
		//const_text_out(szoveg);
		//date_out();
		time_out();
		_delay_ms(25);
	}
}
/*********************************///
ISR (TIMER0_OVF_vect){
	if (IT==8){	IT=0;}
	shift(IT);
	IT++;
}
ISR (TIMER1_COMPA_vect){
	///spi kiszolgálás 
}
ISR (INT0_vect){
	portb.bit.b5=~portb.bit.b5;
}
/*********************************///
//void interrupt TIMER_ISR(void) {
	//if (PIR1bits.TMR1IF==1){
		//if(SET&&!MASK.TIME_SET){
			//TIMER_LENGTH++;
			//if(TIMER_LENGTH==100){
				//MASK.DATE_TIME=1;
				//TIMER_LENGTH=0;
			//}
		//}
		//if(!SET&&!MASK.TIME_SET){
			//MASK.TIME_SET=1;
			//TIMER_LENGTH=50;
			//rtcread(&time[0],&date);
			//minutes=time[0].min_singles+time[0].min_tens*10+((time[0].hour_singles+time[0].hour_tens*10)*60);
		//}
		//if(TIMER_LENGTH>0&&MASK.TIME_SET){
			//if (INCR == 1 & DECR == 1) {
				//PRESS_LENGTH = 0;
				//TIMER_LENGTH--;
				//if(TIMER_LENGTH==0){
					//MASK.TIME_SET=0;
					//rtcwrite_time(minutes);
				//}
			//}
			////----------------------------------------------------------------------------////NÖVELÉS
			//if (INCR == 0) {
				//if (PRESS_LENGTH <= 10) {minutes++;PRESS_LENGTH++;}
				//if (PRESS_LENGTH<30&&PRESS_LENGTH>10) {minutes += 10;PRESS_LENGTH++;}
				//if (PRESS_LENGTH >= 30) {minutes += 60;}
				//if (minutes>1439){minutes-=1440;}
			//}
			////----------------------------------------------------------------------------////CSÖKKENTÉS
			//if (DECR == 0) {
				//if (PRESS_LENGTH <= 10) {minutes--;PRESS_LENGTH++;}
				//if (PRESS_LENGTH > 10&&PRESS_LENGTH<30) {minutes-=10;PRESS_LENGTH++;}
				//if (PRESS_LENGTH >= 30) {minutes -= 60;}
				//if (minutes<0){minutes+=1440;}
			//}
		//}
		//TMR1L=0x00;
		//TMR1H=0x00;
		//PIR1bits.TMR1IF=0;
	//}
//}
		//for(char j=0;j<9;j++){
			//temp=pgm_read_byte(&(text[j]));	
			//for(char k=0;k<6;k++){
				//data[i]=pgm_read_byte(&(characters[temp][k]));		//			data[i]=characters[((*(text+j))*6)+k];
				//i++;
			//}						
		//}
		//
		//
		//while(1){
			//i=0;
			//if((length-kar_elt)==8){break;}//vége 
			//if(betu_elt!=0){
				//i=0;
				//temp=pgm_read_byte(&(text[kar_elt]));
				//for(char j=0;j<(6-betu_elt);j++,i++){
					//data[i]=pgm_read_byte(&(characters[temp][j]));
				//}
			//}
			//b=kar_elt;
			//if(betu_elt!=0){
				//b++;
			//}
			//while(i<48){
				//for(char j=0;j<6;j++,i++){
					//data[i]=pgm_read_byte(&(characters[(text+b)][j]));
				//}
				//b++;
			//}
			//betu_elt++;
			//if(betu_elt==6){
				//betu_elt=0;
				//kar_elt++;
			//}
			//_delay_ms(15);
		//}
	//}
		//for(;kar_elt+8<=length;kar_elt++){
		//i=0;
		//for(char j=0;j<8;j++){
			//temp=pgm_read_byte(&(text[kar_elt+j]));
			//for(char k=0;k<6;k++,i++){
				//data[i]=pgm_read_byte(&(characters[temp][k]));
			//}
		//}
	//}
		//if(length<9){
		//for(char j=0;j<length;j++){
			//temp=pgm_read_byte(&(text[j]));			
			//for(char k=0;k<6;k++,i++){
				//data[i]=pgm_read_byte(&(characters[temp][k]));		//			data[i]=characters[((*(text+j))*6)+k];
			//}
		//}
		//for(;i<48;i++){
			//data[i]=0xff;
		//}
		//_delay_ms(3000);
	//}
	//void text_out(const unsigned char* text){
		//unsigned char length, temp;
		//length=strlength(text);
		//for(char j=0; j<length;j++){
			//temp=pgm_read_byte(&(text[j]));
			//for(char i=0;i<6;i++){
				//data[(j*6)+i]=pgm_read_byte(&(characters[temp][i]));
			//}
		//}
	//}
//char strlength(const unsigned char* text){
	//unsigned char length=0;
	//while(pgm_read_byte(&text[length])!='\0'){
		//length++;
	//}
	//return length;
//}	
/**************************************************///inicializálások