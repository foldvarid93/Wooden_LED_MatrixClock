#include "characters.h"
const unsigned char characters[256][6] ={
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0	
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},// A
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//B
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//E
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//F
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//10
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//11
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xff},//12
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//15
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//16
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//17
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//18
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//19
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1A
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1B
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1C
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1D
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1E
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1F
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//space	20H
	{0xFF,0xFF,0x41,0xFF,0xFF,0xFF},//!	21H
	{0xFF,0xF8,0xFF,0xF8,0xFF,0xFF},//"	22H
	{0xD7,0x01,0xD7,0x01,0xD7,0xFF},//#	23H
	{0xB7,0xAB,0x01,0xAB,0xDB,0xFF},//$	24H
	{0xF9,0xB9,0xDF,0xEF,0x37,0x3B},//%	25H
	{0x93,0x6D,0x55,0xBB,0x5F,0xFF},//&	26H
	{0xFF,0xFA,0xFC,0xFF,0xFF,0xFF},//'	27H
	{0xC7,0xBB,0x7D,0xFF,0xFF,0xFF},//(	28H
	{0x7D,0xBB,0xC7,0xFF,0xFF,0xFF},//)	29H
	{0xD7,0xEF,0x83,0xEF,0xD7,0xFF},//*	2AH
	{0xEF,0xEF,0x83,0xEF,0xEF,0xFF},//+	2BH
	{0x5F,0x9F,0xFF,0xFF,0xFF,0xFF},//,	2CH
	{0xEF,0xEF,0xEF,0xEF,0xEF,0xFF},//-	2DH
	{0xFF,0xFF,0x7F,0xFF,0xFF,0xFF},//.	2EH
	{0xBF,0xDF,0xEF,0xF7,0xFB,0xFF},///	2FH
	{0x83,0x5D,0x6D,0x75,0x83,0xFF},//0	30H
	{0xFF,0x7B,0x01,0x7F,0xFF,0xFF},//1	31H
	{0x7B,0x3D,0x5D,0x6D,0x73,0xFF},//2	32H
	{0xBD,0x7D,0x75,0x69,0x9D,0xFF},//3	33H
	{0xCF,0xD7,0xDB,0x01,0xDF,0xFF},//4	34H
	{0xB1,0x75,0x75,0x75,0x8D,0xFF},//5	35H
	{0x87,0x6B,0x6D,0x6D,0x9F,0xFF},//6	36H
	{0xFD,0x1D,0xED,0xF5,0xF9,0xFF},//7	37H
	{0x93,0x6D,0x6D,0x6D,0x93,0xFF},//8	38H
	{0xF3,0x6D,0x6D,0xAD,0xC3,0xFF},//9	39H
	{0xFF,0xFF,0xBB,0xFF,0xFF,0xFF},//:	3AH
	{0xFF,0x53,0x93,0xFF,0xFF,0xFF},//;	3BH
	{0xEF,0xD7,0xBB,0x7D,0xFF,0xFF},//<	3CH
	{0xD7,0xD7,0xD7,0xD7,0xD7,0xFF},//=	3DH
	{0x7D,0xBB,0xD7,0xEF,0xFF,0xFF},//>	3EH
	{0xFB,0xFD,0x5D,0xED,0xF3,0xFF},//?	3FH
	{0x9B,0x6D,0x0D,0x7D,0x83,0xFF},//@	40H
	{0x03,0xDD,0xDD,0xDD,0x03,0xFF},//A	41H
	{0x01,0x6D,0x6D,0x6D,0x93,0xFF},//B	42H
	{0x83,0x7D,0x7D,0x7D,0xBB,0xFF},//C	43H
	{0x01,0x7D,0x7D,0x7D,0x83,0xFF},//D	44H
	{0x01,0x6D,0x6D,0x6D,0x7D,0xFF},//E	45H
	{0x01,0xED,0xED,0xED,0xFD,0xFF},//F	46H
	{0x83,0x7D,0x6D,0x6D,0x8B,0xFF},//G	47H
	{0x01,0xEF,0xEF,0xEF,0x01,0xFF},//H	48H
	{0xFF,0x7D,0x01,0x7D,0xFF,0xFF},//I	49H
	{0xBF,0x7F,0x7D,0x81,0xFD,0xFF},//J	4AH
	{0x01,0xEF,0xD7,0xBB,0x7D,0xFF},//K	4BH
	{0x01,0x7F,0x7F,0x7F,0x7F,0xFF},//L	4CH
	{0x01,0xFB,0xE7,0xFB,0x01,0xFF},//M	4DH
	{0x01,0xF7,0xEF,0xDF,0x01,0xFF},//N	4EH
	{0x83,0x7D,0x7D,0x7D,0x83,0xFF},//O	4FH
	{0x01,0xED,0xED,0xED,0xF3,0xFF},//P	50H
	{0x83,0x7D,0x5D,0xBD,0x43,0xFF},//Q	51H
	{0x01,0xED,0xCD,0xAD,0x73,0xFF},//R	52H
	{0x73,0x6D,0x6D,0x6D,0x9D,0xFF},//S	53H
	{0xFD,0xFD,0x01,0xFD,0xFD,0xFF},//T	54H
	{0x81,0x7F,0x7F,0x7F,0x81,0xFF},//U	55H
	{0xC1,0xBF,0x7F,0xBF,0xC1,0xFF},//V	56H
	{0x81,0x7F,0x8F,0x7F,0x81,0xFF},//W	57H
	{0x39,0xD7,0xEF,0xD7,0x39,0xFF},//X	58H
	{0xF1,0xEF,0x1F,0xEF,0xF1,0xFF},//Y	59H
	{0x3D,0x5D,0x6D,0x75,0x79,0xFF},//Z	5AH
	{0x01,0x7D,0x7D,0xFF,0xFF,0xFF},//[	5BH
	{0xFD,0xFB,0xF7,0xEF,0xDF,0xBF},//\	5CH
	{0x7D,0x7D,0x01,0xFF,0xFF,0xFF},//]	5DH
	{0xF7,0xFB,0xFD,0xFB,0xF7,0xFF},//^	5EH
	{0x7F,0x7F,0x7F,0x7F,0x7F,0xFF},//_	5FH
	{0xFF,0xFD,0xFB,0xF7,0xFF,0xFF},//`	60H
	{0xBF,0x57,0x57,0x57,0x0F,0xFF},//a	61H
	{0x01,0x6F,0x77,0x77,0x8F,0xFF},//b	62H
	{0x8F,0x77,0x77,0x77,0xBF,0xFF},//c	63H
	{0x8F,0x77,0x77,0x6F,0x01,0xFF},//d	64H
	{0x8F,0x57,0x57,0x57,0xCF,0xFF},//e	65H
	{0xEF,0x03,0xED,0xFD,0xFB,0xFF},//f	66H
	{0xE7,0x5B,0x5B,0x5B,0x83,0xFF},//g	67H
	{0x01,0xEF,0xF7,0xF7,0x0F,0xFF},//h	68H
	{0xFF,0x77,0x05,0x7F,0xFF,0xFF},//i	69H
	{0xBF,0x7F,0x77,0x85,0xFF,0xFF},//j	6AH
	{0x01,0xDF,0xAF,0x77,0xFF,0xFF},//k	6BH
	{0xFF,0x7D,0x01,0x7F,0xFF,0xFF},//l	6CH
	{0x07,0xF7,0xCF,0xF7,0x07,0xFF},//m	6DH
	{0x07,0xEF,0xF7,0xF7,0x0F,0xFF},//n	6EH
	{0x8F,0x77,0x77,0x77,0x8F,0xFF},//o	6FH
	{0x07,0xD7,0xD7,0xD7,0xEF,0xFF},//p	70H
	{0xEF,0xD7,0xD7,0xCF,0x07,0xFF},//q	71H
	{0x07,0xEF,0xF7,0xF7,0xEF,0xFF},//r	72H
	{0x6F,0x57,0x57,0x57,0xB7,0xFF},//s	73H
	{0xF7,0x81,0x77,0x7F,0xBF,0xFF},//t	74H
	{0x87,0x7F,0x7F,0xBF,0x07,0xFF},//u	75H
	{0xC7,0xBF,0x7F,0xBF,0xC7,0xFF},//v	76H
	{0x87,0x7F,0x9F,0x7F,0x87,0xFF},//w	77H
	{0x77,0xAF,0xDF,0xAF,0x77,0xFF},//x	78H
	{0xE7,0x5F,0x5F,0x5F,0x87,0xFF},//y	79H
	{0x77,0x37,0x57,0x67,0x77,0xFF},//z	7AH
	{0xEF,0x93,0x7D,0xFF,0xFF,0xFF},//"kapcsosz�r�jel jobb"	7BH
	{0xFF,0xFF,0x00,0xFF,0xFF,0xFF},//|	7CH
	{0x7D,0x93,0xEF,0xFF,0xFF,0xFF},//"kapcsos bal"	7DH
	{0xEF,0xEF,0xAB,0xC7,0xEF,0xFF},//->	7EH
	{0xEF,0xC7,0xAB,0xEF,0xEF,0xFF},//<-	7FH
	{0xFF,0xEF,0xEF,0xEF,0xEF,0xEF},//80H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//81H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//82H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//83H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//84H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//85H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//86H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//87H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//88H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//89H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8AH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8BH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8CH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8DH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8EH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8FH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//90H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//91H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//92H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//93H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//94H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//95H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//96H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//97H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//98H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//99H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9AH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9BH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9CH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9DH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9DH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9FH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A0H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A2H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A4H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A5H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A8H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//A9H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//AAH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//ABH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//ACH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//ADH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//AEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//AFH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B0H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B2H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B4H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B5H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B8H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//B9H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BAH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BBH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BCH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BDH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//BFH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C0H
	{0x03,0xDD,0xD8,0xDD,0x03,0xFF},//�	C1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C2H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C4H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C5H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//C8H
	{0x01,0x6D,0x68,0x6D,0x7D,0xFF},//�	C9H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//CAH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//CBH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//CCH
	{0x77,0x04,0x77,0xFF,0xFF,0xFF},//�	CDH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//CEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//CFH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D0H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D2H
	{0x83,0x7D,0x78,0x7D,0x83,0xFF},//�	D3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D4H
	{0x87,0x78,0x7B,0x78,0x87,0xFF},//�	D5H
	{0x87,0x7A,0x7B,0x7A,0x87,0xFF},//�	D6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D8H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//D9H
	{0x83,0x7F,0x7C,0x7F,0x83,0xFF},//�	DAH
	{0x83,0x7C,0x7F,0x7C,0x83,0xFF},//�	DBH
	{0x83,0x7E,0x7F,0x7E,0x83,0xFF},//�	DCH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//DDH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//DEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//DFH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E0H
	{0xBF,0x57,0x54,0x57,0x0F,0xFF},//�	E1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E2H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E4H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E5H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//E8H
	{0x8F,0x57,0x54,0x57,0xCF,0xFF},//�	E9H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//EAH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//EBH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//ECH
	{0x77,0x04,0x7F,0xFF,0xFF,0xFF},//�	EDH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//EEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//EFH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F0H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F1H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F2H
	{0x8F,0x77,0x74,0x77,0x8F,0xFF},//�	F3H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F4H
	{0x8F,0x74,0x77,0x74,0x8F,0xFF},//�	F5H
	{0x8F,0x75,0x77,0x75,0x8F,0xFF},//�	F6H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F7H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F8H
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//F9H
	{0x87,0x7F,0x7C,0xBF,0x07,0xFF},//�	FAH
	{0x87,0x7C,0x7F,0xBC,0x07,0xFF},//�	FBH
	{0x87,0x7D,0x7F,0xBD,0x07,0xFF},//�	FCH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//FDH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//FEH
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}//FFH		
};	