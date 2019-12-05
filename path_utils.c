#include <stdbool.h>
#include "constants.h"

bool isRootPath(char* path) {
	return (path[0] == '/' && path[1] == '\0');
}

char* finalToken(char* path) {
	int i = 0, lastSep = 0;
	char c = path[i];

	do {
		if ( c == '/' ) {
			lastSep = i + 1;
		}
	} while ( '\0' != (c = path[++i]) );

	// If the path ends with '/', there is no final token.
	if ( strlen(path) == lastSep ) {
		return NULL;
	}

	return &(path[lastSep]);
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