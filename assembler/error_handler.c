#include <error_handler.h>
#include <global.h>

void fatal_error(char* format, ...){
	printf("[FATAL ERROR]: ");

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	printf("\n");

	exit(1);
}

void error(char* format, ...){
	printf("[ERROR]: ");

	va_list args;
	va_start(args, format);

	_error_detected = true;

	vprintf(format, args);

	printf("\n");
}

void warn(char* format, ...){
	printf("[WARNING]: ");

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	printf("\n");
}