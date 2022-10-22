#ifndef _HAL_H_
#define _HAL_H_
#include <stdint.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define disk0 "demoWinImage.IMA"
#define CONV_LTE_2B(add1,add2) ((add1) | ((add2)<<8))
#define CONV_LTE_4B(add1,add2,add3,add4) ((add1) | ((add2)<<8) | ((add2)<<16)| ((add2)<<32))
#define BYTE_PER_SECTOR 512
#define BYTE_PER_ENTRY  32

/*******************************************************************************
 * API Prototype
 ******************************************************************************/
void HAL_Init();

void HAL_ReadEntry(uint32_t start,uint8_t *buffer);

void HAL_ReadSector(uint32_t start,uint8_t *buffer);

void HAL_ReadMultiSector(uint16_t numberSector, uint32_t Start, uint8_t *buffer);

void HAL_ReadCluster(uint32_t start,uint8_t *buffer, uint16_t size);

void test(uint32_t start,uint8_t *buffer);

#endif /* _HAL_H_ */



