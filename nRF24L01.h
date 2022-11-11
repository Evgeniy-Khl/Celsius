/* Memory Map */
#define CONFIG      0x00 // конфигурационный регистр
#define EN_AA       0x01 // регистр, который использует указанную технологию и включает автоподтверждение для определённого канала обмена
#define EN_RXADDR   0x02 // который как раз и включает использование каналов. А различаться данные каналы будут по адресам, по которым приёмники будут понимать, что данный пакет адресован именно ему
#define SETUP_AW    0x03 // который является общим для всех каналов обмена и устанавливает величину адресов приёмника и передатчика (можно сказать сетевых адресов) в байтах, которая может регулироваться от 3 до 5 байтов
#define SETUP_RETR  0x04 // это регистр, который устанавливает параметры для повторных передач пакета при их неудачной отправке
#define RF_CH       0x05 // который устанавливает частоту работы передатчика. Мы передаём данные на одном из частотных каналов
#define RF_SETUP    0x06 // который используется для настройки скорости и мощности передачи
#define STATUS      0x07 // который обычно используется для того, чтобы узнать то или иное состояние, но порой нужно будет его использовать и для записи
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A // Это регистры адресов каналов обена информации для приёмных устройств
#define RX_ADDR_P1  0x0B // Это регистры адресов каналов обена информации для приёмных устройств
#define RX_ADDR_P2  0x0C // Это регистры адресов каналов обена информации для приёмных устройств                                                                                                                                                
#define RX_ADDR_P3  0x0D // Это регистры адресов каналов обена информации для приёмных устройств                                                                                                                                                
#define RX_ADDR_P4  0x0E // Это регистры адресов каналов обена информации для приёмных устройств
#define RX_ADDR_P5  0x0F // Это регистры адресов каналов обена информации для приёмных устройств
#define TX_ADDR     0x10 // который несёт в себе информацию об адресе передатчика
#define RX_PW_P0    0x11 // заносится количество байт в пакете для каждого канала обмена информацией
#define RX_PW_P1    0x12 // заносится количество байт в пакете для каждого канала обмена информацией
#define RX_PW_P2    0x13 // заносится количество байт в пакете для каждого канала обмена информацией
#define RX_PW_P3    0x14 // заносится количество байт в пакете для каждого канала обмена информацией
#define RX_PW_P4    0x15 // заносится количество байт в пакете для каждого канала обмена информацией
#define RX_PW_P5    0x16 // заносится количество байт в пакете для каждого канала обмена информацией
#define FIFO_STATUS 0x17
#define DYNPD       0x1C // который управляет использованием режима динамического количества байт в пакете для каждого канала обмена
#define FEATURE     0x1D // который используется в режиме динамического количества байтов в пакете и содержит некоторые настройки данного режима

/* Bit Mnemonics */
//--- CONFIG ---
#define MASK_RX_DR  6  // прерывание, возникающее при получении пакета в приёмнике в тот момент, когда пакет появится в приёмном буфере.
#define MASK_TX_DS  5  // прерывание, возникающего при успешной отправке пакета приёмнику в передающем рессивере. Если включено автоподтвержение, то данное прерывание происходит после получения подтверждения от приёмника.
#define MASK_MAX_RT 4  // прерывание, происходящее при исчерпывании максимального количество повторных отправок пакета передатчиком. Максимальное количество устанваливается также в определённом регистре.
#define EN_CRC      3  // включение использования контрольной суммы. Если хотя бы на один канал включено автоподтверждение, данный бит включится самостоятельно.
#define CRCO        2  // количество байтов контрольной суммы. 0 — 1 байт, 1 — 2 байта.
#define PWR_UP      1  // бит управления включением передатчика. 1 — включено, 0 — выключен (или спящий режим).
#define PRIM_RX     0
//--- EN_AA ---
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
//--- EN_RXADDR ---
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
//--- SETUP_AW ---
//#define AW          0
//--- SETUP_RETR ---
#define ARD 4 // 4 бита. Задаёт значение задержки перед повторной отправкой пакета: 250 x (n + 1) мкс
#define ARC 0 // 4 битай. Количество повторных попыток отправки, 0 - повторная отправка отключена.

#define SETUP_RETR_DELAY_250MKS  (0 << ARD)
#define SETUP_RETR_DELAY_500MKS  (1 << ARD)
#define SETUP_RETR_DELAY_750MKS  (2 << ARD)
#define SETUP_RETR_DELAY_1000MKS (3 << ARD)
#define SETUP_RETR_DELAY_1250MKS (4 << ARD)
#define SETUP_RETR_DELAY_1500MKS (5 << ARD)
#define SETUP_RETR_DELAY_1750MKS (6 << ARD)
#define SETUP_RETR_DELAY_2000MKS (7 << ARD)
#define SETUP_RETR_DELAY_2250MKS (8 << ARD)
#define SETUP_RETR_DELAY_2500MKS (9 << ARD)
#define SETUP_RETR_DELAY_2750MKS (10 << ARD)
#define SETUP_RETR_DELAY_3000MKS (11 << ARD)
#define SETUP_RETR_DELAY_3250MKS (12 << ARD)
#define SETUP_RETR_DELAY_3500MKS (13 << ARD)
#define SETUP_RETR_DELAY_3750MKS (14 << ARD)
#define SETUP_RETR_DELAY_4000MKS (15 << ARD)

