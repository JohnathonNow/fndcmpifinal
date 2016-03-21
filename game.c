/* game.c
 * Author:	John Westhoff
 * Date:	12/08/2015
 * Course:	CSE20211
 *
 * This is the meat of the code, the game itself, in all its glory/horror.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "game.h"
#include "menu.h"
#include "gfx_john.h"
#include "control.h"
#include "constants.h"
#include "john_lib.h"
#include "levelparser.h"
/* handle_game
 * Returns:
 * 	The new game state to enter
 * Handles the game itself.
 */
int handle_game()
{
	// This code here all runs when we enter this state
	// Let's set up all our important variables
	int score = 0;
	char score_text[30] = SCORE_PREFIX "0";
	unsigned char lives = 3;
	Bullet bullets = (Bullet){-1000,0,0,0,NULL}; // The list bullet, the start
	Enemy enemies = (Enemy){-1000,0,0,0,0,0,0,SENTINEL,NULL}; // The list enemy, the start
	int ship_x = WINDOW_WIDTH/2;
	int	ship_y = WINDOW_HEIGHT*3/4;
	unsigned int step = 0;
	unsigned int gunstep = 0;
	int hits = 0;
	int rewind = 0;
	char levelt[15];
	int deltat = 0;
	int count_cycles = 0;
	int energy = MAX_ENERGY;
	// And now we'll try starting the level
	snprintf(levelt,15,"level%d.dat",get_chosen_level());
	if (open_level(levelt)==ERROR_OPENING_LEVEL)
	{
		printf("Unable to open level: ~%s~!\n",levelt);
		return STATE_LEVEL_SELECT;
	}
	// Next, let's play the game!
	while (1)
	{
		// Handle the game clock and the shot clock
		gunstep++;
		if (deltat)
		{
			if (++count_cycles>=1/SLOWED_DOWN)
			{
				count_cycles = 0;
				step++;
			}
		}
		else
		{
			count_cycles = 0;
			step++;
		}
		// Step through the level
		if (parse_level(&enemies, step, &lives, &ship_x, &ship_y))
		{
			return STATE_GAME_COMPLETE;
		}
		//  Handle the player controls
		update_keys();
		if (key_down('d')){ship_x+=SHIP_SPEED;}
		if (key_down('a')){ship_x-=SHIP_SPEED;}
		if (key_down('w')){ship_y-=SHIP_SPEED;}
		if (key_down('s')){ship_y+=SHIP_SPEED;}
		ship_x = ship_x>WINDOW_WIDTH?WINDOW_WIDTH:ship_x<0?0:ship_x;
		ship_y = ship_y>WINDOW_HEIGHT?WINDOW_WIDTH:ship_y<0?0:ship_y;
		if (energy>0&&(key_pressed(LEFT_SHIFT)||key_pressed(RIGHT_SHIFT)))
		{
			deltat = !deltat;
		}
		if (deltat)
		{
			energy-=ENERGY_DRAIN_RATE;
			if (energy<=0)
			{
				deltat = 0;
				energy = -ENERGY_PENALTY;
			}
		}
		else
		{
			energy=energy<MAX_ENERGY?energy+ENERGY_RECHARGE_RATE:MAX_ENERGY;
		}
		if (key_down(' '))
		{
			if ((gunstep)%3==0)
			{
				insert_bullet(&bullets,ship_x,ship_y,0,-20);
			}
			if ((gunstep)%2)
			{
				insert_bullet(&bullets,ship_x,ship_y,-(int)(gunstep%5),-20);
			}
			else
			{
				insert_bullet(&bullets,ship_x,ship_y,(gunstep)%5,-20);
			}
		}
		if (key_pressed('\b'))
		{
			while (bullets.next!=NULL)
			{
				remove_bullet(&bullets,bullets.next);
			}
			while (enemies.next!=NULL)
			{
				remove_enemy(&enemies,enemies.next);
			}
			return STATE_MAIN_MENU;
		}
		// Draw stuff and advance through logic
		gfx_clear();
		if ((rewind <= 0)||(step%20<10))
		{
			draw_ship(ship_x,ship_y);
		}
		hits = handle_bullets(&bullets,&enemies);
		if (hits>0)
		{
			increase_score(&score,10*hits,score_text);
		}
		if (handle_enemies(&enemies,step,ship_x,ship_y,deltat,count_cycles))
		{
			if (lives>0&&rewind<=0)
			{
				lives--;
				step=step>REWIND_TIME?step-REWIND_TIME:0;
				rewind = REWIND_TIME;
				deltat = 0;
			}
		}
		if (lives==0)
		{
			while (bullets.next!=NULL)
			{
				remove_bullet(&bullets,bullets.next);
			}
			while (enemies.next!=NULL)
			{
				remove_enemy(&enemies,enemies.next);
			}
			return STATE_GAMEOVER;
		}
		handle_rewind(&enemies,&rewind);
		draw_hud(score_text, lives, energy);
		usleep(GAME_SPEED);
		gfx_flush();
	}
}
/* draw_ship
 * Arguments:
 * 	x	-	the location of the ship horizontally
 *	y	-	the location of the ship vertically
 * Draws a super-realistic spaceship (aka a triangle).
 */
