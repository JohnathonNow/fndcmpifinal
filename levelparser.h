#ifndef LEVELPARSE_INCLUDE
#define LEVELPARSE_INCLUDE
#include "game.h"
#define ERROR_OPENING_LEVEL 0
#define SUCCESS_OPENING_LEVEL 1
#define LEVELTEXT_SIZE 50
typedef enum {
	NONE_SET,
	TIMER,
	ENEMIES_CLEARED
} Condition;
void close_level();
int open_level(char *fname);
int parse_level(Enemy *enemies, unsigned int step, unsigned char *lives, int *shipx, int *shipy);
#endif