#ifndef GAME_H
#define GAME_H

#define ENEMY_SHOOTER 0
#define ENEMY_KAMIKAZE 1
#define ENEMY_ASTEROID 2

#define ENEMY_POOL_SIZE 8
#define PROJECTILE_POOL_SIZE 48

#define ENEMY_FLAG_NONE 0
#define ENEMY_FLAG_CAN_SHOOT 1
#define ENEMY_FLAG_ZIG_ZAG_MOVEMENT 2

typedef void(gameStateTransition)();
typedef void(gameStateUpdate)();
typedef void(gameStateDraw)();

typedef struct
{
	int health;
	int frags;
	int score;

	int speed;

	int nextAttack;
	int x, y;
} CPlayer;

typedef struct
{
	CBitmap* sprite;

	int speed;

	int maxHealth;
	int flags;
	int projectileDamage;
	int contactDamage;
} CEnemyClass;

typedef struct
{
	CEnemyClass* _class;

	int health;
	int nextAttack;
	int x, y;
} CEnemy;

typedef struct
{
	int dir;
	int speed;
	int damage;

	int x;
	int y;
} CProjectile;

typedef struct
{
	CPlayer player;

	int nextSpawn; // In ticks

	CEnemy* enemyPool[ENEMY_POOL_SIZE];
	CProjectile* projectilePool[PROJECTILE_POOL_SIZE];
} CWorld;

void menuTransition();
void menuUpdate();
void menuDraw();

void arenaTransition();
void arenaUpdate();
void arenaDraw();

#endif