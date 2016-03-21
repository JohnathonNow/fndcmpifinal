#ifndef MENU_INCLUDE
#define MENU_INCLUDE
#define MAX_ITEM	2
#define MAX_LEVEL	3

#define PLAY		0
#define	HELP		1
#define	QUIT		2

int handle_levelselect();
int handle_gameover();
void refresh_levels(int selected_item);
int handle_menu();
void refresh_menu(int selected_item);
int get_chosen_level();
int handle_help();
int handle_game_completed();
#endif
