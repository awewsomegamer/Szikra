#include <writer.h>
#include <message_handler.h>

void write_byte(uint8_t byte) {
	if (!_error_flag)
		fwrite(&byte, 1, 1, _output_file);
}

void set_writer_position(uint64_t where) {
	fseek(_output_file, where, SEEK_CUR);
}

uint64_t get_writer_position() {
	fseek(_output_file, 0, SEEK_CUR);
	return ftell(_output_file);
}
