/*
 * perifclk.h
 *
 * Created: 17.03.13 14:19:10
 *  Author: ЖЕНЯ
 */ 


#ifndef PERIFCLK_H_
#define PERIFCLK_H_

void InitPerifClk(void);
void SetPerifClk(uint8_t div, uint8_t OCR);
void StartPerifClk(void);
void StopPerifClk(void);

#endif /* UART_H_ */