#include "main.h"

static void sysError(char const *filename, int fd)
{
	warn("%s", filename);
	close(fd);
}

static void fileError(char const *filename, char *error, int fd)
{
	warnx("%s: %s", filename, error);
	close(fd);
}

static uint8_t *readKeyFile(char const *filename)
{
	static uint8_t fileKey[MAXKEYBYTES + BLOCKSIZE];
	uint8_t flags[FLAG_COUNT];
	ssize_t readBytes;

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (sysError(filename, fd), NULL);

	readBytes = read(fd, flags, sizeof(flags));
	if (readBytes == -1)
		return (sysError(filename, fd), NULL);
	if (readBytes != sizeof(flags))
		return (fileError(filename, "File is too short", fd), NULL);
	if (flags[ENDIAN_FLAGS] != LITTLE_ENDIAN_FLAG)
		return (fileError(filename, "File is not little-endian", fd), NULL);
	if (flags[COMPRESSED_FLAG] == 1)
		return (fileError(filename, "File is compressed", fd), NULL);

	lseek(fd, -sizeof(fileKey), SEEK_END);

	readBytes = read(fd, fileKey, sizeof(fileKey));
	if (readBytes == -1)
		return (sysError(filename, fd), NULL);
	if (readBytes != sizeof(fileKey))
		return (fileError(filename, "File is too short", fd), NULL);

	close(fd);
	return (fileKey);
}

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		dprintf(STDERR_FILENO, "Usage: %s ENCRYPTED_FILE KEY_SIZE KEY_WORD...\n", argv[0]);
		return (EXIT_FAILURE);
	}
	size_t keyLen = atoi(argv[2]);
	if (keyLen < BLOCKSIZE)
	{
		dprintf(STDERR_FILENO, "Key must be at least 8 bytes long\n");
		return (EXIT_FAILURE);
	}
	uint8_t *fileKey = readKeyFile(argv[1]);
	if (fileKey == NULL)
		return (EXIT_FAILURE);
	char *key;
	if (scrambleKeysTester(fileKey, &key, keyLen, argv + 3, argc - 3))
	{
		printf(GREEN "SUCCESS:" RESET " %s\n", key);
		free(key);
		return (EXIT_SUCCESS);
	}
	else
	{
		printf(RED "FAILURE\n" RESET);
		return (EXIT_FAILURE);
	}
}