void draw_ship(int x, int y)
{
	gfx_line(x,y-SHIP_SIZE,x+SHIP_SIZE/2,y);
	gfx_line(x-SHIP_SIZE/2,y,x+SHIP_SIZE/2,y);
	gfx_line(x,y-SHIP_SIZE,x-SHIP_SIZE/2,y);
}
/* handle_rewind
 * Arguments:
 * 	list	-	the list of enemies
 *	rewind	-	a pointer to the rewind timer
 * When you get shot, the game rewinds.
 * This pushes all enemies up, destroys bullets, and
 * decreases the rewind counter. (Only if we're rewinding, though.)
 */
void handle_rewind(Enemy* list, int* rewind)
{
	if (*rewind>0)
	{
		(*rewind)--;
		while ((list!=NULL))
		{
			if (list->type==BULLET)
			{
				explode(list);
			}
			list->y -= list->type==EXPLOSION?0:SCROLL_SPEED*3;
			list = list->next;
		}
	}
}
/* handle_enemies
 * Arguments:
 * 	list		-	the list of enemies
 *	step		-	the game-clock
 *	ship_x		-	the location for the player for shooting
 *	ship_y		-	the location for the player for shooting
 *	slowed		-	whether or not we are in slow-motion
 *	count_cycles-	fractional part of the game-clock	
 * This handles all the things for the bad guys: moving, drawing, processing, shooting, etc.
 */
