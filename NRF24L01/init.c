// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization                     DO       DI
// Port B init. RESET    CSN       CE        SCK       MISO/1WR  MOSI
PORTB=0x12;  // State5=T State4=1  State3=0  State2=0  State1=1  State0=T
DDRB=0x1E;   // Func5=In Func4=Out Func3=Out Func2=Out Func1=Out Func0=In

// Analog Comparator initialization
ACSR=0x80;// Analog Comparator: Off
ADCSRB=0x00;
DIDR0=0x00;

w1_init(); // 1 Wire Bus initialization
devices = w1_search(0xf0,familycode);	// detect how many DS18b20 devices are connected to the 1 Wire bus
if(devices > MAX_DEVICES) devices = MAX_DEVICES;
pv.fifo[DATA_TEMPR] = devices;          // обнаружено датчиков
faultyNrf = NRF_config();               // если faultyNrf > 0 то передатчик неисправен!
if(devices) faultyNrf = 0;              // при подключенных датчиках невозможно считать кофигурационный регистр
/*
else {
SCKPORT = OFF;                          // если датчиков нет то возможно подключен управл€ющий девайс! настроим прием данных от внещнего девайса.
// External Interrupt(s) initialization
GIMSK=0x20; // Interrupt on any change on pins PCINT0-5: On;      INT0: Off
PCMSK=0x01; // Pin Change Mask Register -> PCINT0
GIFR=0x20;

// Timer/Counter 0 initialization
TCCR0B=0x03;// Clock value: 125,000 kHz   clk/64 (From prescaler)

// Global enable interrupts
#asm("sei")
}
*/
// Watchdog Timer initialization
#pragma optsize-
 #asm("wdr")
 if(devices) {
    WDTCR=0x1F;  // Watchdog Timer Prescaler: OSC/256k  (2.0 sec)
    WDTCR=0x0F;  // Watchdog Timer interrupt: Off
    pv.fifo[DATA_TEMPR + AMOUNT_SENS] = 2; // интервал 2.0 sec
 /*
    WDTCR=0x39;  // Watchdog Timer Prescaler: OSC/1024k (8.0 sec)
    WDTCR=0x29;  // Watchdog Timer interrupt: Off
    pv.fifo[DATA_TEMPR + AMOUNT_SENS] = 8; // интервал 8.0 sec
 */ 
 }
 else {
    WDTCR=0x1E;  // Watchdog Timer Prescaler: OSC/128k  (1.0 sec)
    WDTCR=0x0E;  // Watchdog Timer interrupt: Off
 }
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

