#ifndef GAME_INCLUDE
#define GAME_INCLUDE
#define SHIP_SPEED 				8
#define ENEMY_SIZE				16
#define SHIP_SIZE				16
#define	SCROLL_SPEED			3
#define SCORE_PREFIX			"Score: "
#define REWIND_TIME				100
#define EXPLOSION_DURATION		50
#define BULLET_HP				200
#define SLOWED_DOWN				0.2
#define NORMAL_TIME				1.0
#define ENERGY_RECHARGE_RATE	15
#define ENERGY_DRAIN_RATE		3
#define MAX_ENERGY				(ENERGY_DRAIN_RATE*300)
#define ENERGY_PENALTY			(ENERGY_RECHARGE_RATE*500)
typedef struct Bullet_s
{
	double x, y, dx, dy;
	// The bullets are stored as a linked list
	struct Bullet_s* next;
} Bullet;
// enum for the types of enemies
typedef enum {
	EXPLOSION = -1,
	SENTINEL = 0,
	BULLET = 1,
	WEAK_SHIP = 2,
	BETTER_SHIP = 3,
	GOOD_SHIP = 4,
	DIAGONALIZER = 5,
	JERKOSAURUS_REX = 6,
	STRAIGHTIZER = 7,
	SPINNER = 8,
	WALL = 9,
	TURRET = 10
} EnemyClass;
typedef struct Enemy_s
{
	double x, y, dx, dy;
	int hp, r;
	unsigned int birthday;
	EnemyClass type;
	// The enemies are stored as linked list
	struct Enemy_s* next;
} Enemy;
int handle_game();
Bullet insert_bullet(Bullet* list, double x, double y, double dx, double dy);
int handle_bullets(Bullet* list, Enemy* enemies);
int bullet_out(Bullet* b);
void remove_bullet(Bullet* list, Bullet* toremove);
void draw_ship(int x, int y);
int increase_score(int* score,int inc,char text[]);
void draw_hud(char score_text[], unsigned char lives, int energy);
Enemy insert_enemy(Enemy* list, double x, double y, int hp, EnemyClass type, int r, unsigned time);
void remove_enemy(Enemy* list, Enemy* toremove);
int check_enemy_collision(Enemy* list, Bullet* bullet);
int handle_enemies(Enemy* list, int step, int ship_x, int ship_y, int slowed, int count_cycles);
Enemy enemy_shoot(Enemy* list, double x, double y, double dx, double dy, unsigned int time);
void handle_rewind(Enemy* list, int* rewind);
void explode(Enemy *e);
#endif
