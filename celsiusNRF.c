/*****************************************************
Project : CELSIUS_NRF
Board   : CELSIUS.17
Date    : 18.08.2019
Chip type           : ATmega8
Clock frequency     : 4,000000 MHz
*****************************************************/

//#define CELSIUS    0    // Цельсий DHT-21+DS18B20 || 1596 words (39,0%)  18.08.2019  [XXXX]
#define CELSIUS    1    // Цельсий DHT-11/DHT-21+DS18B20+радиоканал     || 2599 words (5198 bytes), 63,5% of FLASH  11.11.2022  [0xF710]

#include <mega8.h>
#include <stdlib.h>
#include <math.h>
#include <delay.h>
#include <1WIRE.h>
#include "nrf.h"
// SPI functions
#include <spi.h>

#define MAX_DEVICES 	4	// максимальное количество микросхем DS18b20 подключенных 1 Wire шине
#define DATA_TEMPR   	MAX_DEVICES * 2	// количество байт под значения датчиков
#define AMOUNT_SENS     1   // обнаружено датчиков
#define INTERVAL        1   // интервал передачи значений датчиков
#define ENERGY          1   // заряд аккумулятора
#define CRC8            1   // контрольная сумма (1 байт)
#define TX_PLOAD_WIDTH  DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY + CRC8

#define F_XTAL          4000000L// quartz crystal frequency [Hz]
#define INIT_TIMER1     TCNT1=0x10000L-F_XTAL/1024L*1L // 1 sec.
#define SCRATCHPAD      8	// DS18B20 MEMORY MAP
#define PRESET0         206	// длительность индикации одной цифры

#define RELAY	        PORTC.0 // реле
#define BEEPER          PORTC.1
#define CSNPORT         PORTC.4 // NRF CSN
#define CE_PORT         PORTC.5 // NRF CE

#define KEYPAD_PIN	    PIND.4  // клавиатура
#define LEDUP           PORTD.6
#define LEDMID          PORTD.7
#define LEDDWN          PORTD.5 

#define ON              0
#define OFF             1

#define SPI_MOUD_TYPE	0x50	// SPI Type: Master, Clock Rate: 1000,0 kHz, Clock Phase: Cycle Half, Clock Polarity: Low, Data Order: MSB First

#define BL              0xFF     // пусто
#define DEF             0xBF     // -
#define HH              0x8B     // h
#define EE              0x86     // E
#define OO              0xA3     // o
#define BB              0x83     // b
#define PER             0xDC     // п
#define CUP             0x9E     // c
//*********************************
#define KEY_UP	        0        // Up
#define KEY_DOWN	    1        // Down
#define KEY_SET	        2        // Установка
#define ON_RELAY        1
#define OFF_RELAY       0
#define ON_BEEP	        0
#define OFF_BEEP        1
//*********************************
//#define V_REF   	5
//#define ADC_RESOLUTION	512
//#define ADC_VREF_TYPE   0x47
//*********************************
// 1 Wire Bus functions
#asm
   .equ __w1_port=0x15 ; //PORTC
   .equ __w1_bit=3
#endasm

// Declare your global variables here
unsigned char keydata, BeepT, wait, waitmode, pipe, devices, counter, status, timeNRF, deviceNRF, energyNRF, setUp, DHTexist;
unsigned int  waitNRF=4;
  //--- union declaration
union{
      unsigned char ds_buffer[8];
      signed int  intval;
    } data;

unsigned char fifo[TX_PLOAD_WIDTH];

union{
     unsigned char copy[TX_PLOAD_WIDTH];
     signed int t[MAX_DEVICES];
   } nrf;
/*
union
   {
     unsigned char lint[LONG_INT];
     long int counter;
   } metering;
*/  //---
signed char mode;
signed int tC, pvT, pvRH, keySetWait, buffint;
unsigned char familycode[MAX_DEVICES][9], displ_buffer[3]={OO,OO,OO};
//			           0    1    2    3    4    5    6    7    8    9    A    B     C    D    E
//flash char digit[15]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x86,0x8E,0xA3,0xAB,0xBF};
//			           0    1    2    3    4    5    6    7    8    9    A    B     C    D    E    F
flash char digit[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x98,0x00,0xc6,0x40,0x86,0x8E};
eeprom signed int maxT=-550, minT=1270, maxAlarm=900, minAlarm=0, biasHum=0;
eeprom char NRFchannel=50, NRFblockAdr=0, Other=0; // Other = 0 -> DHT21; Other = 1 -> DHT11
eeprom char addrPipe[2][5]={{0xE7,0xE7,0xE7,0xE7,0xE7},{0x78,0x78,0x78,0x78,0x78}};

void display(signed int val, unsigned char comma);

bit Exist;
bit Check;
bit SenRH;
bit KeyDown;
bit NRF_Ok;
bit DataNew;

#include "dht11.c"
#include "nrf.c" 

// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void){
  NRF_write_cmd(NOP, 0);                                    // Любая команда возвращает STATUS register
  if(status & 0x40){DataNew = 1; waitNRF = 0; NRF_CE_lo;}   // получены новые значения температуры
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void){
 static unsigned char count=0, led=1, masck, cntMasck;
 unsigned char temp, pD;
 TCNT0 = PRESET0;
 PORTB = 0xFF;          // гасим цифру
 pD = PORTD|0x07;
 pD &= ~led;
 PORTD = pD;		// зажигаем цифру
 led <<= 1;
 temp = displ_buffer[count];
 if(!Exist || setUp) temp |=  masck; // наложим маску для эфекта мигания
 PORTB = temp;
 if (wait) --wait;            // задердка страбатывания кнопки KEY_SET
 else if(!KEYPAD_PIN){keydata = count; KeyDown=1;} // если нажата кнопка то ...
 if(BeepT){--BeepT; BEEPER = ON_BEEP;} else  BEEPER = OFF_BEEP;   // включить/отключить пищалку
 if (++count == 3){count = 0; led = 1;}// поготовим следующию цифру дисплея
 if (++cntMasck==90){ cntMasck = 0; masck = ~masck;}// частота мигания цифры индикатора

}

