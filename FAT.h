#ifndef _FAT_H_
#define _FAT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FAT.h"
#include "HAL.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CONV_LTE_2B(add1,add2) 					((add1)    | ((add2)<<8))
#define CONV_LTE_4B(add1,add2,add3,add4) 		((add1)    | ((add2)<<8) | ((add3)<<16)| ((add4)<<32))
#define CONV_FAT12_EVEN(add1,add2) 				((add1)    | (add2 & 0x0F) << 8)
#define CONV_FAT12_ODD(add1,add2) 				((add1>>4) | (add2)<<4)

#define BYTE_PER_SECTOR 512
#define BYTE_PER_ENTRY  32
typedef struct BootSector{

    /*initialization BIOS*/
    uint16_t BytePerSector;                 /* number byte per sector (=512 byte/sector)*/
    uint8_t  NumberSectorCluster;           /* number sectors per Cluster */
    uint8_t  NumberSectorBeforeFAT;         /* number sectors before FAT (sector of bootsector) */
    uint8_t  NumberFAT;                     /* number FAT table */
    uint8_t  NumberSectorFAT;               /* number sectors of FAT */
    uint16_t NumberEntryRDET;               /* number entries cua Root Directory Region */
    uint16_t NumberSectorDisk;              /* number sectors of disk */
	uint16_t NumberClusterData;             /* number cluster of data */
   
    /* Number sectors of Root Directory*/
    uint16_t NumberSectorRDET;

    /*address start*/
    uint16_t startByteFAT;
    uint16_t startByteRootDirectory;
    uint16_t startByteData;
    
}BootSector;

typedef struct DataEntryInRDET /*in RDET*/ 
{
	uint16_t cluster ;
    uint8_t propertyFile;           	/* entry[B] == 0x0F (Long file name) else short file name*/
    uint8_t statusEntry;              	/* entry[0] == 0x00 => entry NULL => END */
    uint8_t subDirectory[2];          	/* = entry[0x00] and entry[0x01], if == ". " thu muc con, if == ".." thu muc me */
    uint8_t nameFile[11];             	/* = entry[0x00](11 byte), 8 byte name + 3 byte file tail */
    uint8_t day;						/* = entry[0x18](2byte) bit 0~4 */
    uint8_t month;						/* = entry[0x18](2byte) bit 5~8 */
    uint16_t year;						/* = entry[0x18](2byte) bit 9~15 (year start 1980) */
    uint8_t hour;						/* = entry[0x16](2byte) bit 10~15 */
    uint8_t minute;						/* = entry[0x16](2byte) bit 5~10 */
    uint8_t second;						/* = entry[0x16](2byte) bit 0~4 (second *= 4) */
    uint32_t sizeFile;                /* = entry[0x1C] offset 4 byte */
}DataEntryInRDET;

typedef struct address 
{
	uint32_t currAddress;
	
	/* 0 - folder || 1 - text */
	uint8_t type;	
	uint8_t FATType;
	
}Address;
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API Prototype
 ******************************************************************************/
void readBootSector (BootSector*pBootSector);

void readRootDirector(BootSector* pBootSector, DataEntryInRDET* pDataEntryInRDET,Address* pAddress,uint8_t *stop);

void readFAT(BootSector* pBootSector, DataEntryInRDET* pDataEntryInRDET);

void DisplayFileText(BootSector* pBootSector, Address * pAddress);

void display(DataEntryInRDET* pDataEntryInRDET, Address* pAddress);

void TakeDataFromEntryInRDET(DataEntryInRDET * pDataEntryInRDET,uint8_t entry[32]);
/*******************************************************************************
 * Code
 ******************************************************************************/
 #endif /* _FAT_H_ */
