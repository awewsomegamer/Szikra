#include <message_handler.h>
#include <global.h>

void fatal_error(char* format, ...){
	printf("[FATAL ERROR]: ");

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	printf(" [LINE: %d]\n", _line);

	exit(1);
}

void error(char* format, ...){
	printf("[ERROR]: ");

	va_list args;
	va_start(args, format);

	_error_detected = true;

	vprintf(format, args);

	printf(" [LINE: %d]\n", _line);
}

void warn(char* format, ...){
	printf("[WARNING]: ");

	va_list args;
	va_start(args, format);

	vprintf(format, args);

	printf(" [LINE: %d]\n", _line);
}

void debug(char* format, ...){
	if (_debug){
		printf("[DEBUG]: ");
		
		va_list args;
		va_start(args, format);

		vprintf(format, args);

		printf(" [LINE: %d]\n", _line);
	}

}