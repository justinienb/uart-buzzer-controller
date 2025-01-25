/*
 * app.h
 *
 * Created: 22/01/2025 00:06:56
 *  Author: Toor
 */

#ifndef APP_H
#define APP_H

void app_init(void);

void init_tc_pwm(void);
void init_tc_adc(void);
void init_uart_console(void);

void app_run(void);

void configure_tc_trigger(void);

void init_adc(void);

#endif