/*
 * usrin.h
 *
 *  Created on: Jan 26, 2018
 *      Author: Dr. Saldon
 */

/*
 * User input parser
 *
 * Набор стандартных средств для обработки пользовательского ввода (аргументов).
 */

#ifndef USRIN_H_
#define USRIN_H_

/*-----------------------------------------------------------------------------
MACRO SECTION
-----------------------------------------------------------------------------*/
#define CT_RED	 	"\033[1;31m"		// Console text color: red
#define CT_NC 		"\033[0m"			// Console text color: default

/*-----------------------------------------------------------------------------
INCLUDE SECTION
-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*-----------------------------------------------------------------------------
GLOBAL VARIABLES SECTION
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
HEADER SECTION
-----------------------------------------------------------------------------*/
uint8_t find_argv(int argc, char* argv[], const char* arg);
void parse_self_destruct(int argc, char* argv[]);

/*-----------------------------------------------------------------------------
IMPLEMENTATION SECTION
-----------------------------------------------------------------------------*/

/**
 * Find argument value
 *
 * Поиск строки const char* arg в списке аргументов. Возвращает 0 если
 * искомая строка не найдена или номер агрумента, в котором содержится искомая строка.
 */
uint8_t find_argv(int argc, char* argv[], const char* arg)
{
	if(argc > 1){
		for(uint8_t i = 0; i < argc; i++)
		{
			if(strcmp(argv[i], arg) == 0){
				return i;
			}
		}
	}
	return 0;
}

void parse_self_destruct(int argc, char* argv[])
{
	// Код, применяемый для самоуничтодения исполняемого файла приложения
	// Если указан параметр самоуничтожения -sd, очищаем все следы и завершаем работу
	if(find_argv(argc, argv, "-sd") != 0){
		fprintf(stdout, "\r\n\r\n %s APPLICATION SELF DESTRUCT... %s \r\n\r\n", CT_RED, CT_NC);
		system("rm -r -v ../Debug << y");
		fprintf(stdout, "\r\n %s -DONE- %s \r\n\r\n", CT_RED, CT_NC);
	}
}

#endif /* USRIN_H_ */
