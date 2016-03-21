/* final.c
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * The final project for Fund Comp I,
 * this final handles the state machine representing
 * the game, including the setup of various gfx related things.
 */
#include <stdio.h>
#include <string.h>
#include "gfx_john.h"
#include "menu.h"
#include "game.h"
#include "control.h"
#include "constants.h"
int main()
{
	char game_state = STATE_MAIN_MENU;
	gfx_open(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_TITLE);
	gfx_changefont(FONT);
	while (1)
	{
		update_keys();
		switch (game_state)
		{
			case STATE_MAIN_MENU:
				game_state = handle_menu();
			break;
			case STATE_LEVEL_SELECT:
				game_state = handle_levelselect();
			break;
			case STATE_GAME:
				game_state = handle_game();
			break;
			case STATE_GAMEOVER:
				game_state = handle_gameover();
			break;
			case STATE_HELP_SCREEN:
				game_state = handle_help();
			break;
			case STATE_GAME_COMPLETE:
				game_state = handle_game_completed();
			break;
			case STATE_QUIT:
			return 0;
		}
	}
	return 0;
}