int handle_enemies(Enemy* list, int step, int ship_x, int ship_y, int slowed, int count_cycles)
{
	Enemy* start = list;
	Enemy* current;
	int hit_player = 0, i, r;
	double d;
	double deltat = slowed?SLOWED_DOWN:NORMAL_TIME;
	// Skip the head of the list, he's not a bad guy
	list = list->next;
	while ((list!=NULL))
	{
		current = list;
		list = list->next;
		current->y += SCROLL_SPEED*deltat;
		if ((current->y)>0)
		{
			current->x += (current->dx)*deltat;
			current->y += (current->dy)*deltat;
			switch (current->type)
			{
				default: break;
				case WALL:
					gfx_rectangle(current->x-current->r/2,current->y-current->r/2,current->r,current->r);
					if (current->y<WINDOW_HEIGHT/2)
					{
						current->dy=-SCROLL_SPEED+1;
					}
				break;
				case JERKOSAURUS_REX:
					current->dy = (WINDOW_HEIGHT-current->y-24)/30 - SCROLL_SPEED;
					gfx_circle(current->x,current->y,current->r);
					d = angle_to(current->x,current->y,ship_x,ship_y);
					if (!((step+(current->birthday))%50)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  8*cos(d), 8*sin(d), step);
					}
				break;
				case SPINNER:
					gfx_circle(current->x,current->y,current->r);
					if (current->y<WINDOW_HEIGHT/2)
					{
						current->dy=-SCROLL_SPEED+1;
					}
					else
					{
						current->dy = 0;
					}
					if (count_cycles==0)
					{
						enemy_shoot(current, current->x, current->y,  5*cos(((int)step)/M_PI), 5*sin(((int)step)/M_PI), step);
					}
				break;
				case TURRET:
					gfx_circle(current->x,current->y,current->r);
					d = angle_to(current->x,current->y,ship_x,ship_y);
					if (current->y<WINDOW_HEIGHT/2)
					{
						current->dy=-SCROLL_SPEED+1;
					}
					if (!((step+(current->birthday))%40)&&(count_cycles==0))
					{
						int i;
						for (i = -2; i <= 2; i++)
						{
							enemy_shoot(current, current->x, current->y,  2*cos(d+i/10.0), 2*sin(d+i/10.0), step);
						}
					}
				break;
				case STRAIGHTIZER:
					gfx_circle(current->x,current->y,current->r);
					d = angle_to(current->x,current->y,ship_x,ship_y);
					if (current->y<WINDOW_HEIGHT/2)
					{
						current->dy=-SCROLL_SPEED+1;
					}
					else
					{
						current->dy = 0;
					}
					if (!((step+(current->birthday))%10)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  5*cos(d), 5*sin(d), step);
					}
					if (!((step+(current->birthday))%15)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  5*cos(d+.1), 5*sin(d+.1), step);
						enemy_shoot(current, current->x, current->y,  5*cos(d-.1), 5*sin(d-.1), step);
					}
				break;
				case DIAGONALIZER:
					gfx_circle(current->x,current->y,current->r);
					d = angle_to(current->x,current->y,ship_x,ship_y);
					if (current->y<WINDOW_HEIGHT/2)
					{
						current->dy=-SCROLL_SPEED+1;
					}
					else
					{
						current->dy = sin(d)-SCROLL_SPEED;
						current->dx = cos(d);
					}
					if (!((step+(current->birthday))%10)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  5*cos(d+(M_PI/3)), 5*sin(d+(M_PI/3)), step);
						enemy_shoot(current, current->x, current->y,  5*cos(d-(M_PI/3)), 5*sin(d-(M_PI/3)), step);
					}
				break;
				case GOOD_SHIP:
					current->dy = (WINDOW_HEIGHT/3-current->y)/30 - SCROLL_SPEED;
					gfx_circle(current->x,current->y,current->r);
					d = angle_to(current->x,current->y,ship_x,ship_y);
					if (!((step+(current->birthday))%20)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  8*cos(d), 8*sin(d), step);
					}
				break;
				case BETTER_SHIP:
					gfx_circle(current->x,current->y,current->r);
					if (!((step+(current->birthday))%20)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  0, 14, step);
					}
					current->dx = 5*cos(step/(9*M_PI));
				break;
				case WEAK_SHIP:
					gfx_circle(current->x,current->y,current->r);
					if (!((step+(current->birthday))%20)&&(count_cycles==0))
					{
						enemy_shoot(current, current->x, current->y,  4, 9, step);
						enemy_shoot(current, current->x, current->y, -4, 9, step);
					}
				break;
				case BULLET:
					gfx_fill_circle(current->x,current->y,3);
					if ((!slowed)&&(--((current->hp)))<=0)
					{
						explode(current);
					}
				break;
				case EXPLOSION:
					r = (current->r+(rand()%(current->r)))*2;
					d = (rand()%180)/M_PI;
					gfx_circle(current->x,current->y,r*3/4);
					for (i = 0; i < r; i++)
					{
						gfx_line(current->x+r*cos(d),current->y+r*sin(d),current->x-r*cos(d),current->y-r*sin(d));
					}
					gfx_color(255,255,255);
					if ((--((current->hp)))<=0)
					{
						remove_enemy(start,current);
					}
				break;
			}
			if (current!=NULL)
			{
				if (current->type!=EXPLOSION && collision_square(ship_x,ship_y,current->x,current->y,SHIP_SIZE))
				{
					hit_player = 1;
					if ((--((current->hp)))<=0)
					{
						explode(current);
					}
				}
				else
				{
					if (current->y>WINDOW_HEIGHT)
					{
						remove_enemy(start,current);
					}
				}
			}
		}
		else
		{
			if (current->type==BULLET)
			{
				remove_enemy(start,current);
			}
		}
	}
	return hit_player;
}
/* handle_bullets
 * Arguments:
 * 	list	-	the list of bullets
 *	enemies	-	the list of enemies
 * Moves, draws, and collision-checks the bullets.
 */
