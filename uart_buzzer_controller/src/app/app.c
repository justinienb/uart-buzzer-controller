/*
 * CFile1.c
 *
 * Created: 20/12/2024 13:04:39
 *  Author: Toor
 */

#include <FreeRTOS.h>
#include <app.h>
#include <asf.h>
#include <uart_cli.h>
#include <math.h>


#define VOLT_REF        (3300)


/** The conversion data is done flag */
//volatile bool is_conversion_done = false;

/** The conversion data value */
//volatile uint32_t g_ul_value = 0;

/** The maximal digital value */
//static uint32_t g_max_digital;

#define ADC_RES ADC_12_BITS

#define ADC_MAX_VALUE (1<<(12+(ADC_RES>>16)))

uint32_t sysclk;

uint32_t current_digital;

uint8_t counter_flipflpp = 0;

uint32_t div_pwm_analog = 0;
uint32_t tc_clks_pwm_analog = 0;
uint32_t freq_pwm_analog = 200000;


float sin_time = 0;
uint32_t sin_freq = 500;
float sin_result = 0;


uint32_t pwm_counter = 0;
uint32_t pwm_counter_s = 0;


void app_init(){
	

	uart_cli_init();
	
	/* Configure TC */
	app_init_tc();
	
	//app_init_tc_adc_trigger();
	app_init_adc();

	
	app_init_tc_pwm();
	

	
	// UART CLI Task
	//xTaskCreate(uart_cli_task, (const signed char * const) "cli_task", 1024, NULL, 1, NULL);
	//xTaskCreate(app_button_control_task, (const signed char * const) "ctrl_task", 1024, NULL, 0, NULL);

}


void app_run(){
	
	//start tasks
    //vTaskStartScheduler();
	//main task
	while (1) {
		uart_cli_read_command();
		//app_button_control_task();
	}
}

void vApplicationMallocFailedHook(void) {
	// Gérer l'erreur d'allocation mémoire ici (par ex., LED clignotante, message d'erreur)
	for(;;);
}
//
//void app_button_control_task(void *pvParameters){
//
	//while (1) {
		////Is button pressed?
		//if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			//button_pressed = true;
			//// Yes, so turn LED on.
			//ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
			////play buzzer
//
		//} else {
			//button_pressed = false;
			//// No, so turn LED off.
			//ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
			////stop buzzer
//
		//}
	//}
	//
//}

void app_init_tc(void) {

	/* Enable PCK output */
	pmc_disable_pck(PMC_PCK_3);
	pmc_switch_pck_to_sclk(PMC_PCK_3, PMC_PCK_PRES(0));
	pmc_enable_pck(PMC_PCK_3);
	//
	///* Activer l'horloge pour TC0 */
	pmc_enable_periph_clk(ID_TC0);
	///* Activer l'horloge pour TC0 */
	pmc_enable_periph_clk(ID_TC1);
	
	NVIC_DisableIRQ(TC_IRQn);
	NVIC_ClearPendingIRQ(TC_IRQn);
	NVIC_SetPriority(TC_IRQn, 0);
	NVIC_EnableIRQ(TC_IRQn);
	
}

void app_init_tc_pwm(void) {
	
	sysclk = sysclk_get_cpu_hz();
	
	/* Configurer la broche TIOA1 pour la sortie PWM */
	ioport_set_pin_mode(PIN_TC0_TIOA1, PIN_TC0_TIOA1_MUX);
	ioport_disable_pin(PIN_TC0_TIOA1);

	///* Trouver un diviseur pour une fréquence PWM cible (par ex., 1 kHz) */
	uint8_t found =  tc_find_mck_divisor(freq_pwm_analog, sysclk, &div_pwm_analog, &tc_clks_pwm_analog, sysclk);

	/* Configurer TC0 en mode PWM */
	tc_init(TC0, 1, tc_clks_pwm_analog | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_SET | TC_CMR_ACPC_CLEAR);
	/* Configurer RA et RC pour définir la fréquence et le rapport cyclique */
	TC0->TC_CHANNEL[1].TC_RC = (uint32_t)( (float)(sysclk / div_pwm_analog / freq_pwm_analog));			// RC : période complète (1 Hz)
	TC0->TC_CHANNEL[1].TC_RA = (uint32_t)( (float)(sysclk / div_pwm_analog / freq_pwm_analog) /2 );
	
	printf("----- INIT -----\n");
	printf("div : %d\n", div_pwm_analog);
	printf("clk : %d\n", (int) sysclk);
	printf("tc_clk : %d\n", (int) tc_clks_pwm_analog);
	printf("found : %d\n", (uint8_t) found);
	printf("----------------\n");

	//tc_enable_interrupt(TC0, 1, TC_IER_CPCS);
	
	/* Démarrer TC0 */
	tc_start(TC0, 1);
	
}



