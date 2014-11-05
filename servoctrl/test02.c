/*
 * test02.c
 *
 * Created: 17.03.13 13:41:10
 *  Author: ЖЕНЯ
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "periph/proc_events.h"
#include "periph/uart_interface.h"
#include "periph/drive.h"
#include "periph/hctl_2032_encoder.h"
#include "periph/perifclk.h"
#include "periph/adc_router.h"
#include "periph/config.h"
#include "periph/adc_router.h"
#include "periph/memdebug.h"

//static size_t t0, t1, t2;
//static uint16_t* mem_ptr;
int main(void)
{
	/* initialize all of the interface modules */
	InitBaseIO();
	//if(LoadConfig() == 0)
	SetDefConfig();
	
	for(uint8_t i = 0; i < DRV_CNT; i++)
		InitDrive(drv_pins[i]);
		
	UartInt_init();
	InitHCTL();
	InitPerifClk();
	InitADC();
	/*
	t0 = getMemoryUsed();
	t1 = getFreeMemory();
	t2 = getLargestAvailableMemoryBlock();
	mem_ptr = (uint16_t*)malloc(sizeof(uint16_t)*3000);
	
	t0 = getMemoryUsed();
	t1 = getFreeMemory();
	t2 = getLargestAvailableMemoryBlock();
	
	free(mem_ptr); mem_ptr = NULL;
	t0 = getMemoryUsed();
	t1 = getFreeMemory();
	t2 = getLargestAvailableMemoryBlock();
	*/
		
	FlashLeds();
	
	
    
	/* the rest of the application will be under the
	control of the Executive.  */
	sei();
	
	StartConvAdc();
	
	Exec_run();
	
	/* this should never be reached */
	return (0);
}