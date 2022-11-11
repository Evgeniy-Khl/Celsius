char NRF_read_reg(char reg)// Writes only one byte.
{
 char val;
    NRF_CSN_lo;
    status = spi_fast_shift(reg);
    val = spi_fast_shift(NOP);
    NRF_CSN_hi;
    return val;
}

void NRF_write_cmd(char cmd, char len)// ���������� ������� � ������� �������� STATUS register
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
    spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg)); // W_REGISTER � ������� ������ � �������.
    spi_fast_shift(value);                      // Write register
    NRF_CSN_hi;
}

char NRF_config() 
{
  char eeprom *ptr_to_eeprom;
  char errors=0, i, *ptr_to_ram;
    NRF_write_byte(SETUP_RETR, 0x2F); // 750us, 15 retrans ����� ������������ �������� � 750 ����������� � ������������ ���������� ������� � 15
    if(NRF_read_reg(SETUP_RETR) != 0x2F) errors++;
    NRF_write_byte(RF_CH, NRFchannel);         // ����� ������������� ��������������� ������
    if(NRF_read_reg(RF_CH) != NRFchannel) errors++;
//    NRF_write_byte(RF_SETUP, RF_SETUP_1MBPS | RF_SETUP_0DBM); // ����� �������� 1 ����/� � �������� 0dBm
    //--- ���� ���������� ������� ����� ---
    ptr_to_eeprom = &addrPipe[NRFblockAdr][0]; ptr_to_ram = pv.fifo;
    for (i=0; i<3; i++) *ptr_to_ram++ = *ptr_to_eeprom++;  // ����������� �������
    //NRF_write_data(TX_ADDR, 5);                           // ����� �������� �������
    //NRF_write_data(RX_ADDR_P0, 5);                        // ����� �������� �������
    //buffer[0] = 0xb3; buffer[1] = 0xb4; buffer[2] = 0x01; // ����������� �����
    //NRF_write_data(RX_ADDR_P1, 5);                        // ����������� �����
    //--------------------------------------
    NRF_write_byte(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data Pipe0
    if(NRF_read_reg(RX_PW_P0) != TX_PLOAD_WIDTH) errors++;
    NRF_write_byte(STATUS, 0x70);     // Reset flags for IRQ �� ����� ������������� ������� �������� ����� ���� ����������
    // Start receiver 
//    NRF_write_byte(CONFIG, 0x08);     // ����� POWER DOWN
    return errors;
}

//------------------------------------------------
void NRF_Sending(void)
{
    NRF_CE_lo;                        // Deactivate transreceiver
	NRF_write_byte(CONFIG, 0x0a);     // ����� �����������
	delay_ms(5);                      // �������� ������� 130 ���
	NRF_write_cmd(FLUSH_TX, 0);       // FLUSH_TX � ������� ������� ������ FIFO, ���������������� ��� �������.
	NRF_write_cmd(W_TX_PAYLOAD, TX_PLOAD_WIDTH);//�������� ������
    
	NRF_CE_hi;                        // Pulse CE high. 
	delay_us(20);                     // minimum 10us high pulse
	NRF_CE_lo;
    delay_us(350);                    // �������� ������� 130 ���
    NRF_write_byte(CONFIG, 0x00);     // ����� Power Down
}
