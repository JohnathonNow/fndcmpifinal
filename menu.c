/* menu.c
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * Handles the various menus of the game,
 * really, any screen with text on it.
 */
#include <stdio.h>
#include <unistd.h>
#include "menu.h"
#include "gfx_john.h"
#include "control.h"
#include "constants.h"
#include "john_lib.h"
static int chosen_level = 0;
/* handle_menu
 * Returns:
 * 	The new game state to enter
 * Handles the in-game main menu.
 */
int handle_menu()
{	
	static char selected_item = 0;
	refresh_menu(selected_item);
	while (1)
	{
		update_keys();
		if (key_pressed('s'))
		{
			if (selected_item<MAX_ITEM)
			{
				refresh_menu(++selected_item);
			}
		}
		if (key_pressed('w'))
		{
			if (selected_item>0)
			{
				refresh_menu(--selected_item);
			}
		}
		if (key_released(' '))
		{
			switch (selected_item)
			{
				case QUIT: return STATE_QUIT;
				case PLAY: return STATE_LEVEL_SELECT;
				case HELP: return STATE_HELP_SCREEN;
			}
		}
	}
	return STATE_MAIN_MENU;
}
/* handle_levelselect
 * Returns:
 * 	The new game state to enter
 * Handles the level selection screen.
 */
int handle_levelselect()
{	
	static char selected_level = 0;
	refresh_levels(selected_level);
	while (1)
	{
		update_keys();
		if (key_pressed('s'))
		{
			if (selected_level<MAX_LEVEL)
			{
				refresh_levels(++selected_level);
			}
		}
		if (key_pressed('w'))
		{
			if (selected_level>0)
			{
				refresh_levels(--selected_level);
			}
		}
		if (key_pressed('\b'))
		{
			return STATE_MAIN_MENU;
		}
		if (key_released(' '))
		{
			chosen_level = selected_level;
			return STATE_GAME;
		}
	}
	return STATE_LEVEL_SELECT;
}
/* get_chosen_level
 * Returns:
 * 	The number representing the level chosen at the level select screen
 * Gets the chosen level.
 */
int get_chosen_level()
{
	return chosen_level;
}
/* refresh_menu
 * Arguments:
 * 	selected_item		-	the currently highlighted item
 * Redraws the main menu.
 */
void refresh_menu(int selected_item)
{
	static char menu_items[3][5] = {"PLAY","HELP","QUIT"};
	static int width[3] = {48,48,48};
	static int height[3] = {22,22,22};
	gfx_clear();
	gfx_text((WINDOW_WIDTH-13*11)/2,100,"BULLET HFIL");
	gfx_text(12,556,"Navigate with W and S.");
	gfx_text(12,578,"Select with SPACE.");
	int i;
	for (i = 0; i <= MAX_ITEM; i++)
	{
		gfx_text((WINDOW_WIDTH-width[i])/2,200+100*i,menu_items[i]);
		if (i==selected_item)
		{
			gfx_rectangle((WINDOW_WIDTH-width[i])/2-1,200+100*i-height[i]-1,width[i]+2,height[i]+2);
		}
	}
	gfx_flush();
}
/* refresh_levels
 * Arguments:
 * 	selected_item		-	the currently highlighted item
 * Redraws the level select screen.
 */
void refresh_levels(int selected_item)
{
	static int width = 54;
	static int height = 22;
	gfx_clear();
	gfx_text(12,556,"Navigate with W and S.");
	gfx_text(12,578,"Select with SPACE.");
	int i;
	for (i = 0; i <= MAX_LEVEL; i++)
	{
		gfx_text((WINDOW_WIDTH-width)/2,100+100*i,"LEVEL");
		gfx_text((WINDOW_WIDTH)/2+width,100+100*i,num2str(i));
		if (i==selected_item)
		{
			gfx_rectangle((WINDOW_WIDTH-width)/2-1,100+100*i-height-1,width*3/2+18,height+2);
		}
	}
	gfx_flush();
}
/* handle_gameover
 * Returns:
 * 	The new game state to enter
 * Handles the gameover screen.
 */
int handle_gameover()
{
	static int width = 13*9;
	int i;
	for (i = 0; i <= WINDOW_HEIGHT; i++)
	{
		gfx_color(0,0,0);
		gfx_line(0,i,WINDOW_WIDTH,i);
		gfx_color(255,255,255);
		gfx_text((WINDOW_WIDTH-width)/2,300,"GAME OVER");
		update_keys();
		gfx_flush();
		usleep(GAME_SPEED/2);
	}
	return STATE_MAIN_MENU;
}
/* handle_game_completed
 * Returns:
 * 	The new game state to enter
 * Handles the "you win" screen.
 */
int handle_game_completed()
{
	static int width = 13*11;
	int i;
	for (i = 0; i <= WINDOW_HEIGHT; i++)
	{
		gfx_color(0,0,0);
		gfx_line(0,i,WINDOW_WIDTH,i);
		gfx_color(255,255,255);
		gfx_text((WINDOW_WIDTH-width)/2,300,"You did it!");
		update_keys();
		gfx_flush();
		usleep(GAME_SPEED/2);
	}
	return STATE_MAIN_MENU;
}
/* handle_help
 * Returns:
 * 	The new game state to enter
 * Handles the help page.
 */
int handle_help()
{
	static int width = 54;
	gfx_clear();
	gfx_text((WINDOW_WIDTH-width)/2,36,"HELP");
	gfx_text(12,82,"Use WASD to move and SPACE to shoot.");
	gfx_text(12,104,"Your score is in the top left,");
	gfx_text(12,126,"your lives are in the top right.");
	gfx_text(12,148,"The meter in the middle is your energy,");
	gfx_text(12,170,"If you press SHIFT, it will drain,");
	gfx_text(12,192,"but time will slow down.");
	gfx_text(12,214,"Press SHIFT again and things return to normal.");
	gfx_text(12,236,"Do not run out of energy.");
	gfx_text(12,578,"Press BACKSPACE to return to the main menu.");
	while (!key_pressed('\b'))
	{
		update_keys();
	}
	return STATE_MAIN_MENU;
}
