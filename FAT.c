#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "FAT.h"
#include "HAL.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API Prototype
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void readBootSector (BootSector*pBootSector)
{

    uint8_t buffer[512];
    uint8_t init;
    
    HAL_ReadSector(0,buffer);
	
    pBootSector->NumberSectorBeforeFAT = CONV_LTE_2B(buffer[0x0E], buffer[0x0F]); 
    pBootSector->NumberFAT = buffer[0x10];
    pBootSector->NumberSectorFAT = CONV_LTE_2B(buffer[0x16], buffer[0x16 + 0x01]);

    pBootSector->NumberEntryRDET = CONV_LTE_2B(buffer[0x11], buffer[0x12]);
    pBootSector->NumberSectorRDET = (pBootSector->NumberEntryRDET * 32) / 512;

    /*size of disk*/
	if (buffer[0x13] != 0)
    {
        pBootSector->NumberSectorDisk = CONV_LTE_2B(buffer[0x13], buffer[0x13 + 0x01]);
    }
    else
    {
        pBootSector->NumberSectorDisk = CONV_LTE_4B(buffer[0x20 ], buffer[0x20 + 0x01],
                                                               buffer[0x20 + 0x02], buffer[0x20 + 0x03]);
    }
    
    pBootSector->NumberSectorCluster = 	buffer[0x0D];
    pBootSector->NumberClusterData   = 	(pBootSector->NumberSectorDisk - pBootSector->NumberSectorBeforeFAT - 
										pBootSector->NumberSectorRDET - pBootSector->NumberSectorFAT*2) / pBootSector->NumberSectorCluster;

    /*address start*/
    pBootSector->startByteFAT = pBootSector->NumberSectorBeforeFAT * BYTE_PER_SECTOR;
    
    pBootSector->startByteRootDirectory = 	pBootSector->startByteFAT + pBootSector->NumberSectorFAT * 
											pBootSector->NumberFAT * BYTE_PER_SECTOR;
											
    pBootSector->startByteData = (pBootSector->NumberFAT * pBootSector->NumberSectorFAT +
                                 pBootSector->NumberSectorBeforeFAT + pBootSector->NumberSectorRDET) * BYTE_PER_SECTOR;
                                 
}

void readRootDirector(BootSector* pBootSector, DataEntryInRDET* pDataEntryInRDET, Address* pAddress,uint8_t *stop)
{

    bool check = true;

    uint8_t  i,CountEntry = 0;
    uint8_t space = 0;
    uint8_t countFile  =0;
    uint8_t entry[BYTE_PER_ENTRY];
    uint8_t choice;
    uint8_t buffType[pBootSector->NumberSectorRDET];
    uint16_t init;
    uint32_t addStart;
    uint16_t buffCluster[pBootSector->NumberSectorRDET];

	
	if(pAddress->currAddress != 0)
	{
		addStart = pAddress->currAddress; 
    }
    else
    {
    	addStart = pBootSector->startByteRootDirectory  ;
	}
    system("cls"); 
    printf("\n%-9s%-21s%-14s%-18s%-10s", "NUMBER", "NAME", "SIZE", "TYPE", "MODIFIED");
    printf("\n_________________________________________________________________________________");
    
    if(addStart == pBootSector->startByteRootDirectory)
	{
		printf("\n%-7dEXIT",0);
	}


    while(check && CountEntry<20)
	{
    	/*init for entry*/
    	HAL_ReadEntry(addStart + BYTE_PER_ENTRY*CountEntry, entry);
    	TakeDataFromEntryInRDET(pDataEntryInRDET,entry );
    	
    	/* check entry is empty ? */
		if(pDataEntryInRDET->statusEntry != 0x00)  
    	{
    		/* short file name ? */
    		if(pDataEntryInRDET->propertyFile != 0x0F)
    		{
    			//readFAT( pBootSector, pDataEntryInRDET);
    			
    			/* display RDET */
    			if(pDataEntryInRDET->subDirectory[0] != 0x2E)
    			{
    				printf("\n%-7d", ++(countFile));
    				display(pDataEntryInRDET, pAddress);
    				buffCluster[countFile] = pDataEntryInRDET->cluster;
    				buffType   [countFile] =  pAddress->type;
				}
				
				/* display subDirectory */
				if(pDataEntryInRDET->subDirectory[0] == 0x2E && pDataEntryInRDET->subDirectory[1] == 0x2E )
    			{
    				printf("\n%-7d is back", (countFile));
    				buffCluster[countFile] = pDataEntryInRDET->cluster;
    				buffType   [countFile] =  pAddress->type;
				}
    			
			}
			CountEntry++;
		}
		else
			check = false;
	}
	
	
  	printf("\nChoice: ");
  	scanf("%d",&choice);
	
	pAddress->type        =  buffType[choice];
  
  	/* read text */
	if(pAddress->type == 1)
	{
		
		pAddress->currAddress = pBootSector->startByteData + (buffCluster[choice] - 2) * pBootSector->NumberSectorCluster * BYTE_PER_SECTOR; 
		DisplayFileText(pBootSector, pAddress);
		/* address back before folder */
		pAddress->currAddress = addStart;
	}
	
	/* read folder */
	else
	{
		if(buffCluster[choice] != 0x00)
		{
			pAddress->currAddress =  pBootSector->startByteData + (buffCluster[choice] - 2) * pBootSector->NumberSectorCluster * BYTE_PER_SECTOR; 
  		}
		else
			pAddress->currAddress = pBootSector->startByteRootDirectory;
	}
	
	if(choice ==0 && addStart == pBootSector->startByteRootDirectory)
	{
		*stop = choice;
	}
}