int handle_bullets(Bullet* list, Enemy* enemies)
{
	int count = 0;
	Bullet* prev = list;
	Bullet* current;
	while ((list!=NULL))
	{
		current = list;
		list=list->next;
		(current->x) += (current->dx);
		(current->y) += (current->dy);
		gfx_color(0,255,0);
		gfx_line((current->x),(current->y),(current->x)+(current->dx),(current->y)+(current->dy));
		gfx_color(255,255,255);
		if (bullet_out(current))
		{
			remove_bullet(prev,current);
			current = prev;
		}
		else
		{
			if (check_enemy_collision(enemies,current))
			{
				remove_bullet(prev,current);
				current = prev;
				count++;
			}
		}
		prev = current;
	}
	return count;
}
/* bullet_out
 * Arguments:
 * 	b	-	the bullet to check
 * Returns:
 *	1 if the bullet is outta bounds, 0 otherwise
 * Just a check to see if your bullet has left the screen.
 */
int bullet_out(Bullet* b)
{
	return (b->x<0)||(b->x>WINDOW_WIDTH)||(b->y<0)||(b->y>WINDOW_HEIGHT);
}
/* remove_bullet
 * Arguments:
 * 	list	-	the list of bullets
 *	toremove-	the bullet to be deleted
 * Just a function to handle the remove operation on the bullet linked list.
 */
void remove_bullet(Bullet* list, Bullet* toremove)
{
	if (list!=NULL && toremove!=NULL && list!=toremove)
	{
		while (list->next!=toremove)
		{
			list=list->next;
		}
		list->next = toremove->next;
		toremove->next = NULL;
		free(toremove);
	}
}
/* insert_bullet
 * Arguments:
 * 	list	-	the list of bullets
 *	x		-	the x position for the new bullet
 *	y		-	the y position for the new bullet
 *	dx		-	the x velocity for the new bullet
 *	dy		-	the y velocity for the new bullet
 * Returns:
 *	A copy of the bullet added.
 * Creates a new bullet and adds it to the linked list.
 */
Bullet insert_bullet(Bullet* list, double x, double y, double dx, double dy)
{
	if (list==NULL)
	{
		list = (Bullet*)malloc(sizeof(Bullet));
	}
	else
	{
		while (list->next!=NULL)
		{
			list=list->next;
		}
		list->next = (Bullet*)malloc(sizeof(Bullet));
		list=list->next;
	}
	list->next = NULL;
	list->x = x;
	list->y = y;
	list->dx = dx;
	list->dy = dy;
	return *list;
}
/* remove_enemy
 * Arguments:
 * 	list	-	the list of enemies
 *	toremove-	the enemy to be deleted
 * Just a function to handle the remove operation on the enemy linked list.
 */
void remove_enemy(Enemy* list, Enemy* toremove)
{
	if (list!=NULL && toremove!=NULL && list!=toremove)
	{
		while (list->next!=toremove)
		{
			list=list->next;
		}
		list->next = toremove->next;
		toremove->next = NULL;
		free(toremove);
	}
}
/* explode
 * Arguments:
 * 	enemy	-	the enemy who exploded
 * Changes an enemy type to explosion and makes it stay still.
 * For enemies who die/bullets.
 */
void explode(Enemy *e)
{
	e->type = EXPLOSION;
	e->hp = EXPLOSION_DURATION;
	e->dx = 0;
	e->dy = -SCROLL_SPEED;
}
/* enemy_shoot
 * Arguments:
 * 	list	-	the list of enemies
 *	x		-	the x position for the new enemy bullet
 *	y		-	the y position for the new enemy bullet
 *	dx		-	the x velocity for the new enemy bullet
 *	dy		-	the y velocity for the new enemy bullet
 *	time	-	the game clock for the bullet's birthday
 * Returns:
 *	A copy of the enemy bullet added.
 * Creates a new enemy bullet and adds it to the linked list.
 */
