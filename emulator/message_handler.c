#include <message_handler.h>

void error(char* form, ...) { 
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


