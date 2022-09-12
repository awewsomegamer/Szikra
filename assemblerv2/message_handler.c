#include <message_handler.h>

uint8_t _error_flag = 0;
uint8_t _debug = 0;

void error(char* form, ...) { 
	_error_flag = 1;
	
	va_list args;
	va_start(args, form);

	printf("[ERROR]: ");
	vprintf(form, args);
	printf("\n");
}

void fatal_error(char* form, ...) { 
	va_list args;
	va_start(args, form);

	printf("[FATAL ERROR]: ");
	vprintf(form, args);
	printf("\n");
}

void warn(char* form, ...) { 
	va_list args;
	va_start(args, form);

	printf("[WARN]: ");
	vprintf(form, args);
	printf("\n");
}

void debug(char* format, ...){
	if (_debug) {
		printf("[DEBUG]: ");
		
		va_list args;
		va_start(args, format);

		vprintf(format, args);

		printf(" [LINE: %d]\n", _line);
	}
}