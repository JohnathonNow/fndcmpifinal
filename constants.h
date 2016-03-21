/* constants.h
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * Provides global constants for use throughout the program.
 * For constants specific to a single file, see the corresponding
 * header file.
 */
#ifndef CONSTANT_INCLUDE
#define CONSTANT_INCLUDE

#define	WINDOW_WIDTH	600
#define	WINDOW_HEIGHT	600
#define	WINDOW_TITLE	"BULLET HFIL"

#define STATE_QUIT		-1
#define	STATE_MAIN_MENU	0
#define	STATE_GAME		1
#define STATE_LEVEL_SELECT	2
#define STATE_GAMEOVER 3
#define STATE_HELP_SCREEN 4
#define STATE_GAME_COMPLETE 5

#define FONT			"12x24"
#define SCORE_FONT		"6x9"
#define GAME_SPEED		10000
#endif