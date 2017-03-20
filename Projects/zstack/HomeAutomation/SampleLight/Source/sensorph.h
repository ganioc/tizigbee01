#pragma once

#include "hal_types.h"

#define SLAVE_ADDR_PH           0x02
#define ERR_CODE                0xFA
#define READ_CMD_LEN            4
#define WRITE_CMD_LEN           5
#define READ_DATA_LEN           8

uint8 read_dev_addr(void);

bool write_dev_addr(void);

uint16 Read_Soil_Ph(void);