Enemy enemy_shoot(Enemy* list, double x, double y, double dx, double dy, unsigned int time)
{
	if (list==NULL)
	{
		list = (Enemy*)malloc(sizeof(Enemy));
	}
	else
	{
		while (list->next!=NULL)
		{
			list=list->next;
		}
		list->next = (Enemy*)malloc(sizeof(Enemy));
		list=list->next;
	}
	list->next = NULL;
	list->x = x;
	list->y = y;
	list->dx = dx;
	list->dy = dy-SCROLL_SPEED;
	list->hp = BULLET_HP;
	list->r = 4;
	list->birthday = time;
	list->type = BULLET;
	return *list;
}
/* insert_enemy
 * Arguments:
 * 	list	-	the list of enemies
 *	x		-	the x position for the new enemy bullet
 *	y		-	the y position for the new enemy bullet
 *	hp		-	the health of the added enemy
 *	type	-	the enemy class
 *	r		-	the radius of the enemy
 *	time	-	the game clock for the enemie's birthday
 * Returns:
 *	A copy of the enemy added.
 * Creates a new enemy and adds it to the linked list.
 */
Enemy insert_enemy(Enemy* list, double x, double y, int hp, EnemyClass type, int r, unsigned time)
{
	if (list==NULL)
	{
		list = (Enemy*)malloc(sizeof(Enemy));
	}
	else
	{
		while (list->next!=NULL)
		{
			list=list->next;
		}
		list->next = (Enemy*)malloc(sizeof(Enemy));
		list=list->next;
	}
	list->next = NULL;
	list->x = x;
	list->y = y;
	list->dx = 0;
	list->dy = 0;
	list->r = r;
	list->hp = hp;
	list->birthday = time;
	list->type = type;
	return *list;
}
/* check_enemy_collision
 * Arguments:
 * 	list	-	the list of enemies
 *	bullet	-	the bullet to test for
 * Returns:
 *	1 if the bullet hit an enemy, 0 otherwise
 * Checks a bullet for collision with any enemy.
 */
int check_enemy_collision(Enemy* list, Bullet* bullet)
{
	while ((list!=NULL))
	{
		if (list->type!=EXPLOSION && list->type!=BULLET)
		{
			if (collision_square((list->x),(list->y),(bullet->x),(bullet->y),list->r))
			{
				if ((--((list->hp)))<=0)
				{
					explode(list);
				}
				return 1;
			}
		}
		list = list->next;
	}
	return 0;
}
/* increase_score
 * Arguments:
 * 	score	-	a pointer to the score variable
 *	inc		-	how much to change the score by
 *	text	-	the text for displaying the score to modify
 * Returns:
 *	the new score (value)
 * Increases and updates the score and score text.
 */
int increase_score(int* score,int inc,char text[])
{
	*score += inc;
	strcpy(text,SCORE_PREFIX);
	strcat(text,num2str(*score));
	return *score;
}
/* draw_hud
 * Arguments:
 * 	score_text	-	the score text to display
 *	lives		-	the number of lives to display for the player
 *	energy		-	the fullness of the player's energy meter
 * Handles displaying the information the player should see
 * during the game, like score and lives.
 */
void draw_hud(char score_text[], unsigned char lives, int energy)
{
	johngfx_text_font(12,12,score_text,SCORE_FONT);
	unsigned char i;
	for (i = 1; i <= lives; i++)
	{
		gfx_circle(WINDOW_WIDTH-12*i,12,4);
	}
	if (energy>0)
	{
		gfx_rectangle(200,12,200*energy/MAX_ENERGY,12);
	}
	else
	{
		gfx_color(255,0,0);
		gfx_rectangle(200,12,-200*energy/ENERGY_PENALTY,12);
		gfx_color(255,255,255);
	}
}