void display(DataEntryInRDET* pDataEntryInRDET, Address* pAddress)
{
	uint8_t init;
	uint8_t space = 0;
	
	/* Name file */
	for(init=0; init<11; init++)
	{	
		/* type file text/folder */
        if(init==8 && (pDataEntryInRDET->propertyFile & 0x10) != 0x10)  /*file text*/
		{
        	printf(".");
        }
                
        /*  */
        if(pDataEntryInRDET->nameFile[init] != 0x20)
		{
            printf("%c",pDataEntryInRDET->nameFile[init]);
        }
        else space++;
    }
    while(space>0)
	{ 
		printf(" "); 
		space--; 
	}
	
	/* type file */
    if((pDataEntryInRDET->propertyFile & 0x10) == 0x10)
    {
        printf("                        %-18s", "File Folder");
        pAddress->type = 0;
        
    }
    else
    {
        printf("      %6d %-10s", pDataEntryInRDET->sizeFile, "byte");
        printf("%-18s", "Text Document");
        pAddress->type = 1;
    }
    
    /* dd/mm/yy */
    if(pDataEntryInRDET->day < 10)
    {
        printf("0%d/", pDataEntryInRDET->day);
    }
    else
    {
        printf("%d/",  pDataEntryInRDET->day);
    }
    if(pDataEntryInRDET->month < 10)
    {
        printf("0%d/", pDataEntryInRDET->month);
    }
    else
    {
        printf("%d/", pDataEntryInRDET->month);
    }
    printf("%d ", pDataEntryInRDET->year);
    
    /* hh:mm:ss */
    if(pDataEntryInRDET->hour < 10)
    {
        printf("0%d:", pDataEntryInRDET->hour);
    }
    else
    {
        printf("%d:", pDataEntryInRDET->hour);
    }
    
    if(pDataEntryInRDET->minute<10)
    {
        printf("0%d:", pDataEntryInRDET->minute);
    }
    else
    {
        printf("%d:", pDataEntryInRDET->minute);
    }
    if(pDataEntryInRDET->second<10)
    {
        printf("0%d", pDataEntryInRDET->second);
    }
    else
    {
        printf("%d", pDataEntryInRDET->second);
    }
}


