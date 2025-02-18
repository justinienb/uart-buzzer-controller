/*
 * uart_cli.h
 *
 * Created: 25/01/2025 03:46:02
 *  Author: Toor
 */ 


#ifndef UART_CLI_H_
#define UART_CLI_H_


//CLI
void uart_cli_task(void *pvParameters);

// Initialisation de l'interface CLI UART
void uart_cli_init(void);

// Affichage du menu des fréquences sur la CLI UART
void uart_cli_display_frequencies(int* num);

// Lecture du choix de l'utilisateur via la CLI UART
void uart_cli_read_choice(int* choice);

// Définir la fréquence de TC via la CLI UART
void uart_cli_set_tc_frequency(void);

void empty_stdin(void);

void uart_cli_read_command(void);


struct Command {
	char name;
	uint32_t value;
};

#endif /* UART_CLI_H_ */