/*
 * uart_cli.c
 *
 * Created: 25/01/2025 03:44:59
 *  Author: Toor
 */ 

#include <ioport.h>
#include <uart_cli.h>
#include <uart_serial.h>
#include <stdio_serial.h>
#include <string.h>
#include <tc.h>

extern int sysclk;

void uart_cli_init(){
	
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


void uart_cli_task(void *pvParameters){
	
	int choice = 0;
	
	while (1){
		
		//printf("\e[1;1H\e[2J");
				
		uart_cli_display_frequencies(&choice);
		
		//blocking call
		
		uart_cli_read_choice(&choice);
		
		int div = 32;
		
		switch(choice){
			case 0:
				div = 32;
				break;
			case 1:
				div = 64;
				break;
			case 2:
				div = 128;
				break;
			case 3:
				div = 256;
				break;
			case 4:
				div = 512;
				break;
			case 5:
				div = 1024;
				break;
			case 6:
				div = 2048;
				break;
			case 7:
				div = 4096;
				break;
			case 8:
				div = 8192;
				break;
			case 9:
				div = 16284;
				break;
			default:
				choice = 0;
				div = 32;
				break;
		}
		
		
		TC0->TC_CHANNEL[0].TC_RC = (sysclk / 128) / div;			// RC : période complète (1 Hz)
		TC0->TC_CHANNEL[0].TC_RA = ((sysclk / 128) / div) / 2;
	}
}


void uart_cli_display_frequencies(int* num){
	printf(	"┌── Frequencies menu ──┐\n"
			"│ 0 : 32Hz             │\n"
			"│ 1 : 64Hz             │\n"
			"│ 2 : 128Hz            │\n"
			"│ 3 : 256Hz            │\n"
			"│ 4 : 512Hz            │\n"
			"│ 5 : 1024Hz           │\n"
			"│ 6 : 2048Hz           │\n"
			"│ 7 : 4096Hz           │\n"
			"│ 8 : 8192Hz           │\n"
			"│ 9 : 16384Hz          │\n"
			"├──────────────────────┤\n"
			"│ Current : %d          │\n"
			"└──────────────────────┘\n", *num);

}


void uart_cli_read_choice(int* choice){
	
}

void uart_cli_read_command(void){
	struct Command command;
	scanf("%c %d", &command.name, (int)&command.value);
	fflush(stdin);
	switch(command.name){
		case 'A':
			TC0->TC_CHANNEL[1].TC_RA = command.value;
			break;
		case 'C':
			tc_stop(TC0, 1);
			TC0->TC_CHANNEL[1].TC_RC = command.value;
			tc_start(TC0, 1);
			break;
		default:
			printf("command unrecognised\n");
			break;
	}
	
	printf("RC : %d\n",TC0->TC_CHANNEL[1].TC_RC);
	printf("RA : %d\n",TC0->TC_CHANNEL[1].TC_RA);
	printf("\n");
}

void uart_cli_set_tc_frequency(){
	
}