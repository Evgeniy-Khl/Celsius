char NRF_read_reg(char reg)// Writes only one byte.
{
 char val;
    NRF_CSN_lo;
    status = spi_fast_shift(reg);
    val = spi_fast_shift(NOP);
    NRF_CSN_hi;
    return val;
}

void NRF_write_cmd(char cmd, char len)// Записываем команду и сразуже получаем STATUS register
{
 char i;
    NRF_CSN_lo;                                // Pull down chip select
    status = spi_fast_shift(cmd);
    if(len)
     {
       for(i=0; i<len; i++) spi_fast_shift(pv.fifo[i]);     // Write data
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

char NRF_config() 
{
  char eeprom *ptr_to_eeprom;
  char errors=0, i, *ptr_to_ram;
    NRF_write_byte(SETUP_RETR, 0x2F); // 750us, 15 retrans будем использовать задержку в 750 микросекунд и максимальное количество попыток — 15
    if(NRF_read_reg(SETUP_RETR) != 0x2F) errors++;
    NRF_write_byte(RF_CH, NRFchannel);         // номер используемого радиочастотного канала
    if(NRF_read_reg(RF_CH) != NRFchannel) errors++;
//    NRF_write_byte(RF_SETUP, RF_SETUP_1MBPS | RF_SETUP_0DBM); // выбор скорости 1 Мбит/с и мощности 0dBm
    //--- если необходимо заменим адрес ---
    ptr_to_eeprom = &addrPipe[NRFblockAdr][0]; ptr_to_ram = pv.fifo;
    for (i=0; i<3; i++) *ptr_to_ram++ = *ptr_to_eeprom++;  // копирование массива
    //NRF_write_data(TX_ADDR, 5);                           // Адрес удалённой стороны
    //NRF_write_data(RX_ADDR_P0, 5);                        // Адрес удалённой стороны
    //buffer[0] = 0xb3; buffer[1] = 0xb4; buffer[2] = 0x01; // Собственный адрес
    //NRF_write_data(RX_ADDR_P1, 5);                        // Собственный адрес
    //--------------------------------------
    NRF_write_byte(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data Pipe0
    if(NRF_read_reg(RX_PW_P0) != TX_PLOAD_WIDTH) errors++;
    NRF_write_byte(STATUS, 0x70);     // Reset flags for IRQ Во время инициализации следует сбросить флаги всех прерываний
    // Start receiver 
//    NRF_write_byte(CONFIG, 0x08);     // режим POWER DOWN
    return errors;
}

//------------------------------------------------
void NRF_Sending(void)
{
    NRF_CE_lo;                        // Deactivate transreceiver
	NRF_write_byte(CONFIG, 0x0a);     // режим передатчика
	delay_ms(5);                      // Задержка минимум 130 мкс
	NRF_write_cmd(FLUSH_TX, 0);       // FLUSH_TX — команда очистки буфера FIFO, предназначенного для предачи.
	NRF_write_cmd(W_TX_PAYLOAD, TX_PLOAD_WIDTH);//Отправим данные
    
	NRF_CE_hi;                        // Pulse CE high. 
	delay_us(20);                     // minimum 10us high pulse
	NRF_CE_lo;
    delay_us(350);                    // Задержка минимум 130 мкс
    NRF_write_byte(CONFIG, 0x00);     // режим Power Down
}
