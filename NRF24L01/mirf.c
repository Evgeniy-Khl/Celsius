
// Pin change interrupt service routine
interrupt [PC_INT0] void pin_change_isr(void)
// Interrupt handler 
{     
    NRF_write_cmd(NOP, 0);                          // Любая команда возвращает STATUS register
    // If still in transmitting mode then finish transmission
/*    if (ptx)
     {
        NRF_Mode(MODE_PRX);                            // Power up in receiving mode
        NRF_write_byte(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Clear flags "TX_DS", "MAX_RT"
     }  */
}

char NRF_read_register(char addr) // Read register
{
 char data;
    if (ptx) return 0;
    NRF_CSN_lo;                                // Pull down chip select
    spi_fast_shift(addr);                      // Address of register
    data = spi_fast_shift(NOP);                // Read register
    NRF_CSN_hi;                                // Pull up chip select
    return data;
}

void NRF_write_cmd(char cmd, char len)// Записываем команду и сразуже получаем STATUS register
{
 char i;
    NRF_CSN_lo;                                // Pull down chip select
    status = spi_fast_shift(cmd);
    if(len)
     {
       for(i=0; i<len; i++) spi_fast_shift(buffer[i]);     // Write data
     }
    NRF_CSN_hi;                                // Pull up chip select
}

