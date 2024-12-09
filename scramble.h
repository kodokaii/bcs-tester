#include "bcrypt_tester.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define EMPTY_SEPARATOR '\x1f'
#define SEPARATORS "\x1f _-/"
#define SEPARATOR_COUNT 5

typedef struct
{
	char **words;
	unsigned wordCount;
	unsigned *wordIndex;
	size_t keyLen;
} ScrambleCTX;

bool scrambleKeysTester(uint8_t *fileKey, char **key, size_t keyLen, char **words, unsigned wordCount);