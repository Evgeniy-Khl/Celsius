// Port B initialization
PORTB=0xFF;// State7=1 State6=1 State5=1 State4=1 State3=1 State2=1 State1=1 State0=1
DDRB=0xFF;// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 

// Port C initialization  CE=0    CSN=1
PORTC=0x12;// State6=T State5=0 State4=1 State3=T State2=T State1=1 State0=0 
DDRC=0x33;// Func6=In Func5=Out Func4=Out Func3=In Func2=In Func1=Out Func0=Out 

// Port D initialization
PORTD=0xEF;// State7=1 State6=1 State5=1 State4=T State3=P State2=1 State1=1 State0=1
DDRD=0xE7;// Func7=Out Func6=Out Func5=Out Func4=In Func3=In Func2=Out Func1=Out Func0=Out

// Timer/Counter 0 initialization
TCCR0=0x04;// Clock value: 15,625 kHz

// Timer/Counter 1 initialization
TCCR1B=0x05;// Clock value: 3,906 kHz
INIT_TIMER1;

// Timer/Counter 2 initialization
TCCR2=0x00;// Clock value: Timer 2 Stopped

// External Interrupt(s) initialization
GICR=0x00;
//MCUCR=0x00;  // INT1 Mode: Low level
MCUCR=0x08;  // INT1 Mode: Falling Edge

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x05;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC disabled
ADCSRA=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;
SPSR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// NRF initialization
//while (1) {NRF_write_cmd(0x55, 0); delay_us(10);}              // Любая команда возвращает STATUS register
NRF_write_cmd(STATUS, 0);
if(status<=0x0E) NRF_Ok = 1;
//if(NRFamount==0) NRF_Ok = 0;            // если не указаны радиопередатчики
//displ_buffer[1]=digit[status>>4];  
//displ_buffer[2]=digit[status&0x0F];
// 1 Wire Bus initialization
w1_init(); // 1 Wire Bus initialization
devices = w1_search(0xf0,familycode);	// detect how many DS18b20 devices are connected to the 1 Wire bus

#asm("sei")                             // Global enable interrupts

if (devices < MAX_DEVICES)              // если датчиков мало ...
 {
  if(NRF_Ok)                            // подключаем приемник NRF24L01
   {
     waitmode = NRF_config();
     if(waitmode==0){GIFR=0x80; GICR|=0x80;}  // INT0: Off;    INT1: On;   
   }
 }
else devices=MAX_DEVICES;		        // если датчиков много то оставляем только три
displ_buffer[0] = digit[devices];      	// выводим на индикатор количество обнаруженных датчиков.
w1_init();                              // 1 Wire Bus initialization
w1_write(0xCC);                         // Load Skip ROM [CCH] command
w1_write(0x44);                         // Load Convert T [44H] command

displ_buffer[1]=DEF;
if(NRF_Ok) displ_buffer[1]=PER;         // если приемник обнаружен  "п" 
if(waitmode) {displ_buffer[1]=digit[waitmode]; NRF_Ok=0;} // если приемник неисправен  выводим номер ошибки
delay_ms(1000);                         // задержка на включение модуля 
if(devices < MAX_DEVICES) SenRH=readDHT();// detect DHT-21/11
if(SenRH) displ_buffer[2]=HH;           // если датчик обнаружен  "-h"

BeepT = 200;
pipe = 1; // начинаем с 1 канала
delay_ms(2000);

// Watchdog Timer initialization
#pragma optsize-
WDTCR=0x1F;
WDTCR=0x0F;// Watchdog Timer Prescaler: OSC/2048k
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
#asm("wdr")
