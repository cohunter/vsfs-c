#include <stdbool.h>
#include "constants.h"

bool isRootPath(char* path) {
	return (path[0] == '/' && path[1] == '\0');
}
/*
bool nextPathComponent();

char* lastPathComponent(char* path) {
	char temp[DIRNAME_MAX_LEN + 1];
	strncpy(temp, path, DIRNAME_MAX_LEN);

	char* pos;
	char tok, prevtok;
	for (tok = strtok_r(temp, "/", &pos); prevtok = tok, tok != NULL; tok = strtok_r(temp, "/", &pos));
	return prevtok;
}
*/