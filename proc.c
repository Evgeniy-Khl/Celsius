#define TUNING    170
void temperature_check(unsigned char item)
{
  char *ptr_to_ram, byte, crc, try=0; 
  do
   {
     w1_init(); // 1 Wire Bus initialization
     w1_write(0x55); // Load MATCH ROM [55H] comand
     ptr_to_ram = &familycode[item][0];
     for (byte=0; byte < 8; byte++) w1_write(*ptr_to_ram++); // Load cont. byte
     ptr_to_ram = data.ds_buffer;
     w1_write(0xBE); // Read Scratchpad command [BE]
     for (byte=0; byte < 8; byte++)
      {
        *ptr_to_ram = w1_read(); // Read cont. byt
        *ptr_to_ram++;
      }
     crc = w1_read(); // Read CRC byte
     ptr_to_ram = data.ds_buffer;
     if (w1_dow_crc8(ptr_to_ram, 8)==crc){try = 0; tC = data.intval; Exist = 1;}
     else if (++try > 2) {tC = 0x0C70; try = 0; Exist = 0;};// (199) ���� ������ ����� X ��� �� ������ �� ����������
   }
  while (try);
  if(tC<0) {tC=-tC; tC = tC*10/16; tC=-tC;} else tC = tC*10/16;// ������� � ���������� �������� !!
  //----- ������������ ������� DS18B20 ------------------------------------------------------------------------------------------
#pragma warn-
     if(data.ds_buffer[2]==TUNING) tC +=(signed char)data.ds_buffer[3]; // �������� ��������� �������
#pragma warn+
  //-----------------------------------------------------------------------------------------------------------------------------
  w1_init(); // 1 Wire Bus initialization
  w1_write(0xCC); // Load Skip ROM [CCH] command
  w1_write(0x44); // Load Convert T [44H] command
}

void display(signed int val, unsigned char comma)
{
 unsigned char neg=0;
  if(comma) comma=0x7F; else comma=0xFF;
  if(val<0) {neg=1; val=-val;}
  if(neg)
     {
       if(val<100)
        {
          displ_buffer[0] = DEF;
          displ_buffer[1] = digit[((val/10)%10)&0x0F]&comma; // �������
          displ_buffer[2] = digit[(val%10)&0x0F];
        }
       else
        {
          displ_buffer[0] = DEF;
          displ_buffer[1] = digit[(val/100)&0x0F];
          displ_buffer[2] = digit[((val/10)%10)&0x0F];
        };
     }
  else if(val<100)
     {
       displ_buffer[0] = BL;
       displ_buffer[1] = digit[((val/10)%10)&0x0F]&comma; // �������
       displ_buffer[2] = digit[(val%10)&0x0F];
     }
  else if(val<1000)
     {
       displ_buffer[0] = digit[(val/100)&0x0F];
       displ_buffer[1] = digit[((val/10)%10)&0x0F]&comma; // �������
       if(comma==0x7F) displ_buffer[2] = digit[(val%10)&0x0F];
       else displ_buffer[2] = PER; // ���� ������� ��� �� � ������� ���� �� ����������
     }
  else if(val<1990)
     {
       val/=10;
       displ_buffer[0] = digit[(val/100)&0x0F];
       displ_buffer[1] = digit[((val/10)%10)&0x0F];
       displ_buffer[2] = digit[(val%10)&0x0F];
     }
  else 
     {
       val %= 10;
       switch (val)    // val = 1993; 1992
        {
          case 3: displ_buffer[0] = OO; displ_buffer[1] = PER; displ_buffer[2] = OO; break;  // ������� �����������
          case 2: displ_buffer[0] = OO; displ_buffer[1] = HH; displ_buffer[2] = OO;  break;  // ������� ������ ���������
          case 0: displ_buffer[0] = OO; displ_buffer[1] = OO; displ_buffer[2] = OO;  break;  // ������� ������ �����������     
        }; 
     };
}

void compare_T(int pvVal)
{
signed int temp;
 temp = max(pvVal, maxT);
 if (maxT!=temp) maxT=temp;
temp = min(pvVal, minT);
 if (minT!=temp) minT=temp;
}

void carousel(char d)
{
 char i,j,c=2;
 for (j=0; j<5; j++)
  {
   for (i=0; i<3; i++) displ_buffer[i] = ~c;
   c<<=1; delay_ms(100);
  }
 if(d)
  {
   for (i=0; i<3; i++) displ_buffer[i] = 0xFE; delay_ms(500);
   for (i=0; i<3; i++) displ_buffer[i] = 0xBF; delay_ms(500);
   for (i=0; i<3; i++) displ_buffer[i] = 0xF7; delay_ms(500);
  };
}

void switchLED(char n)
{
 switch (n)
  {
   case 1: LEDUP=0; LEDMID=1; LEDDWN=0; break;
   case 2: LEDUP=1; LEDMID=0; LEDDWN=1; break;
   case 3: LEDUP=0; LEDMID=0; LEDDWN=0; break;
   default:LEDUP=1; LEDMID=1; LEDDWN=1;
  };
}

void checkData(void)
{
 char *ptr_to_ram1, *ptr_to_ram0, i;  
     
     ptr_to_ram0 = fifo;
     if(w1_dow_crc8(ptr_to_ram0, DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY)==fifo[DATA_TEMPR + AMOUNT_SENS + INTERVAL + ENERGY])
      {
        ptr_to_ram0 = fifo; ptr_to_ram1 = nrf.copy;
        for (i=0; i<(TX_PLOAD_WIDTH); i++) *ptr_to_ram1++ = *ptr_to_ram0++; // ����������� �������
        deviceNRF = nrf.copy[DATA_TEMPR];                                   // ���������� ��������
        timeNRF   = nrf.copy[DATA_TEMPR + AMOUNT_SENS];                     // �������� �������� �������� ��������
        energyNRF = nrf.copy[DATA_TEMPR + AMOUNT_SENS + INTERVAL];          // ����� ������������
      }
}

void PTXwrite(void)      // ������������� ��������� ��������� � �����������
{
 char eeprom *ptr_to_eeprom;
 char crc, *ptr_to_ram;
  ptr_to_eeprom = &NRFchannel; ptr_to_ram = fifo;
  for (crc=0; crc<3; crc++) *ptr_to_ram++ = *ptr_to_eeprom++; // ����������� �������
  ptr_to_ram = fifo;
  crc = w1_dow_crc8(ptr_to_ram, 3);    // ������ CRC
  fifo[3] = crc;
  crc = w1_init(); // 1 Wire Bus initialization
  if(crc) BeepT = 200;
  w1_write(0xEC); // ��������� ������� command
  for (crc=0; crc<4; crc++) w1_write(fifo[crc]);  
}
/*
void convert_longint(void)
{
 char *ptr_to_ram1, *ptr_to_ram0, i;
     ptr_to_ram0 = &fifo[DATA_TEMPR + CRC8];
     ptr_to_ram1 = metering.lint;
     for (i=0; i<(LONG_INT); i++) *ptr_to_ram1++ = *ptr_to_ram0++; // ����������� �������
}
*/