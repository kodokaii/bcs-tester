#include "scramble.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"