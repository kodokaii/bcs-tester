#include "scramble.h"

static void scrambleCTXInit(ScrambleCTX *sctx, char **words, unsigned wordCount, size_t keyLen)
{
	sctx->words = words;
	sctx->wordCount = wordCount;
	sctx->wordIndex = calloc(keyLen, sizeof(int));
	sctx->keyLen = keyLen;
}

static void generateKey(ScrambleCTX *sctx, char *key)
{
	size_t k = 0;

	for (unsigned i = 0; i < sctx->keyLen && k < sctx->keyLen; ++i)
	{
		char *word = sctx->words[sctx->wordIndex[i]];
		size_t wordSize = strlen(word);
		if (sctx->keyLen < k + wordSize)
			wordSize = sctx->keyLen - k;
		memcpy(key + k, word, wordSize);
		k += wordSize;
	}
	key[k] = '\0';
}

static bool increment(ScrambleCTX *sctx)
{
	unsigned i = 0;
	while (i < sctx->keyLen && sctx->wordIndex[i] + 1 == sctx->wordCount)
		sctx->wordIndex[i++] = 0;
	if (i < sctx->wordCount)
		sctx->wordIndex[i]++;
	else
		return (false);
	return (true);
}

static bool exitTester(ScrambleCTX *sctx, bool result)
{
	free(sctx->wordIndex);
	return (result);
}

bool scrambleKeysTester(uint8_t *fileKey, char **key, size_t keyLen, char **words, unsigned wordCount)
{
	ScrambleCTX sctx;

	scrambleCTXInit(&sctx, words, wordCount, keyLen);
	*key = malloc(keyLen + 1);
	do
	{
		generateKey(&sctx, *key);
		if (BCTester(*key, fileKey))
			return (exitTester(&sctx, true));
	} while (increment(&sctx));
	free(*key);
	*key = NULL;
	return (exitTester(&sctx, false));
}