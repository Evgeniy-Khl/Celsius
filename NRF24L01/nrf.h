
#ifndef _NRF_H_
#define _NRF_H_

//#include <avr/io.h>

// Mirf settings
/*
#define NRF_CH         2
#define NRF_PAYLOAD    16
#define NRF_CONFIG     ( (1<<MASK_RX_DR) | (1<<EN_CRC) | (0<<CRCO) )
*/

// Definitions for selecting and enabling MiRF module
#define NRF_CSN_hi     CSNPORT = OFF;
#define NRF_CSN_lo     CSNPORT = ON;
#define NRF_CE_hi      CE_PORT = OFF;
#define NRF_CE_lo      CE_PORT = ON;

// Public standart functions
char NRF_config();
char NRF_read_reg(char reg);
void NRF_write_cmd(char cmd, char len);
void NRF_write_byte(char reg, char value);

#endif /* _NRF_H_ */
