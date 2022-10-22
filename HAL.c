#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "HAL.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
FILE *   g_pDisk = NULL;
uint16_t g_BytePerSector;
/*******************************************************************************
 * API Prototype
 ******************************************************************************/
 void HAL_Init();

/*******************************************************************************
 * Code
 ******************************************************************************/
void HAL_Init()
{
    uint8_t buffer[2];
    g_pDisk = fopen("demoWinImage.IMA","rb");
    
    /*Disk is empty => exit program*/
    if(g_pDisk==NULL)	
    {
        printf("Disk is empty");
        exit(1);
    }
    //pDataEntryInRDET->cluster = 0;
}

void HAL_ReadSector(uint32_t start,uint8_t *buffer)
{
    fseek(g_pDisk, start * BYTE_PER_SECTOR, SEEK_SET);
    fread(buffer, BYTE_PER_SECTOR, 1, g_pDisk);
}

void HAL_ReadMultiSector(uint16_t numberSector, uint32_t Start, uint8_t *buffer)
{
	uint8_t i;
    fseek(g_pDisk, Start, SEEK_SET);
    fread(buffer, BYTE_PER_SECTOR , 1, g_pDisk);
    for(i=0; i<32; i++)
    {
    	printf("%x ",*(buffer + i));
	}
}

void HAL_ReadEntry(uint32_t start,uint8_t *buffer)
{
    fseek(g_pDisk, start, SEEK_SET);
    fread(buffer, BYTE_PER_ENTRY, 1, g_pDisk);
}

void HAL_ReadCluster(uint32_t start,uint8_t *buffer, uint16_t size)
{
	uint8_t init;
    fseek(g_pDisk, start, SEEK_SET);
    fread(buffer, size, 1, g_pDisk);
    
}


void HAL_ReadFAT12(uint16_t start,uint8_t *buffer)
{
	uint8_t init;
    fseek(g_pDisk, start, SEEK_SET);
    fread(buffer, 2, 1, g_pDisk);
    for(init = 0; init <2; init++)
			{
				printf("\nbuffer[%d] %x",init, buffer[init]);
			}
}