#define SETUP_RETR_NO_RETRANSMIT (0 << ARC)
#define SETUP_RETR_UP_TO_1_RETRANSMIT (1 << ARC)
#define SETUP_RETR_UP_TO_2_RETRANSMIT (2 << ARC)
#define SETUP_RETR_UP_TO_3_RETRANSMIT (3 << ARC)
#define SETUP_RETR_UP_TO_4_RETRANSMIT (4 << ARC)
#define SETUP_RETR_UP_TO_5_RETRANSMIT (5 << ARC)
#define SETUP_RETR_UP_TO_6_RETRANSMIT (6 << ARC)
#define SETUP_RETR_UP_TO_7_RETRANSMIT (7 << ARC)
#define SETUP_RETR_UP_TO_8_RETRANSMIT (8 << ARC)
#define SETUP_RETR_UP_TO_9_RETRANSMIT (9 << ARC)
#define SETUP_RETR_UP_TO_10_RETRANSMIT (10 << ARC)
#define SETUP_RETR_UP_TO_11_RETRANSMIT (11 << ARC)
#define SETUP_RETR_UP_TO_12_RETRANSMIT (12 << ARC)
#define SETUP_RETR_UP_TO_13_RETRANSMIT (13 << ARC)
#define SETUP_RETR_UP_TO_14_RETRANSMIT (14 << ARC)
#define SETUP_RETR_UP_TO_15_RETRANSMIT (15 << ARC)
//--- RF_SETUP ---
#define CONT_WAVE   7 // (Только для nRF24L01+) Непрерывная передача несущей (для тестов)
#define RF_DR_LOW   5 // (Только для nRF24L01+) Включает скорость 250кбит/с. RF_DR_HIGH должен быть 0
#define PLL_LOCK    4 // Для тестов
#define RF_DR_HIGH  3 // Выбор скорости обмена (при значении бита RF_DR_LOW = 0): 0 - 1Мбит/с; 1 - 2Мбит/с
#define RF_PWR      1 // 2бита. Выбирает мощность передатчика: 0 - -18dBm; 1 - -16dBm; 2 - -6dBm; 3 - 0dBm

#define RF_SETUP_MINUS18DBM (0 << RF_PWR)
#define RF_SETUP_MINUS12DBM (1 << RF_PWR)
#define RF_SETUP_MINUS6DBM  (2 << RF_PWR)
#define RF_SETUP_0DBM       (3 << RF_PWR)

#define RF_SETUP_1MBPS (0 << RF_DR_HIGH)
#define RF_SETUP_2MBPS (1 << RF_DR_HIGH)
#define RF_SETUP_250KBPS (1 << RF_DR_LOW) // этот режим не должен использоваться с контролем доставки
//--- STATUS ---        
#define RX_DR       0x40
#define TX_DS       0x20
#define MAX_RT      0x10
//#define RX_P_NO     1   // 000-101: Data Pipe Number;    110: Not Used;    111: RX FIFO Empty. #define TX_FULL     0
//--- OBSERVE_TX ---
//#define PLOS_CNT    4
//#define ARC_CNT     0
//--- FIFO_STATUS ---
//#define TX_REUSE    6
//#define FIFO_FULL   5
//#define TX_EMPTY    4
//#define RX_FULL     1
//#define RX_EMPTY    0

/* Instruction Mnemonics */
#define R_REGISTER          0x00 // + n Прочитать регистр n
#define W_REGISTER          0x20 // + n Записать регистр n
#define R_RX_PAYLOAD        0x61 // Принять данные данные из верхнего слота очереди приёмника. 
#define W_TX_PAYLOAD        0xA0 // Записать в очередь передатчика данные для отправки
#define FLUSH_TX            0xE1 // Сбросить очередь передатчика
#define FLUSH_RX            0xE2 // Сбросить очередь приёмника
#define REUSE_TX_PL         0xE3 // Использовать повторно последний переданный пакет
#define R_RX_PL_WID         0x60 // Прочитать размер данных принятого пакета в начале очереди приёмника. 
#define W_ACK_PAYLOAD       0xA8 // + p Записать данные для отправки с пакетом подтверждения по каналу p. 
#define W_TX_PAYLOAD_NOACK  0xB0 // Записать в очередь передатчика данные, для отправки без подтверждения
#define NOP                 0xFF // Нет операции. Может быть использовано для чтения регистра статуса
/*
ACTIVATE — команда активирует команды R_RX_PL_WID, W_ACK_PAYLOAD 
W_TX_PAYLOAD_NOACK. В данном даташите, из которого был сделан скриншот, её нет, есть в другом.
*/
#define REGISTER_MASK  0x1F
#define MODE_PTX        0
#define MODE_PRX        1