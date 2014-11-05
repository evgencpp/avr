/*
 * pid.h
 *
 * Created: 04.04.13 0:17:12
 *  Author: ЖЕНЯ
 */ 


#ifndef PID0_H_
#define PID0_H_

#include <avr/io.h>

#define SCALING_FACTOR  128
/*! \brief PID Status 
 * 
 * Setpoints and data used by the PID control algorithm 
 */ 
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

typedef struct PID_DATA{
	//! Last process value, used to find derivative of process value.
	int16_t lastProcessValue;
	//! Summation of errors, used for integrate calculations
	int32_t sumError;
	//! The Proportional tuning constant, multiplied with SCALING_FACTOR
	int16_t P_Factor;
	//! The Integral tuning constant, multiplied with SCALING_FACTOR
	int16_t I_Factor;
	//! The Derivative tuning constant, multiplied with SCALING_FACTOR
	int16_t D_Factor;
	//! Maximum allowed error, avoid overflow
	int16_t maxError;
	//! Maximum allowed sumerror, avoid overflow
	int32_t maxSumError;
} pidData_t;


void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid);
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);
void pid_Reset_Integrator(pidData_t *pid_st);

#endif /* PID_H_ */