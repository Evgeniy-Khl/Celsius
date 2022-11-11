void checkKey()
{
  static unsigned char key, count;
  if(key==keydata){++count;}
  else {count=0; keySetWait=0; key=keydata;};
  if (count>7)
   {
    count = 0; BeepT = 20; waitmode = 2;     // waitmode -> время удержания режима установок
    if(setUp==0)    // ----------- Режим измерения температур -------------------------------------------
     {
       switch (key)
        {
         case KEY_SET:                       // вход в режим редактирования после ....
            if(cabs(mode)>10){setUp = 1; wait=200;} // длительного удерживания KEY_UP
            else if(mode>0) {maxT=-550; displ_buffer[0] = DEF; displ_buffer[1] = DEF;  displ_buffer[2] = DEF;}// короткого нажатия KEY_UP
            else if(mode<0) {minT=1270; displ_buffer[0] = DEF; displ_buffer[1] = DEF;  displ_buffer[2] = DEF;}// короткого нажатия KEY_DOWN
            if (mode == 0)                   // вход в режим СПЕЦИЯЛЬНОГО редактирования
             {
              if(++keySetWait<20)   {displ_buffer[0] = DEF; displ_buffer[1] = HH;  displ_buffer[2] = DEF;} // -h-  смещение показаний датчика влажности
              else if(keySetWait<40){displ_buffer[0] = DEF; displ_buffer[1] = CUP; displ_buffer[2] = DEF;} // -c-  выбор частотного канала 0 - 127
              else if(keySetWait<60){displ_buffer[0] = DEF; displ_buffer[1] = BB;  displ_buffer[2] = DEF;} // -b-  выбор блока адресов
              else if(keySetWait<80){displ_buffer[0] = DEF; displ_buffer[1] = OO;  displ_buffer[2] = DEF;} // -o-  прочее
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
    else if(setUp==1)   // Установка граничных температур
     {
       switch (key)     // выход из режима установок
        {
         case KEY_SET:  // запись границ температур
            if(mode>0) maxAlarm = tC; else minAlarm = tC;
            mode=0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
         break;         
         case KEY_UP:   if(++tC>1250) tC=1250; display(tC,1); delay_ms(100); break; // задание опорного значения MAX
         case KEY_DOWN: if(--tC<-550) tC=-550; display(tC,1); delay_ms(100); break; // задание опорного значения MIN
        };
     }
    else if(setUp==2)   // Устанока смещения датчика влажности
     {
       switch (key)     // выход из режима установок
        {
         case KEY_SET:  // запись          NRFt = biasHum;
            biasHum = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
         break;         
         case KEY_UP:   buffint+=5; if(buffint>400)  buffint=400;  display(buffint,1); delay_ms(500); break; // смещения датчика влажности
         case KEY_DOWN: buffint-=5; if(buffint<-400) buffint=-400; display(buffint,1); delay_ms(500); break; // смещения датчика влажности
        };
     } 
    else if(setUp==3)   // Выбор частотного канала
     {
       switch (key)     // выход из режима установок
        {
         case KEY_SET:  // запись 
            NRFchannel = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // синхронизация начальных установок в передатчике. Если успех то -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>127) buffint=127; display(buffint,0); delay_ms(500); break; // выбор частотного канала
         case KEY_DOWN: if(--buffint<0)   buffint=0;   display(buffint,0); delay_ms(500); break; // выбор частотного канала
        };
     }
    else if(setUp==4)   // Выбор блока адресов
     {
       switch (key)     // выход из режима установок
        {
         case KEY_SET:  // запись 
            NRFblockAdr = buffint; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // синхронизация начальных установок в передатчике. Если успех то -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>1) buffint=1; display(buffint,0); delay_ms(500); break; // выбор блока адресов
         case KEY_DOWN: if(--buffint<0) buffint=0; display(buffint,0); delay_ms(500); break; // выбор блока адресов
        };
     }
    else if(setUp==5)   // Other
     {
       switch (key)     // выход из режима установок
        {
         case KEY_SET:  // запись 
            Other = buffint; biasHum=0; mode = 0; setUp = 0; wait=200; LEDUP=1; LEDMID=1; LEDDWN=1; carousel(1);
            PTXwrite(); // синхронизация начальных установок в передатчике. Если успех то -> BeepT = 200;
         break;         
         case KEY_UP:   if(++buffint>1) buffint=1; display(buffint,0); delay_ms(500); break; // Other = 1 -> DHT11
         case KEY_DOWN: if(--buffint<0) buffint=0; display(buffint,0); delay_ms(500); break; // Other = 0 -> DHT21
        };
     }
   };
};
