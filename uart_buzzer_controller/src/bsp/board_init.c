/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	

	
	sysclk_init();
	
	#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	WDT->WDT_MR = WDT_MR_WDDIS;
	#endif

	ioport_init();

	/* Initialize LED0, turned off */
	ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);
	

	//sysclk_enable_peripheral_clock(ID_PIOA);
	
	//pio_set_peripheral(PIOA, PIO_PERIPH_A,  PIO_PA9 | PIO_PA10);
	
	//EDBG Virtual Com
	//pio_set_peripheral(PIOA, PIO_PERIPH_B,  PIO_PA28 | PIO_PA27);
	
	//USART PA28/PA27	
	//sysclk_enable_peripheral_clock(ID_FLEXCOM7);
	//flexcom_set_opmode(FLEXCOM7, FLEXCOM_MR_OPMODE_USART);
			
	//sysclk_enable_peripheral_clock(ID_FLEXCOM0);
	//flexcom_set_opmode(FLEXCOM0, FLEXCOM_MR_OPMODE_USART);
		
	/* Configure UART pins */
	ioport_set_port_peripheral_mode(PINS_USART7_PORT, PINS_USART7,
	PINS_USART7_FLAGS);
	
}
