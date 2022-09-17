#include <util.h>

int size_in_bytes(int argument){
	int i = 0;

	while (argument != 0){
		argument >>= 8;
		if (argument != 0) i++;
	}

	return i;
}

uint64_t hash_string(char* string){
	uint64_t hash = 5381;
	int c;

	while (c = *string++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

void create_label(char* name, uint32_t address, uint8_t defined) {
	_label_count++;
	_labels = realloc(_labels, sizeof(struct label) * _label_count);

	struct label* new_label = malloc(sizeof(struct label));
	memset(new_label, 0, sizeof(struct label));
	new_label->address = address;
	new_label->name = strdup(name);
	new_label->references = NULL;
	new_label->line = _line;
	new_label->defined = defined;

	_labels[_label_count - 1] = *new_label;
}

struct label* find_label(char* name) {
	for (uint32_t i = 0; i < _label_count; i++) {
		if (strcmp(name, _labels[i].name) == 0) {
			return &_labels[i];
		}
	}

	return NULL;
}

