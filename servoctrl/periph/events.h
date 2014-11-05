/*
 * events.h
 *
 * Created: 04.04.13 0:44:13
 *  Author: ЖЕНЯ
 */ 


#ifndef EVENTS_H_
#define EVENTS_H_



/* Definitions */
/* Originally, all events were passed in a bitmask...however,
an event FIFO was finally used, but the coding of the event
definitions were never translated back....doesn't make a
difference, but looks a little weird */
#define EV_SERIAL_DATA_RECEIVED		0x01
#define EV_DUMP_FRAME				0x02
#define EV_PROCESS_FRAME_COMPLETE	0x04
#define EV_PROCESS_LINE_COMPLETE    0x08
#define EV_ACQUIRE_LINE_COMPLETE	0x10
#define EV_ACQUIRE_FRAME_COMPLETE	0x20
#define EV_CONFIGURE_CAMERA			0x40
#define EV_ENABLE_TRACKING			0x80
#define EV_DISABLE_TRACKING			0x81
#define EV_SERIAL_DATA_PENDING_TX	0x90
#define EV_RED_COLOR_MAP_RECEIVED	0x91
#define EV_BLUE_COLOR_MAP_RECEIVED	0x92
#define EV_GREEN_COLOR_MAP_RECEIVED	0x93

/* This is used to pass fast events through the system
so there is a minimum of processing time needed
between lines of tracking data */
#define FEV_SYS_TIMER_COMPLETE 	0x01
#define FEV_PROCESS_LINE_COMPLETE 	0x02

/* This is needed for the event fifo */
#define EXEC_EVENT_FIFO_SIZE 8
#define EXEC_EVENT_FIFO_MASK EXEC_EVENT_FIFO_SIZE-1 


#endif /* EVENTS_H_ */