void TakeDataFromEntryInRDET(DataEntryInRDET * pDataEntryInRDET,uint8_t entry[BYTE_PER_ENTRY] )
{
	//uint8_t entry[BYTE_PER_ENTRY];
	uint8_t init;

    /*status file & (0x0f) -> LFN or SFN || &(0x10) -> file or folder */
    pDataEntryInRDET->propertyFile = entry[0x0B]; 
    pDataEntryInRDET->statusEntry = entry[0x00];
	
	/*address data file*/
    pDataEntryInRDET->cluster = CONV_LTE_2B(entry[0x1A], entry[0x1B]); 
    
	/*read subfolder/folder*/
    pDataEntryInRDET->subDirectory[0] = entry[0x00];
    pDataEntryInRDET->subDirectory[1] = entry[0x01];

    /*name and tail*/
    for(init=0; init<11; init++){
        pDataEntryInRDET->nameFile[init] = entry[init];
    }

    /*size file/folder*/
    pDataEntryInRDET->sizeFile = CONV_LTE_4B(entry[0x1C], entry[0x1D], entry[0x1E], entry[0x1F]);

    /*day - month - year*/
    pDataEntryInRDET->day   =  CONV_LTE_2B(entry[0x18], entry[0x19]) & 0x1F;
    pDataEntryInRDET->month = (CONV_LTE_2B(entry[0x18], entry[0x19]) & 0x1E0) >> 5;
    pDataEntryInRDET->year  = (CONV_LTE_2B(entry[0x18], entry[0x19]) >> 9) + 1980;

    /*hour - minute - ss*/
    pDataEntryInRDET->hour   = (entry[0x17]>>3);
    pDataEntryInRDET->minute = ((entry[0x17]&0x07)<<3)|(entry[0x16]>>5);
    pDataEntryInRDET->second = (entry[0x16]&0x1F)*2;
    
}

void DisplayFileText(BootSector* pBootSector, Address * pAddress)
{
	system("cls");
	uint8_t check = 0;
	uint8_t choice;
	uint16_t init=0;
	uint32_t addStart;
	uint8_t buffer[BYTE_PER_SECTOR * pBootSector->NumberSectorCluster]; 
	
	addStart = pAddress->currAddress;
	
	HAL_ReadCluster(addStart, buffer, BYTE_PER_SECTOR * pBootSector->NumberSectorCluster);
	
	do
	{ 
		for(init = 0; init < BYTE_PER_SECTOR * pBootSector->NumberSectorCluster; init++)
		{
			if(buffer[init] != 0x00)
			{
				printf("%c",buffer[init]);
			}
		}
		printf("\nChoice 0 is back: ");
		scanf("%d",&choice);
	}while( choice != 0 );
}

void readFAT(BootSector* pBootSector, DataEntryInRDET* pDataEntryInRDET)
{
	uint8_t bufferFAT [2];
	uint8_t init;
	uint16_t nextCluster;
	uint16_t addStart;
	
	addStart = pDataEntryInRDET->cluster * 3 / 2 + pBootSector->startByteFAT;
	printf("\npBootSector->startByteFAT %x",pBootSector->startByteFAT);
	printf("\npBootSector->startByteData %x",pBootSector->startByteData);
	/* FAT12 max number cluster of DATA <= 0xFEF or 4079 cluster*/
	if(pBootSector->NumberClusterData <= 0xFEF)
	{
		if(pDataEntryInRDET->cluster % 2 == 0 )
		{
			printf("\npDataEntryInRDET->cluster %x",pDataEntryInRDET->cluster);
			printf("\naddStart %x",addStart);
			HAL_ReadFAT12(addStart,bufferFAT);
			nextCluster = CONV_FAT12_EVEN(bufferFAT[0],bufferFAT[1]);
			
		}
		else
		{
			printf("\npDataEntryInRDET->cluster %x",pDataEntryInRDET->cluster);
			
			HAL_ReadFAT12(addStart,bufferFAT);
			nextCluster = CONV_FAT12_ODD(bufferFAT[0],bufferFAT[1]);
		}
		printf("\nnextCluster %x",nextCluster );
		printf("\naddnnextCluster %x\n",pBootSector->startByteData + (nextCluster - 2) * pBootSector->NumberSectorCluster * BYTE_PER_SECTOR);
	}
}



