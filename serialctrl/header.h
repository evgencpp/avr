#ifndef HEADER_H
#define HEADER_H
//main in device

// HDLC headers
#define HDLC_FLAG_SEQUENCE  0x7e   // Flag Sequence
#define HDLC_CONTROL_ESCAPE 0x7d   // Asynchronous Control Escape
#define HDLC_ESCAPE_BIT     0x20   // Asynchronous transparency modifier

#define HDLC_INITFCS    0xffff      /* Initial FCS value */
#define HDLC_GOODFCS    0xf0b8      /* Good final FCS value */


// Protocol header
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// input system commands
#define PING_CMD				1		// [head, len, cnt]
#define STAT_CMD				2		// [head, len, cnt]
#define VERSION_CMD				3		// [head, len, cnt]

// dvg params
#define WRITE_DVGPRM_EEPROM_CMD	4
#define LOAD_DVGPRM_EEPROM_CMD	5
#define SET_DVGPRM_EEPROM_CMD	6
#define GET_DVGPRM_EEPROM_CMD	7

#define SETDVG_CMD              8		// [head, len, cnt, idx, VALUE]	set drive value
#define RSTDVG_CMD				9		// [head, len, cnt, idx]
#define WINDDVG_CMD				10		// [head, len, cnt, idx, value]
#define CALIBDVG_CMD			11		// [head, len, cnt, idx]

// encoder params
#define GET_ENC_CMD              12		// [head, len, cnt, idx]
#define RSTENC_CMD              13		// [head, len, cnt, idx]
#define SETENCMODE_CMD          14		// [head, len, cnt, mode]
#define GETENCMODE_CMD          15		// [head, len, cnt]
#define GET_SPEED_CMD              16		// [head, len, cnt, idx]

//current sensor
#define GET_CURR_SNS_CMD		17		// [head, len, cnt, idx]
#define GET_ALL_CURR_SNS_CMD	18		// [head, len, cnt, idx]

// system CTC
#define GET_SYS_CTC             19      // [head, len, cnt]
#define SET_SYS_CTC             20      // [head, len, cnt, div, ocr]
#define START_SYS_CTC           21      // [head, len, cnt]
#define STOP_SYS_CTC            22      // [head, len, cnt]

#define ST_ENCEV_CMD            23      // [head, len, cnt, idx, value, hi_cnt, low_cnt]


/////////////////////////////////////////////
// packet lengths
#define PING_CMD_LEN				3		// [head, len, cnt]
#define STAT_CMD_LEN				3		// [head, len, cnt]
#define VERSION_CMD_LEN				3		// [head, len, cnt]
#define FLASH_PARAMS_CMD_LEN		4		// [head, len, cnt, idx]

// dvg params
#define SETDVG_CMD_LEN              5		// [head, len, cnt, idx, VALUE]	set drive value
#define RSTDVG_CMD_LEN				4		// [head, len, cnt, idx]
#define WINDDVG_CMD_LEN				5		// [head, len, cnt, idx, state]
#define CALIBDVG_CMD_LEN			4		// [head, len, cnt, idx]

// encoder params
#define GET_ENC_CMD_LEN              4		// [head, len, cnt, idx]
#define RSTENC_CMD_LEN              4		// [head, len, cnt, idx]
#define SETENCMODE_CMD_LEN          4		// [head, len, cnt, mode]
#define GETENCMODE_CMD_LEN          3		// [head, len, cnt]

//current sensor
#define GET_CURR_SNS_CMD_LEN		4		// [head, len, cnt, idx]
#define GET_ALL_CURR_SNS_CMD_LEN        3		// [head, len, cnt]

// system CTC
#define GET_SYS_CTC_LEN             3      // [head, len, cnt]
#define SET_SYS_CTC_LEN             5      // [head, len, cnt, div, ocr]
#define START_SYS_CTC_LEN           3      // [head, len, cnt]
#define STOP_SYS_CTC_LEN            3      // [head, len, cnt]

#define ST_ENCEV_CMD_LEN        7      // [head, len, cnt, idx, value, hi_cnt, low_cnt]

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// output system commands
// system commands
#define PING_ACK_CMD				1		// [head, len, cnt]
#define STAT_ACK_CMD				2
#define VERSION_ACK_CMD				3

#define GET_ENC_ACK_CMD              4		// [head, len, cnt, idx, enc0, enc1, enc2, enc3]
// [head, len, cnt, enc00, enc01, enc02, enc03, enc10, enc11, enc12, enc13]

// drive commands
#define GET_DVGPRM_ACK_CMD		5
// encoder commands
#define GET_SPEED_ACK_CMD		6
//current sensor
#define GET_CURR_SNS_ACK_CMD		7		// [head, len, cnt, idx, enc00]
#define GET_ALL_CURR_SNS_ACK_CMD	8		// [head, len, cnt, enc00, enc10]
#define	GET_SYS_CTC_ACK_CMD			9		//  [head, len, cnt, div, ocr]
#define	FAIL_ALLOC_MEM_CMD			10		//  [head, len, cnt]
#define	EVAL_ENC_DATA_CMD			11		//  [head, len, cnt, point_idx, data]



#define	GET_CURR_SNS_ACK_CMD_LEN	5

#define PING_ACK_CMD_LEN		0x3
#define STAT_ACK_CMD_LEN		0x7
#define VERSION_ACK_CMD_LEN		0x3
#define GET_SYS_CTC_ACK_CMD_LEN 0x5 //  [head, len, cnt, div, ocr]
#define	FAIL_ALLOC_MEM_CMD_LEN	0x3		//  [head, len, cnt]
#define	EVAL_ENC_DATA_CMD_LEN	0x6		//  [head, len, cnt, point_idx, point_idx, data]


#endif
 