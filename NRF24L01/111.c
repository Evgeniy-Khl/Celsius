/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Professional
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 05.08.2019
Author  : 
Company : 
Comments: 


Chip type               : ATtiny85
AVR Core Clock frequency: 9,600000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*****************************************************/

#include <tiny85.h>

// Pin change interrupt service routine
interrupt [PC_INT0] void pin_change_isr(void)
{
// Place your code here

}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Func5=In Func4=In Func3=In Func2=In Func1=Out Func0=In 
// State5=T State4=T State3=T State2=T State1=0 State0=T 
PORTB=0x00;
DDRB=0x02;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Timer1 Overflow Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
PLLCSR=0x00;

TCCR1=0x00;
GTCCR=0x00;
TCNT1=0x00;
OCR1A=0x00;
OCR1B=0x00;
OCR1C=0x00;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-5: On
GIMSK=0x20;
MCUCR=0x00;
PCMSK=0x20;
GIFR=0x20;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Universal Serial Interface initialization
// Mode: Three Wire (SPI)
// Clock source: Register & Counter=USICLK
// USI Counter Overflow Interrupt: Off
USICR=0x12;

// Analog Comparator initialization
// Analog Comparator: Off
ACSR=0x80;
ADCSRB=0x00;
DIDR0=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// Global enable interrupts
#asm("sei")

while (1)
      {
      // Place your code here

      }
}
