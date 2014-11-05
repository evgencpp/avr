#include <avr/io.h>
#include <string.h>


#include "proc_events.h"
#include "config.h"


drivecfg_t EEMEM EEprmmem[DRV_CNT];

/*
unsigned char loadCfg(void)
{
	for(uint8_t i=0; i<DRV_CNT; i++)
		eeprom_read_block(&drvCfg[i], &EEprmmem[i], DRV_CFG_SIZE); 
	return 1;	
}
*/

void SetDefConfig(void)
{
	/* TODO: debug SetDefConfig function */
	for(uint8_t i=0;i<DRV_CNT;i++){
		memset(&drvCfg[i], 0, DRV_CFG_SIZE);
		//memset(&EEprmmem[i], 0, DRV_CFG_SIZE);
		
		drvCfg[i].maxPWM_CCW	= MAX_SPEED;
		drvCfg[i].maxPWM_CW		= MAX_SPEED;
		drvCfg[i].minPWM_CCW	= MAX_SPEED;
		drvCfg[i].minPWM_CW		= MAX_SPEED;
	}
}

uint8_t readCfg(const drivecfg_t *cfg)
{
	return 1;	
}

uint8_t writeCfg(const drivecfg_t cfg, uint8_t idx)
{
	eeprom_write_block(&cfg, &EEprmmem[idx], DRV_CFG_SIZE);
	return 1;
}

uint8_t LoadConfig(uint8_t idx)
{
	return 0;
}

void SaveConfig(uint8_t idx)
{

}
