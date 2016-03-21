/* levelparser.c
 * Author:	John Westhoff
 * Date:	12/10/2015
 * Course:	CSE20211
 *
 * This handles all of the level IO for the game.
 * It reads in the file and acts accordingly,
 * with some attempt at error-robustness.
 * For information on the level layout, see level_format.txt
 */
#include <stdio.h>
#include "levelparser.h"
#include "game.h"
#include "gfx_john.h"
#include "string.h"
#include "constants.h"
static FILE* current_level;
static unsigned int next_time = 0;
static unsigned int flag_time = 0;
static Condition current_condition = NONE_SET;
/* close_level
 * Closes the currently opened level file.
 */
void close_level()
{
	if (current_level!=NULL)
	{
		fclose(current_level);
	}
}
/* open_level
 * Arguments:
 * 	fname	-	the filename of the level
 * Returns:
 * 	SUCCESS_OPENING_LEVEL if the file opened successfully, ERROR_OPENING_LEVEL otherwise
 * Attempts to open a level file for parsing.
 */
int open_level(char *fname)
{
	close_level();
	current_condition = NONE_SET;
	current_level = fopen(fname,"r");
	if (current_level == NULL)
	{
		return ERROR_OPENING_LEVEL;
	}
	return SUCCESS_OPENING_LEVEL;
}
/* parse_level
 * Arguments:
 * 	enemies		-	the list of enemies (to add to, if need be)
 *	step		-	the current tick of the game clock
 *	lives		-	pointer to the player's lives, for modification (if need be)
 *	shipx		-	pointer to the player's x position, for modification (if need be)
 *	shipy		-	pointer to the player's y position, for modification (if need be)
 * Returns:
 * 	1 if the level reached EOF or the e command, 0 otherwise
 * Tells whether key is being held down
 */
int parse_level(Enemy *enemies, unsigned int step, unsigned char *lives, int *shipx, int *shipy)
{
	static char level_text[LEVELTEXT_SIZE] = "";
	johngfx_text_font(12,WINDOW_HEIGHT-28,level_text,SCORE_FONT);
	// Keep reading until we reach a point where we need to wait
	while (1)
	{
		// Handle the level flow commands
		switch (current_condition)
		{
			case NONE_SET:
			default:
			break;
			case TIMER:
				if (step>=next_time)
				{
					current_condition = NONE_SET;
				}
				else
				{
					return 0;
				}
			break;
			case ENEMIES_CLEARED:
				if (enemies==NULL||enemies->next==NULL)
				{
					current_condition = NONE_SET;
				}
				else
				{
					return 0;
				}
			break;
		}
		// Now, if we are in a state where we are ready to read a command, do it
		int c = '\0';
		int error_flag = 0;
		int t,x,y,h,r;
		unsigned int ut;
		do
		{
			error_flag = 0;
			c = fgetc(current_level);
			switch (c)
			{
				case 'e':
				case '\0':
				case EOF:
					return 1;
				break;
				case 'a':
					error_flag = (fscanf(current_level,"%d%d%d%d%d",&t,&x,&y,&h,&r)!=5); // Ensure we read 5 variables
					if (!error_flag)
					{
						insert_enemy(enemies, x, y, h, t, r, step);
					}
				break;
				case 'm':
					error_flag = (fscanf(current_level,"%d%d",&x,&y)!=2); // Ensure we read 5 variables
					if (!error_flag)
					{
						*shipx = x;
						*shipy = y;
					}
				break;
				case 'w':
					error_flag = (fscanf(current_level,"%u",&ut)!=1);
					if (!error_flag)
					{
						current_condition = TIMER;
						next_time = step+ut;
					}
				break;
				case 'k':
					current_condition = ENEMIES_CLEARED;
				break;
				case 'l':
					error_flag = (fscanf(current_level,"%u",&ut)!=1);
					if (!error_flag)
					{
						*lives = ut;
					}
				break;
				case 'c':
				{
					if (enemies!=NULL)
					{
						Enemy *e = enemies->next;
						while (e!=NULL)
						{
							explode(e);
							e = e->next;
						}
					}
				}
				break;
				case 's':
				{
					fgets(level_text,LEVELTEXT_SIZE,current_level);
					ungetc('\n',current_level);
					int len = strlen(level_text)-1;
					if (len>=0&&level_text[len]=='\n')
					{
						level_text[len]='\0';
					}
				}
				break;
				case 'f':
					flag_time = step;
				break;
				case 'T':
				{
					error_flag = (fscanf(current_level,"%u",&ut)!=1);
					if (!error_flag)
					{	
						if (step-flag_time>ut)
						{
							t = 0;
							while (t<2)
							{
								if (fgetc(current_level)=='\n')
								{
									t++;
								}
							}
							ungetc('\n',current_level);
						}
					}
				}
				break;
				case 'L':
				{
					char new_level[LEVELTEXT_SIZE];
					fscanf(current_level,"%s",new_level);
					open_level(new_level);
					return 0;
				}
				break;
				default:
					error_flag = 1;
				break;
			}
			error_flag = (fgetc(current_level)!='\n');
		}
		while (error_flag);
	}
	return 0;
}