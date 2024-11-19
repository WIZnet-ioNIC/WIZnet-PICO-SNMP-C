#ifndef _SNMP_CUSTOM_H_
#define _SNMP_CUSTOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "snmp.h"

extern dataEntryType snmpData[];
extern const int32_t maxData;

// Define for using W5500-EVB: H/W Dependency (e.g., LEDs...)
//#define _USE_WIZNET_W5500_EVB_

#define COMMUNITY					"public\0"
#define COMMUNITY_SIZE				(strlen(COMMUNITY))

/* Predefined function: Response value control */
void initTable();

/* W5100S-EVB-Pico/W5500-EVB-Pico: LED(green) Control examples */
typedef void(*getUserLED_handle)(void*, uint8_t*);	
typedef void(*setUserLED_handle)(int32_t);
// eg: void(*getUserLED_handle)(void* ptr, uint8_t* len);
// ->  *len = sprintf((char *)ptr, "LED ", led_status?"On":"Off");

void UserLED_Control_init(getUserLED_handle get_fun, setUserLED_handle set_fun); /* Registration function */
static void get_LEDStatus_UserLED(void *ptr, uint8_t *len);
static void set_LEDStatus_UserLED(int32_t val);

/* User defined functions: LED control examples */
#ifdef _USE_WIZNET_W5500_EVB_
	void get_LEDStatus_all(void *ptr, uint8_t *len);
	void set_LEDStatus_R(int32_t val);
	void set_LEDStatus_G(int32_t val);
	void set_LEDStatus_B(int32_t val);
#endif
/* SNMP Trap: warmStart(1) */
void initial_Trap(uint8_t * managerIP, uint8_t * agentIP);

#ifdef __cplusplus
}
#endif

#endif
