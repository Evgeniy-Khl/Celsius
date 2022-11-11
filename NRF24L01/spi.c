/*
* 15.3.2 SPI Master Operation Example  (ATtiny85 [DATASHEET] page 110) здесь описание процедуры
*/
#asm
.EQU _UsiCR=0x0D
.EQU _UsiSR=0x0e
.EQU _UsiOIF=6
#endasm

char spi_fast_shift(char data)
{
  USIDR = data;        // входные данные
  #asm
  push R16
  push R17
  ldi R16,0b00010001  ;(1<<USIWM0)|(0<<USICS0)|(1<<USITC)
  ldi R17,0b00010011  ;(1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)
  out _UsiCR,R16 ; MSB
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16
  out _UsiCR,R17
  out _UsiCR,R16 ; LSB
  out _UsiCR,R17
  pop R17
  pop R16
  #endasm
  return USIDR;
}