/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"

#include "snmp_custom.h" // Use snmp

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define SOCKET_SNMP 0

#if (DEVICE_BOARD_NAME == W55RP20_EVB_PICO)
#define USER_LED_PIN 19
#else
#define USER_LED_PIN 25
#endif

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 11, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 11, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                        // DHCP enable/disable
};

static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
}; // common buffer

/* SNMP */
uint8_t manager[4] = {192, 168, 11, 162}; // manager ip, (is your pc ip or others managers)

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

void UserLED_Init(void);
void getUserLEDStatus(void *ptr, uint8_t *len);
void setUserLEDStatus(int32_t val);
bool precess_snmp_time_handle(struct repeating_timer *t);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{

    struct repeating_timer timer;

    /* Initialize */
    int retval = 0;
    uint8_t dhcp_retry = 0;
    uint8_t dns_retry = 0;

    set_clock_khz();

    stdio_init_all();
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    add_repeating_timer_ms(10, precess_snmp_time_handle, NULL, &timer);  // Add SNMP 10ms Tick Timer handler

    network_initialize(g_net_info);

    /* Get network information */
    print_network_information(g_net_info);
    
    UserLED_Init();
    setUserLEDStatus(0);

    snmpd_init(manager, g_net_info.ip, SOCKET_SNMP, SOCKET_SNMP + 1); // Initialize snmp

    /* Infinite loop */
    while (1)
    {
        snmpd_run();
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

bool precess_snmp_time_handle(struct repeating_timer *t)
{
    SNMP_time_handler(); // SNMP 10ms Tick Timer handler
}

void UserLED_Init()
{
    gpio_init(USER_LED_PIN);                                  // Initialize LED
    gpio_set_dir(USER_LED_PIN, GPIO_OUT);                     // Output mode
    UserLED_Control_init(getUserLEDStatus, setUserLEDStatus); // Pass control control function
}

void getUserLEDStatus(void *ptr, uint8_t *len)
{
    uint8_t led_status = 0;

    led_status = gpio_get(USER_LED_PIN);

    *len = sprintf((char *)ptr, "USER LED(green) [%s]. ", led_status ? "On" : "Off");
}

void setUserLEDStatus(int32_t val)
{
    if (val == 0)
        gpio_put(USER_LED_PIN, 0);
    else
        gpio_put(USER_LED_PIN, 1);
}