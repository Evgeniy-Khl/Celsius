char NRF_read_reg(char reg) // Считываем 1 байт по указанному адресу
{
 char val;
    TIMSK=0x04;
    SPCR = SPI_MOUD_TYPE;   // SPI port enabled
    NRF_CSN_lo;
    SPDR = (reg);                   // адрес регистра.
    while ((SPSR & (1<<SPIF))==0);  // ожидаем конца передачи по SPI
    status = SPDR;
    if(reg!=STATUS)
     {
        SPDR = NOP;
        while ((SPSR & (1<<SPIF))==0);  // ожидаем конца передачи по SPI 
        val = SPDR;
     }
    NRF_CSN_hi;
    SPCR = 0x00;         //disable SPI
    TIMSK=0x05;
    return val;
}

void NRF_read_data(char cmd, char len)  // Reads an array of bytes from the given start position in the MiRF registers.
{
 char i;
    TIMSK=0x04;
    SPCR = SPI_MOUD_TYPE;// SPI port enabled
    NRF_CSN_lo;
    SPDR = (cmd);          // R_REGISTER — команда чтения регистра.
    while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
    for(i=0; i<len; i++)
     {
       SPDR = NOP; // read data
       while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
       fifo[i] = SPDR;
     }
    NRF_CSN_hi;
    SPCR = 0x00;         //disable SPI 
    TIMSK=0x05;
}

void NRF_write_cmd(char cmd, char len)  // Записываем команду и сразуже получаем STATUS register
{
 char i;
    TIMSK=0x04;
    SPCR = SPI_MOUD_TYPE;// SPI port enabled
    NRF_CSN_lo;                                // Pull down chip select
    SPDR = cmd;
    while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
    status = SPDR;
    if(len)
     {
       for(i=0; i<len; i++)
        {
           SPDR = fifo[i];     // Write data
           while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
        }
     }
    NRF_CSN_hi;                          // Pull up chip select 
    SPCR = 0x00;         //disable SPI
    TIMSK=0x05;
}

void NRF_write_byte(char reg, char value)   // Записываем 1 байт по указанному адресу
{
    TIMSK=0x04;
    SPCR = SPI_MOUD_TYPE;// SPI port enabled
    NRF_CSN_lo;
    SPDR = (W_REGISTER | (REGISTER_MASK & reg));// W_REGISTER — команда записи в регистр.
    while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
    SPDR = value;
    while ((SPSR & (1<<SPIF))==0);// ожидаем конца передачи по SPI
    NRF_CSN_hi;
    SPCR = 0x00;         //disable SPI
    TIMSK=0x05;
}

char NRF_config() 
{
 char eeprom *ptr_to_eeprom;
 char errs=0, i, *ptr_to_ram;
//---- http://narodstream.ru/stm-urok-103-moduli-nrf24l01-chast3/  ---------------------------------------------------------------
    NRF_write_byte(CONFIG,    ((1<<PWR_UP) | (1<<EN_CRC) | (0<<CRCO) | (1<<PRIM_RX)));  // Приемник
    if(NRF_read_reg(CONFIG)!= ((1<<PWR_UP) | (1<<EN_CRC) | (0<<CRCO) | (1<<PRIM_RX))) errs|=0x01;
    delay_ms(5);                        // Дадим модулю включиться, по даташиту около 1,5 мсек, а лучше 5
 //   NRF_write_byte(EN_AA, 0x00);        // отключим автоматическую отправку подтверждения приёма данных 
 //   NRF_write_byte(EN_RXADDR, 0x01);  // Enable data pipe 0 ERX _P0 = 1
 //   NRF_write_byte(SETUP_AW, 0x01);   // Setup address width=3 bytes
    NRF_write_byte(SETUP_RETR,    (SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT));
    if(NRF_read_reg(SETUP_RETR)!= (SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT)) errs|=0x02;
    NRF_write_byte(RF_CH,    NRFchannel);  // частота 2400+NRFchannel MHz
    if(NRF_read_reg(RF_CH)!= NRFchannel) errs|=0x04;
 //   NRF_write_byte(RF_SETUP, RF_SETUP_1MBPS | RF_SETUP_0DBM); // выбор скорости 1 Мбит/с и мощности 0dBm
    //--- если необходимо заменим адрес ---
    ptr_to_eeprom = &addrPipe[NRFblockAdr][0]; ptr_to_ram = fifo;
    for (i=0; i<3; i++) *ptr_to_ram++ = *ptr_to_eeprom++;  // копирование массива
    //NRF_write_cmd(TX_ADDR, 5);                           // занесём этот адрес в память передатчика
    //NRF_write_cmd(RX_ADDR_P0, 5);                        // Добавим адрес в канал pipe0 на случай, если нам придётся использовать модуль в качестве приёмника.
    NRF_write_byte(RX_PW_P0,    TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 0
    if(NRF_read_reg(RX_PW_P0)!= TX_PLOAD_WIDTH) errs|=0x08;
    NRF_write_byte(RX_PW_P1,    TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
//    if(NRF_read_reg(RX_PW_P1)!= TX_PLOAD_WIDTH) errs|=0x10;
    NRF_write_byte(STATUS, 0x70);       // Reset flags for IRQ Во время инициализации следует сбросить флаги всех прерываний 
    NRF_write_cmd(FLUSH_RX, 0);
    NRF_CE_hi;
    return errs;
}