void TC_Handler(void){
	
	tc_get_status(TC0, 1);  // Lire le statut pour effacer l'IRQ

	pwm_counter++;
	
	sin_time = (float)pwm_counter/(float)freq_pwm_analog;
	
	sin_result =  0.5 + 0.4*sin( 2*M_PI*sin_freq*sin_time);
	
	// Applique le duty cycle
	TC0->TC_CHANNEL[1].TC_RA = TC0->TC_CHANNEL[1].TC_RC * sin_result -1;

	if (counter_flipflpp < 3){
		printf("%2.2f\n", sin_result);	
		printf("RC : %d\n",TC0->TC_CHANNEL[1].TC_RC);
		printf("RA : %d\n",TC0->TC_CHANNEL[1].TC_RA);
		printf("\n");
		counter_flipflpp++;
	}
		


}


/**
 * \brief Configure to trigger ADC by TIOA output of timer.
 */
void app_init_tc_adc_trigger(void)
{

	uint32_t div = 0;
	uint32_t tc_clks = 0;
	uint32_t freq = 1;

	sysclk = sysclk_get_cpu_hz();

	/* Configurer la broche TIOA1 pour la sortie PWM */
	ioport_set_pin_mode(PIN_TC0_TIOA0, PIN_TC0_TIOA0_MUX);
	ioport_disable_pin(PIN_TC0_TIOA0);

	///* Trouver un diviseur pour une fréquence PWM cible (par ex., 1 kHz) */
	uint8_t found =  tc_find_mck_divisor(freq, sysclk, &div, &tc_clks, sysclk);

	printf("div : %d\n", (uint8_t) div);
	printf("clk : %d\n", (int) sysclk);
	printf("tc_clk : %d\n", (int) tc_clks);
	printf("found : %d\n", (uint8_t) found);

	/* Configurer TC0 en mode PWM */
	tc_init(TC0, 0, tc_clks | TC_CMR_CPCTRG | TC_CMR_WAVE | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET);
	/* Configurer RA et RC pour définir la fréquence et le rapport cyclique */
	TC0->TC_CHANNEL[0].TC_RC = sysclk / div / freq;			// RC : période complète (1 Hz)
	TC0->TC_CHANNEL[0].TC_RA = sysclk / div / freq /2;

	printf("\n");
	printf("start conversion...\n");

	/* Démarrer TC0 */
	tc_start(TC0, 0);
}

//static void adc_data_ready(void)
//{
	//current_digital = adc_channel_get_value(ADC, ADC_CHANNEL_0);
	////printf("%d\n", (int)data_point);
	//
	////uint32_t new_duty = (TC1->TC_CHANNEL[2].TC_RC * g_ul_value); // Rapport cyclique en fonction du signal
	////TC0->TC_CHANNEL[2].TC_RA = new_duty;
//}

static void adc_end_conversion(void)
{
	current_digital = adc_channel_get_value(ADC, ADC_CHANNEL_1);
	current_digital = (current_digital*VOLT_REF)/ADC_MAX_VALUE;
	TC0->TC_CHANNEL[1].TC_RA = TC0->TC_CHANNEL[1].TC_RC * ((float)current_digital/3300.0);

}

void app_init_adc(){
	
	adc_enable();
	
	adc_select_clock_source_mck(ADC);
	
	struct adc_config recorder_cfg;
	
	adc_get_config_defaults(&recorder_cfg);
	
	adc_init(ADC, &recorder_cfg);

	adc_channel_enable(ADC, ADC_CHANNEL_1);
	
	adc_set_trigger(ADC, ADC_TRIG_TIO_CH_1);
		
	adc_set_callback(ADC, ADC_INTERRUPT_EOC_1, adc_end_conversion, 1);

	adc_set_resolution(ADC, ADC_RES);	

	adc_start_calibration(ADC);
	
	//adc_start_software_conversion(ADC);
	
}