void NRF_write_byte(char reg, char value)// Writes only one byte.
{
    NRF_CSN_lo;
    spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg)); // W_REGISTER — команда записи в регистр.
    spi_fast_shift(value);                      // Write register
    NRF_CSN_hi;
}
/*
void NRF_write_data(char reg, char len)// Writes an array of bytes. 
{
 char i;
    NRF_CSN_lo;
    spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg)); // W_REGISTER — команда записи в регистр.
    for(i=0; i<len; i++) spi_fast_shift(buffer[i]);     // Write data
    NRF_CSN_hi;
}
*/
void NRF_config() 
// Sets the important registers in the MiRF module and powers the module in receiving mode
{
//---- http://narodstream.ru/stm-urok-103-moduli-nrf24l01-chast3/  ---------------------------------------------------------------
    delay_ms(5);                     // Wait for mirf to come up
    NRF_write_byte(CONFIG, 0x0a);     //*(Transmitter) ( (1<<PWR_UP) | (1<<EN_CRC) | (0<<CRCO) | (0<<PRIM_RX) )
    delay_ms(5);                      //*Дадим модулю включиться, по даташиту около 1,5 мсек, а лучше 5
    NRF_write_byte(EN_AA, 0x01);      //*Enable auto acknowledgement ENAA_P0 = 1 
    NRF_write_byte(EN_RXADDR, 0x01);  //*Enable data pipe 0 ERX _P0 = 1
 //   NRF_write_byte(SETUP_AW, 0x01);   //*Setup address width=3 bytes
    NRF_write_byte(SETUP_RETR, 0x2F); //*750us, 15 retrans будем использовать задержку в 750 микросекунд и максимальное количество попыток — 15
    //NRF24_ToggleFeatures();????
    //NRF24_WriteReg(FEATURE, 0);????
    
//---- http://narodstream.ru/stm-urok-103-moduli-nrf24l01-chast4/   --------------------------------------------------------------
    //NRF24_WriteReg(DYNPD, 0);????
    NRF_write_byte(STATUS, 0x70);     // Reset flags for IRQ Во время инициализации следует сбросить флаги всех прерываний 
    NRF_write_byte(RF_CH, 76);        // частота 2476 MHz  (76 ch.)
    NRF_write_byte(RF_SETUP, 0x01);   // TX_PWR: -12dBm,     Datarate: 1Mbps
    //--- если необходимо заменим адрес ---
    //buffer[0] = 0xb3; buffer[1] = 0xb4; buffer[2] = 0x01; // новый адрес передатчика
    //NRF_write_data(TX_ADDR, 3);                           // занесём этот адрес в память рессивера
    //NRF_write_data(RX_ADDR_P0, 3);                        // Добавим адрес в канал pipe0 на случай, если нам придётся использовать модуль в качестве приёмника.
    //--------------------------------------
    // Set length of incoming payload 
    //NRF_write_byte(RX_PW_P0, NRF_PAYLOAD);  // (0x11, 16)
    NRF_write_byte(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data Pipe0
    // Start receiver 
    NRF_write_byte(CONFIG, 0x08);     // режим POWER DOWN
   // NRF_Mode(MODE_PRX);                                    // //пока уходим в режим приёмника
}
/*
void NRF_Mode(char mode)
{
 char regval;
  NRF_CE_lo;                           // Deactivate transreceiver
  regval = NRF_read_register(CONFIG);  //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  if(mode==MODE_PRX)
   {
     ptx = 0;                         // Set to receiving mode
     regval |= (1<<PRIM_RX);
   }
  else
   {
     ptx = 1;                         // Set to transmitter mode
     regval &= (0<<PRIM_RX);
   }
  regval |= (1<<PWR_UP);
  NRF_write_byte(CONFIG, regval);
  NRF_CE_hi;                          // Listening for pakets
  delay_ms(150);                      // Задержка минимум 130 мкс
  NRF_write_cmd(FLUSH_RX, 0);          // FLUSH_RX — команда очистки буфера FIFO, предназначенного для приема.
  NRF_write_cmd(FLUSH_TX, 0);          // FLUSH_TX — команда очистки буфера FIFO, предназначенного для предачи.
}

//------------------------------------------------

void NRF_set_rxADDR(char * adr) 
// Sets the receiving address
{
    NRF_CE_lo;
    NRF_write_register(RX_ADDR_P0,adr,5);
    NRF_CE_hi;
}

void NRF_set_txADDR(char * adr)
// Sets the transmitting address
{
    NRF_write_register(TX_ADDR, adr,5);
}


extern void NRF_get()  // (char * data)
// Reads NRF_PAYLOAD bytes into data array
{
 char i;
    NRF_CSN_lo;                                 // Pull down chip select
    spi_fast_shift(R_RX_PAYLOAD);               // Send cmd to read rx payload
// R_RX_PAYLOAD — команда чтения буфера FIFO. После команды по MISO начинают передаваться данные с младжего байта. 
// После окончания чтения буфер FIFO освобождается.
    //spi_transfer_sync(data,data,NRF_PAYLOAD); // Read payload
    for(i=0; i<NRF_PAYLOAD; i++) buffer[i] = spi_fast_shift(NOP); //read write register
    NRF_CSN_hi;                                 // Pull up chip select
    NRF_write_byte(STATUS,(1<<RX_DR));          // Clar flag "RX_DR"
}



void NRF_read_data(char reg, char len)
// Reads an array of bytes from the given start position in the MiRF registers.
{
 char i;
    NRF_CSN_lo;
    spi_fast_shift(R_REGISTER | (REGISTER_MASK & reg));   // R_REGISTER — команда чтения регистра.
    for(i=0; i<len; i++) buffer[i] = spi_fast_shift(NOP); // read data
    NRF_CSN_hi;
}

void NRF_Transmit() // Sends a data package to the default address. Be sure to send the correct amount of bytes as configured as payload on the receiver.
{
 //   while (ptx) {}                  // Wait until last paket is send
    
    delay_us(10);
    NRF_write_cmd(W_TX_PAYLOAD, TX_PLOAD_WIDTH);
    NRF_CE_hi;                        // Listening for pakets
// W_TX_PAYLOAD — команда записи данных в буфер FIFO. После команды по MOSI мы данные передаются в модуль и записываются в буфер FIFO. 
// Загружать данные также следует с младшего байта.
 //   NRF_Mode(MODE_PTX);             // Power up in transmission mode
}
*/
//------------------------------------------------
void NRF_Sending(void)
{
// char regval;
    NRF_CE_lo;                        // Deactivate transreceiver    
//	regval = NRF_read_register(CONFIG);
//	regval |= (1<<PWR_UP);
//	regval &= (0<<PRIM_RX);
	NRF_write_byte(CONFIG, 0x0a);     // режим передатчика
	delay_us(150);                    // Задержка минимум 130 мкс
	NRF_write_cmd(FLUSH_TX, 0);       // FLUSH_TX — команда очистки буфера FIFO, предназначенного для предачи.
	NRF_write_cmd(W_TX_PAYLOAD, TX_PLOAD_WIDTH);//Отправим данные
    
	NRF_CE_hi;                        // Pulse CE high. 
	delay_us(15);                     //minimum 10us high pulse
	NRF_CE_lo;
//	while((GPIO_PinState)IRQ == GPIO_PIN_SET) {}
	delay_ms(10);
    NRF_CE_hi;
    ptx = 0;
    NRF_write_cmd(STATUS, 0);
	if(status & TX_DS)                // TX_DS -> 0x20
	{
		ptx = 1;
		NRF_write_byte(STATUS, TX_DS);
	}
	else if(status & MAX_RT)          // Maximum number of TX retries interrupt; MAX_RT -> 0x10
	{
		ptx = 2;
        NRF_write_byte(STATUS, MAX_RT);
	}
    else if(status==0xE) ptx = 3;
//	regval = NRF_read_register(OBSERVE_TX);
//    if(regval) ptx = 4;
//--------------------------------------------------------------------------------------------------
// The device goes into Standby-I mode if CE is low. Otherwise next payload in TX_FIFO will be sent.
// If TX_FIFO is empty and CE is still high, the device will enter Standby-II mode.
// If the device is in Standby-II mode, it will go to Standby-I mode immediately if CE is set low.
//---------------------------------------------------------------------------------------------------
//  NRF_write_byte(CONFIG, 0x08);     // режим POWER DOWN
}
