/*
 * CFile1.c
 *
 * Created: 20/12/2024 13:04:39
 *  Author: Toor
 */
#include <app.h>
#include <ioport.h>
#include "asf.h"

/** The conversion data is done flag */
volatile bool is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

/** The maximal digital value */
//static uint32_t g_max_digital;

volatile uint32_t ul_sysclk;


void app_init(){
	
	init_uart_console();
	/* Configure TC */
	//init_tc_adc();
	init_tc_pwm();
		
	//configure_adc();
	
	app_run();
}


void app_run(){
	
	int cycle = 0;
	
	/* Insert application code here, after the board has been initialized. */
	
	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		/* Is button pressed? */
		if (TC0->TC_CHANNEL[0].TC_CV == TC0->TC_CHANNEL[0].TC_RC){
			cycle++;
			printf("SYSCLK : %d    RC : %d    CV : %d    CYCLE : %d\n", (int)ul_sysclk, (int)TC0->TC_CHANNEL[0].TC_RC, (int)TC0->TC_CHANNEL[0].TC_CV, cycle);
		}
		
		if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			
			/* Yes, so turn LED on. */
			ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
			} else {
			/* No, so turn LED off. */
			ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}

void init_uart_console(){
		
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
		#endif
		.paritytype = CONF_UART_PARITY,
		#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
		#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
	
}

void init_tc_pwm(void) {
	//uint32_t ul_div = 0;
	//uint32_t ul_tc_clks = 0;
	printf("---------------	TC init ---------------\n");

	ul_sysclk = sysclk_get_cpu_hz();
	printf("Set SYSCLK to %d\n", (int)ul_sysclk);


	/* Activer l'horloge pour TC0 */
	pmc_enable_periph_clk(ID_TC0);
	printf("TC0 CLK enabled by PMC\n");

	/* Enable PCK output */
	//pmc_disable_pck(PMC_PCK_3);
	//pmc_switch_pck_to_sclk(PMC_PCK_3, PMC_PCK_PRES(0));
	//pmc_enable_pck(PMC_PCK_3);
	
	/* Configurer la broche TIOA1 pour la sortie PWM */
	ioport_set_pin_mode(PIN_TC0_TIOA0, PIN_TC0_TIOA0_MUX);
	ioport_disable_pin(PIN_TC0_TIOA0);	
		
	/* Trouver un diviseur pour une fréquence PWM cible (par ex., 1 kHz) */
	//tc_find_mck_divisor(1, ul_sysclk, &ul_div, &ul_tc_clks, ul_sysclk);

	/* Configurer TC0 en mode PWM */
	tc_init(TC0, 0, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_SET | TC_CMR_ACPC_CLEAR);
	printf("TC0 init on channel 0\n");

	/* Configurer RA et RC pour définir la fréquence et le rapport cyclique */
	TC0->TC_CHANNEL[0].TC_RC = (ul_sysclk / 128) / 16;			// RC : période complète (1 Hz)
	printf("TC0 CH0 RC set to %d\n", (int)TC0->TC_CHANNEL[0].TC_RC);

	TC0->TC_CHANNEL[0].TC_RA = ((ul_sysclk / 128) / 16) / 2;
	printf("TC0 CH0 RA set to %d\n", (int)TC0->TC_CHANNEL[0].TC_RA);

	printf("---------------------------------------\n");

	/* Démarrer TC0 */
	tc_start(TC0, 0);
	printf("TC START............\n");

}


/**
 * \brief Configure to trigger ADC by TIOA output of timer.
 */
void init_tc_adc(void)
{
	uint32_t ul_div = 0;
	uint32_t ul_tc_clks = 0;
	ul_sysclk = sysclk_get_cpu_hz();


	/* Enable peripheral clock. */
	pmc_enable_periph_clk(ID_TC0);
	
	pmc_disable_pck(PMC_PCK_3);
	pmc_switch_pck_to_sclk(PMC_PCK_3, PMC_PCK_PRES(0));
	pmc_enable_pck(PMC_PCK_3);

	/* Configure TC for a 1Hz frequency and trigger on RC compare. */
	tc_find_mck_divisor(1, ul_sysclk, &ul_div, &ul_tc_clks, ul_sysclk);
	tc_init(TC0, 0, ul_tc_clks | TC_CMR_CPCTRG | TC_CMR_WAVE |
	TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET);
	TC0->TC_CHANNEL[0].TC_RA = (ul_sysclk / ul_div) / 2;
	TC0->TC_CHANNEL[0].TC_RC = (ul_sysclk / ul_div) / 1;

	/* Start the Timer. */
	tc_start(TC0, 0);
}

static void adc_end_conversion(void)
{
	g_ul_value = adc_channel_get_value(ADC, ADC_CHANNEL_1);
	is_conversion_done = true;
	
	//uint32_t new_duty = (TC1->TC_CHANNEL[2].TC_RC * g_ul_value); // Rapport cyclique en fonction du signal
	//TC0->TC_CHANNEL[2].TC_RA = new_duty;
}

void init_adc(){
	
	adc_enable();
	adc_select_clock_source_mck(ADC);
	
	struct adc_config recorder_cfg;
	
	adc_get_config_defaults(&recorder_cfg);
	
	adc_init(ADC, &recorder_cfg);

	adc_channel_enable(ADC, ADC_CHANNEL_0);
	
	adc_set_trigger(ADC, ADC_TRIG_TIO_CH_0);
	
	adc_set_callback(ADC, ADC_INTERRUPT_EOC_1,
	adc_end_conversion, 1);

	adc_start_calibration(ADC);
}