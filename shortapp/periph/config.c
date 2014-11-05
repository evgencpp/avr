#include <avr/io.h>
#include <string.h>
#include "config.h"


drivecfg_t EEMEM EEprmmem[CONFIG_DVG_CNT];

drivecfg_t curConfig[CONFIG_DVG_CNT];


unsigned char loadCfg(void)
{
	for(uint8_t i=0;i<CONFIG_DVG_CNT;i++)
		eeprom_read_block(&curConfig[i], &EEprmmem[i], DRV_CFG_SIZE); 
	return 1;	
}

void SetDefConfig(void)
{
	/* !! make it works)) */
	for(uint8_t i=0;i<CONFIG_DVG_CNT;i++){
		memset(&curConfig[i], 0, DRV_CFG_SIZE);
		memset(&EEprmmem[i], 0, DRV_CFG_SIZE);
	}
}

unsigned char readCfg(const drivecfg_t *cfg)
{
	return 1;	
}

unsigned char writeCfg(const drivecfg_t cfg)
{
	for(uint8_t i=0;i<CONFIG_DVG_CNT;i++)
		eeprom_write_block(&cfg, &EEprmmem[i], DRV_CFG_SIZE);
	
	return 1;
}

void LoadConfig(uint8_t idx)
{
	

}

void SaveConfig(uint8_t idx)
{


}
