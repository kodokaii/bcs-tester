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
	if (i < sctx->keyLen)
		sctx->wordIndex[i]++;
	else
		return (false);
	return (true);
}

void progress_bar(size_t total, size_t current)
{
	int percentage = (100 * current) / total;

	write(1, "\r\e[J[\e[38;2;163;255;149;1m", 26);
	write(1, "####################################################################################################", percentage);
	write(1, "                                                                                                    ", 100 - percentage);
	write(1, "\e[0m]", 5);
}

static bool exitTester(ScrambleCTX *sctx, bool result)
{
	progress_bar(100, 100);
	write(1, "\n", 1);
	free(sctx->wordIndex);
	return (result);
}

bool scrambleKeysTester(uint8_t *fileKey, char **key, size_t keyLen, char **words, unsigned wordCount)
{
	ScrambleCTX sctx;
	size_t progress = 0;
	size_t progressMax = pow(wordCount, keyLen);
	size_t progressStep = progressMax / 100;

	scrambleCTXInit(&sctx, words, wordCount, keyLen);
	*key = malloc(keyLen + 1);
	do
	{
		if (progress % progressStep == 0)
			progress_bar(progressMax, progress);
		generateKey(&sctx, *key);
		if (BCTester(*key, fileKey))
			return (exitTester(&sctx, true));
		progress++;
	} while (increment(&sctx));
	free(*key);
	*key = NULL;
	return (exitTester(&sctx, false));
}