void checkKey()
{
  static unsigned char key, count;
  if(key==keydata){++count;}
  else {count=0; keySetWait=0; key=keydata;};
  if (count>7)
   {
    count = 0; BeepT = 20; waitmode = 2;     // waitmode -> ����� ��������� ������ ���������
    if(setUp==0)    // ----------- ����� ��������� ���������� -------------------------------------------
     {
       switch (key)
        {
         case KEY_SET:                       // ���� � ����� �������������� ����� ....
            if(cabs(mode)>10){setUp = 1; wait=200;} // ����������� ����������� KEY_UP
            else if(mode>0) {maxT=-550; displ_buffer[0] = DEF; displ_buffer[1] = DEF;  displ_buffer[2] = DEF;}// ��������� ������� KEY_UP
            else if(mode<0) {minT=1270; displ_buffer[0] = DEF; displ_buffer[1] = DEF;  displ_buffer[2] = DEF;}// ��������� ������� KEY_DOWN
            if (mode == 0)                   // ���� � ����� ������������ ��������������
             {
              if(++keySetWait<20)   {displ_buffer[0] = DEF; displ_buffer[1] = HH;  displ_buffer[2] = DEF;} // -h-  �������� ��������� ������� ���������
              else if(keySetWait<40){displ_buffer[0] = DEF; displ_buffer[1] = CUP; displ_buffer[2] = DEF;} // -c-  ����� ���������� ������ 0 - 127
              else if(keySetWait<60){displ_buffer[0] = DEF; displ_buffer[1] = BB;  displ_buffer[2] = DEF;} // -b-  ����� ����� �������
              else if(keySetWait<80){displ_buffer[0] = DEF; displ_buffer[1] = OO;  displ_buffer[2] = DEF;} // -o-  ������
              mode=0; LEDUP=1; LEDMID=1; LEDDWN=1; waitmode = 1;
             };
         break;
         case KEY_UP:
            if(++mode>10) {tC = maxAlarm; display(tC,1); LEDUP=0; LEDMID=0; LEDDWN=1; mode=20;}
            else if(mode>0) {tC = maxT; display(tC,1); LEDUP=0; LEDMID=1; LEDDWN=1;}
            else mode = 1;
         break;
         case KEY_DOWN:
            if(--mode<-10) {tC = minAlarm; display(tC,1); LEDUP=1; LEDMID=0; LEDDWN=0; mode=-20;}
            else if(mode<0) {tC = minT; display(tC,1); LEDUP=1; LEDMID=1; LEDDWN=0;}
            else mode = -1;
         break;
        };
     }
    else if(setUp==1)   // ��������� ��������� ����������
     {
       switch (key)     // ����� �� ������ ���������
        {
         case KEY_SET:  // ������ ������ ����������
            if(mode>0) maxAlarm = tC; else minAlarm = tC;
            mode=0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
         break;         
         case KEY_UP:   if(++tC>1250) tC=1250; display(tC,1); delay_ms(100); break; // ������� �������� �������� MAX
         case KEY_DOWN: if(--tC<-550) tC=-550; display(tC,1); delay_ms(100); break; // ������� �������� �������� MIN
        };
     }
    else if(setUp==2)   // �������� �������� ������� ���������
     {
       switch (key)     // ����� �� ������ ���������
        {
         case KEY_SET:  // ������          NRFt = biasHum;
            biasHum = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
         break;         
         case KEY_UP:   buffint+=5; if(buffint>400)  buffint=400;  display(buffint,1); delay_ms(500); break; // �������� ������� ���������
         case KEY_DOWN: buffint-=5; if(buffint<-400) buffint=-400; display(buffint,1); delay_ms(500); break; // �������� ������� ���������
        };
     } 
    else if(setUp==3)   // ����� ���������� ������
     {
       switch (key)     // ����� �� ������ ���������
        {
         case KEY_SET:  // ������ 
            NRFchannel = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // ������������� ��������� ��������� � �����������. ���� ����� �� -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>127) buffint=127; display(buffint,0); delay_ms(500); break; // ����� ���������� ������
         case KEY_DOWN: if(--buffint<0)   buffint=0;   display(buffint,0); delay_ms(500); break; // ����� ���������� ������
        };
     }
    else if(setUp==4)   // ����� ����� �������
     {
       switch (key)     // ����� �� ������ ���������
        {
         case KEY_SET:  // ������ 
            NRFblockAdr = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // ������������� ��������� ��������� � �����������. ���� ����� �� -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>1) buffint=1; display(buffint,0); delay_ms(500); break; // ����� ����� �������
         case KEY_DOWN: if(--buffint<0) buffint=0; display(buffint,0); delay_ms(500); break; // ����� ����� �������
        };
     }
    else if(setUp==5)   // Other
     {
       switch (key)     // ����� �� ������ ���������
        {
         case KEY_SET:  // ������ 
            Other = buffint; biasHum=0; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // ������������� ��������� ��������� � �����������. ���� ����� �� -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>1) buffint=1; display(buffint,0); delay_ms(500); break; // Other = 1 -> DHT11
         case KEY_DOWN: if(--buffint<0) buffint=0; display(buffint,0); delay_ms(500); break; // Other = 0 -> DHT21
        };
     }
   };
};
