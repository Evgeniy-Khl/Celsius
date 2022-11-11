#define TUNING    170
void temperature_check(void)
{
  char item, byte, crc, try=0;
  unsigned char *ptr_to_ram;
  for (item=0; item < devices;)
   {
     w1_init(); // 1 Wire Bus initialization
     w1_write(0x55); // Load MATCH ROM [55H] comand
     ptr_to_ram = &familycode[item][0];
     for (byte=0; byte < 8; byte++) w1_write(*ptr_to_ram++); // Load cont. byte
     ptr_to_ram = ds.buffer;
     w1_write(0xBE); // Read Scratchpad command [BE]
     for (byte=0; byte < 8; byte++)
      {
        *ptr_to_ram = w1_read(); // Read cont. byt
        *ptr_to_ram++;
      }
     crc = w1_read(); // Read CRC byte
 //    ptr_to_ram = ds.buffer;
 //    if (w1_dow_crc8(ptr_to_ram, 8)==crc)
 //     {
       try = 0; pv.t[item] = ds.intval;
       if(pv.t[item]<0) {pv.t[item]=-pv.t[item]; pv.t[item] = pv.t[item]*10/16; pv.t[item]=-pv.t[item];} // перевод в десятичное значение !!
       else pv.t[item] = pv.t[item]*10/16; // перевод в десятичное значение !!
       //----- Коректировка датчика DS18B20 ------------------------------------------------------------------------------------------
       if(ds.buffer[2]==TUNING) pv.t[item] +=(signed char)ds.buffer[3]; // корекция показаний датчика
       //-----------------------------------------------------------------------------------------------------------------------------
 //     }
 //    else if (++try > 2) {pv.t[item] = 1990; try = 0;};          // (199) если ошибка более X раз то больше не опрашиваем
     if (try==0) item++;
   };
  w1_init(); // 1 Wire Bus initialization
  w1_write(0xCC); // Load Skip ROM [CCH] command
  w1_write(0x44); // Load Convert T [44H] command
}
/*
void copyToPload(void)
{
 unsigned char *ptr_to_ram, i;
 unsigned char eeprom *ptr_to_eeprom;

  ptr_to_ram = &pv.fifo[DATA_TEMPR + CRC8];
  ptr_to_eeprom = metering.lint;
  for (i=0; i<(LONG_INT); i++) *ptr_to_ram++ = *ptr_to_eeprom++;
}

void w1_handler(void)
{
 unsigned char byte,*p;
  byte=w1_read_slave();
  if(byte==0xEC) // состояние Тревога
   {
//     #asm("wdr")
     p = ds.buffer;
     for (byte=0; byte<4; byte++) *p++ = w1_read_slave(); // Read cont. byt
     p = ds.buffer;
     byte=w1_dow_crc8(p,3);
     SCKPORT = ON;
     if(byte==ds.buffer[3]) {NRFchannel = ds.buffer[0]; NRFblockAdr = ds.buffer[1]; NRFother = ds.buffer[2];}
   };
  TimeSlot=0; Fall=0;  // ВАЖНО на этом месте !!!
  TIMSK=0;             // TOV0 Off TOV1 Off
  GIMSK=0x20;          // Interrupt on any change on pins PCINT0-5: On;      INT0: Off
}
*/