// Timer 1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void){
 Check = 1;
 INIT_TIMER1;
}

#include "proc.c"
#include "key.c"

void main(void){
char i;
unsigned int word;
#include "Init.c"
while (1){
  if(KeyDown){KeyDown=0; checkKey();}
  if(DataNew){
     DataNew = 0;
     NRF_read_data(R_RX_PAYLOAD, TX_PLOAD_WIDTH);
     NRF_write_cmd(FLUSH_RX, 0);
     NRF_write_byte(STATUS, status);                // Просто запишем регистр обратно, тем самым сбросив биты прерываний
     NRF_CE_hi;                                     // прослушивание эфира
     checkData();
     //convert_longint();
  }
  
  if(Check){
    #asm("wdr")
    Check = 0;
    if (++counter&1){
      if(NRF_Ok){  // Приемник присутствует и исправен
         waitNRF++; word = timeNRF*8;  // каждые 16 секунд ...
         if((waitNRF % word)==0) {NRF_CE_lo; NRF_write_cmd(FLUSH_RX, 0); NRF_write_byte(STATUS, status); NRF_CE_hi;} // Сбросить очередь приёмника
         word = timeNRF*900;           // через 30 минут ...
         if(waitNRF>word) {waitNRF = (timeNRF*8); for(i=0; i<MAX_DEVICES; i++) nrf.t[i] = 1270;}  // Индикация потери связи с передатчиком
      }
      if(mode || keySetWait){
         if(waitmode) waitmode--;
         else if(setUp==1)
          {
            if(mode>0) maxAlarm = tC; else minAlarm = tC;
            mode=0; setUp = 0; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
          }
         else if(keySetWait)
          {
            if(keySetWait<20)      {setUp = 2; waitmode = 5; keySetWait = 0; buffint = biasHum;     display(buffint,1); mode=1;} // Режим "Устанока смещения датчика влажности" 
            else if(keySetWait<40) {setUp = 3; waitmode = 5; keySetWait = 0; buffint = NRFchannel;  display(buffint,0); mode=1;} // Режим "Выбор частотного канала"
            else if(keySetWait<60) {setUp = 4; waitmode = 5; keySetWait = 0; buffint = NRFblockAdr; display(buffint,0); mode=1;} // Режим "Выбор блока адресов" 
            else if(keySetWait<80) {setUp = 5; waitmode = 5; keySetWait = 0; buffint = Other;       display(buffint,0); mode=1;} // Режим "Прочее"
            else {mode=0; setUp=0; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(0);}
          }
         else {mode=0; setUp=0; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(0);}
         if(cabs(mode)>10) BeepT = 250; else BeepT = 50;
      }
      else {
         if(pipe<=devices){
            temperature_check(pipe-1);
            display(tC,1);
            if(tC < 1270) compare_T(tC);   // если меньше 127 грд.С
            if(tC > maxAlarm) {RELAY = ON_RELAY; BeepT = 255;}
            else if(tC < minAlarm) {RELAY = ON_RELAY; BeepT = 255;}
            else RELAY = OFF_RELAY;
            switchLED(pipe);
            if(!SenRH && !NRF_Ok && pipe==devices) pipe=1; // больше нет датчиков...
            else pipe++; 
         }
         else if(NRF_Ok && (pipe - devices)<=deviceNRF){    // есть радиоприемник
            i = pipe - devices - 1;
            if(nrf.t[i] < 1270){   // радиодатчик работает
               Exist = 1;
               display(nrf.t[i],1);
               if(nrf.t[i] < 1270) compare_T(nrf.t[i]);   // если меньше 127 грд.С
               if(nrf.t[i] > maxAlarm) {RELAY = ON_RELAY; BeepT = 255;}
               else if(nrf.t[i] < minAlarm) {RELAY = ON_RELAY; BeepT = 255;}
               else RELAY = OFF_RELAY;
               switchLED(pipe);
               if(++pipe>4) pipe=1;
            }
            else {display(1993,0); switchLED(pipe); pipe++; RELAY = ON_RELAY; BeepT = 255;};
         } 
         else if(SenRH){       // есть датчик влажности
            if(readDHT()) DHTexist = 3; else if(DHTexist) DHTexist--; // датчик влажности работает?
            if(DHTexist){
               Exist = 1;
               if(pipe<=3){
                  display(pvT,1); switchLED(pipe); pipe=4;
                  if(pvT < 1270) compare_T(pvT);   // если меньше 127 грд.С
                  if(pvT > maxAlarm) {RELAY = ON_RELAY; BeepT = 255;}
                  else if(pvT < minAlarm) {RELAY = ON_RELAY; BeepT = 255;}
                  else RELAY = OFF_RELAY;
               }
               else {display(pvRH,0); switchLED(pipe); pipe=1; RELAY = OFF_RELAY;}
            }
            else {Exist = 0; display(1992,0); switchLED(4); pipe=1; RELAY = ON_RELAY; BeepT = 255;};
         }
         else pipe=1;         
      };
    };
  };
};
}
