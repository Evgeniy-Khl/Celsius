/*****************************************************
Chip type               : ATtiny85
AVR Core Clock frequency: 8,0 MHz
Program size            : 794 words (1588 bytes), 19,4% of FLASH   30.04.2021
*****************************************************/

#include <tiny85.h>
#include <delay.h>
#include <1WIRE.h>
//#include <1wireslave.h>
#include <sleep.h>
#include "nrf.h"
#include "nRF24L01.h"

#define ON      0
#define OFF     1

#define W1PORT 	PINB.0    // 1WR;   Yello LED
#define SCKPORT	PORTB.2   // Red LED
#define CE_PORT PORTB.3
#define CSNPORT PORTB.4

#define MAX_DEVICES 	4	// максимальное количество микросхем DS18b20 подключенных 1 Wire шине
#define DATA_TEMPR   	MAX_DEVICES * 2	// количество байт под значени€ датчиков
#define AMOUNT_SENS     1   // обнаружено датчиков
#define INTERVAL        1   // интервал передачи значений датчиков
#define ENERGY          1   // зар€д аккумул€тора
#define CRC8            1   // контрольна€ сумма (1 байт)
#define TX_PLOAD_WIDTH  DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY + CRC8

#define TIMING480   50	// 50*64*0.125=400 us  (50*64*0.125=400 us)
#define PRESET48 	250	// (256 - n)*64*0.125= 48 us  ( Waits 48 us )
#define PRESET120 	241	// (256 - n)*64*0.125= 120 us ( Presence pulse 120 us )
#define PRESET260 	224	// (256 - n)*64*0.125= 256,0 us ( Waiting 260 us )
// 1 Wire Bus functions
#asm
   .equ __w1_port=0x18 ; // PORTB
   .equ __w1_bit=0     ; // PORTB.0
#endasm

// Declare your global variables here
unsigned char status, devices, faultyNrf;
unsigned char familycode[MAX_DEVICES][9];
//unsigned long int waiting;
//--- union declaration
union
   {
     unsigned char buffer[8];
     signed int  intval;
   } ds;

union
   {
     unsigned char fifo[TX_PLOAD_WIDTH];
     signed int t[MAX_DEVICES];
   } pv;
/*
eeprom union first
   {
     unsigned char lint[LONG_INT];
     unsigned long int counter;
   } metering={0};

eeprom union second
   {
     unsigned char lint[LONG_INT];
     unsigned long int counter;
   } metering2={0x84,0xD8,0x03,0};// 252804

eeprom union third
   {
     unsigned char lint[LONG_INT];
     unsigned long int counter;
   } metering3={0};
*/
eeprom char NRFchannel=50, NRFblockAdr=0/*, NRFother=0*/;  // номер радиочастотного канала, блок адрессов, прочее
eeprom char addrPipe[2][5]={{0xE7,0xE7,0xE7,0xE7,0xE7},{0x78,0x78,0x78,0x78,0x78}};
/*
bit TimeSlot;
bit Fall;
bit RstPulse;
bit Waiting;
bit PrsPulse;
*/
#include "spi.c"
#include "nrf.c"
#include "proc.c"
/*
// Pin change interrupt service routine
interrupt [PC_INT0] void pin_change_isr(void)
{
 if (W1PORT==0 && Fall==0) // прищел спад начало RESET PULSE
  {
    #asm("wdr")
    TCNT0=0;               // 256*64*0.10417=1706 us max.
    Fall=1;
  }
 else                      // пришел фронт
  {
    if (TCNT0>=TIMING480)
     {
       RstPulse=1;
       TCNT0=PRESET48; TIFR=0x02; TIMSK=0x02;// Waits 48 us
       GIMSK=0;            // INT0: Off; Interrupt on any change on pins PCINT0-5: Off
     }
    else Fall=0;           // не та длительность импульса
  };
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
  if(RstPulse)
   {
     TCNT0=PRESET120;                 // генерируем Presence pulse 120 us
     #asm
	  cbi  __w1_port,__w1_bit
	  sbi  __w1_port-1,__w1_bit       ; set 1Wr
     #endasm
     RstPulse=0; PrsPulse=1;
   }
  else if(PrsPulse)
   {
     TCNT0=PRESET260;                 // Waiting 260 us
     #asm
	  cbi  __w1_port-1,__w1_bit	      ; relise 1Wr
     #endasm
     PrsPulse=0; Waiting=1;
   }
  else if(Waiting) {Waiting=0; TimeSlot=1; TIMSK=0;}// TOV0 Off
}
*/
void main(void)
{
// Declare your local variables here
#include "init.c"
pv.t[0] = 500; pv.t[1] = 600; pv.t[2] = 700; pv.t[3] = 800;

 while (1)
 {
    if(devices) {  // все исправно измер€ем и передаем температуру
      temperature_check();
      pv.fifo[DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY] = w1_dow_crc8(&pv.fifo, DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY); // формируем CRC
//      copyToPload();   // счетчик количества измерений
      NRF_Sending();   // передача информации
      SCKPORT = OFF;   // мигаем красным LED
      sleep_enable(); powerdown();                     // выключение процессора
    }
    /*
    else if(faultyNrf) { // неисправен или отсутствует передатчики (возможно подключен к управл€ющему девайсу)
      SCKPORT = ON;
      if(TimeSlot) w1_handler();
      else waiting++;
      if(waiting == 0) {SCKPORT = OFF; sleep_enable(); powerdown();}                     // выключение процессора
    }
    */
    else { // передатчик исправен но нет датчиков
      SCKPORT = ON; 
      delay_ms(5);    // вспышка
      SCKPORT = OFF;
      delay_ms(250);  // пауза
      SCKPORT = ON;
      delay_ms(5);    // вспышка
      SCKPORT = OFF;
      sleep_enable(); powerdown();                     // выключение процессора
    }   // мигаем красным LED            
 }
}
