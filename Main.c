#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FAT.h"
#include "HAL.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/
uint16_t g_BytePerSector;
/*******************************************************************************
 * API Prototype
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
int main(){
	BootSector     * pBootSector      = malloc(sizeof(BootSector));
	DataEntryInRDET* pDataEntryInRDET = malloc(sizeof(DataEntryInRDET));
	Address        * pAddress         = malloc(sizeof(Address));
	
	uint8_t stop = 1;
	
	HAL_Init();
	pAddress->currAddress = 0;
	readBootSector(pBootSector);
	while(stop != 0)
	{
		readRootDirector(pBootSector, pDataEntryInRDET, pAddress, &stop);
	}
	
	free(pBootSector);
	free(pDataEntryInRDET);
	free(pAddress);
	return 0;
}
