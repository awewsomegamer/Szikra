#include <writer.h>

void write_byte(uint8_t byte){
	fwrite(&byte, 1, 1, _out_file); // Write given byte
}

void write_word(uint16_t word){
	write_byte(word & 0xFF);	// Write low byte
	write_byte((word >> 8) & 0xFF); // Write high byte
}

void write_dword(uint32_t dword){
	write_word(dword & 0xFFFF);	    // Write low word
	write_word((dword >> 16) & 0xFFFF); // Write high word
}

void write_bytes(uint8_t* bytes, size_t length){
	fwrite(bytes, 1, length, _out_file);
}

void write_words(uint16_t* word, size_t length){
	fwrite(word, 2, length, _out_file);
}

void write_dwords(uint32_t* dword, size_t length){
	fwrite(dword, 4, length, _out_file);
}

// Size: in bytes - 1
void write(int what, int size){
	fwrite(&what, size + 1, 1, _out_file);
}

// Size: in bytes - 1
void write_array(int* what, int size, size_t count){
	fwrite(what, size + 1, count, _out_file);
}

void set_write_position(int where){
	fseek(_out_file, where, SEEK_SET);
}

int get_write_position(){
	fseek(_out_file, 0, SEEK_CUR);
	return ftell(_out_file);
}
