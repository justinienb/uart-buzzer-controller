/*
 * app.h
 *
 * Created: 22/01/2025 00:06:56
 *  Author: Toor
 */

#ifndef APP_H
#define APP_H

void vApplicationMallocFailedHook(void);

void app_button_control_task(void *pvParameters);

void app_init(void);

void app_init_tc(void);

void app_init_tc_pwm(void);

void app_init_tc_adc_trigger(void);

void app_init_adc(void);

void app_run(void);

//static void adc_data_ready(void);

#endif