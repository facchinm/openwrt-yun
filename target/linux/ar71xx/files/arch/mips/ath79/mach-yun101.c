/*
 *  DRAGINO V2 board support, based on Atheros AP121 board support
 *  
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2012 Elektra Wagenrad <elektra@villagetelco.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define YUN101_GPIO_LED_WLAN		0
#define YUN101_GPIO_LED_LAN		13
#define YUN101_GPIO_LED_WAN		17

/* The following GPIO is actually named "Router" on the board. 
 * However, since the "Router" feature is not supported as of yet
 * we use it to display USB activity. 
 */

#define YUN101_GPIO_LED_SYS		28
#define YUN101_GPIO_BTN_JUMPSTART	11
#define YUN101_GPIO_BTN_RESET		12

#define YUN101_KEYS_POLL_INTERVAL	20	/* msecs */
#define YUN101_KEYS_DEBOUNCE_INTERVAL	(3 * YUN101_KEYS_POLL_INTERVAL)

#define YUN101_MAC0_OFFSET		0x0000
#define YUN101_MAC1_OFFSET		0x0006
#define YUN101_CALDATA_OFFSET		0x1000
#define YUN101_WMAC_MAC_OFFSET		0x1002

static struct gpio_led yun101_leds_gpio[] __initdata = {
	{
		.name		= "yun101:red:lan",
		.gpio		= YUN101_GPIO_LED_LAN,
		.active_low	= 1,
	},
	{
		.name		= "yun101:red:wlan",
		.gpio		= YUN101_GPIO_LED_WLAN,
		.active_low	= 0,
	},
		{
		.name		= "yun101:red:wan",
		.gpio		= YUN101_GPIO_LED_WAN,
		.active_low	= 0,
	},
	{
		.name		= "yun101:red:system",
		.gpio		= YUN101_GPIO_LED_SYS,
		.active_low	= 0,
	},
	
	
};

static struct gpio_keys_button yun101_gpio_keys[] __initdata = {
	{
		.desc		= "jumpstart button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = YUN101_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= YUN101_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	},
	{
		.desc		= "reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = YUN101_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= YUN101_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};



static void __init yun101_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);
	ath79_register_wmac(art + YUN101_CALDATA_OFFSET,
			    art + YUN101_WMAC_MAC_OFFSET);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + YUN101_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + YUN101_MAC1_OFFSET, 0);
	
	ath79_register_mdio(0, 0x0);
	
	/* Enable GPIO13, GPIO14, GPIO15, GPIO16 and GPIO17 */
	ath79_gpio_function_disable(AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				    AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				    AR933X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				    AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				    AR933X_GPIO_FUNC_ETH_SWITCH_LED4_EN);
 
	/* LAN ports */
	ath79_register_eth(1);

	/* WAN port */
	ath79_register_eth(0);

	/* Enable GPIO26 and GPIO27 */
	ath79_reset_wr(AR933X_RESET_REG_BOOTSTRAP,
		       ath79_reset_rr(AR933X_RESET_REG_BOOTSTRAP) |
		       AR933X_BOOTSTRAP_MDIO_GPIO_EN);

}

static void __init yun101_setup(void)
{
	yun101_common_setup();
		
	ath79_register_leds_gpio(-1, ARRAY_SIZE(yun101_leds_gpio),
				 yun101_leds_gpio);
	ath79_register_gpio_keys_polled(-1, YUN101_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(yun101_gpio_keys),
					yun101_gpio_keys);
	ath79_register_usb();

}


MIPS_MACHINE(ATH79_MACH_YUN101, "YUN101", "Arduino Yun 101",
	     yun101_setup);

