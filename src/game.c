#include "platform.h"
#include "resources/gamedata.h"

#include "game.h"

CColor clearColor = { 0, 0, 128 };

gameStateUpdate* gsUpdate;
gameStateDraw* gsDraw;

CEnemyClass enemyClasses[3];

CWorld world;

int randRange(int min, int max)
{
	return ((double)sysRand() / RAND_MAX) * (max - min) + min;
}

bool aabbTest(int x1, int y1, int w1, int h1,
	int x2, int y2, int w2, int h2)
{
	return (x1 < x2 + w2) && (x2 < x1 + w1) && (y1 < y2 + h2) && (y2 < y1 + h1);
}

void setCurrentState(gameStateTransition transition, gameStateUpdate update, gameStateDraw draw)
{
	transition();

	gsUpdate = update;
	gsDraw = draw;
}

void preInitAssets()
{
	initSpriteData();

	// Asteroid
	enemyClasses[0].sprite = &sprEnemy1;
	enemyClasses[0].flags = ENEMY_FLAG_NONE;
	enemyClasses[0].maxHealth = 45;
	enemyClasses[0].contactDamage = 15;
	enemyClasses[0].speed = 2;

	// Regular unit
	enemyClasses[1].sprite = 0;
	enemyClasses[1].flags = ENEMY_FLAG_CAN_SHOOT;
	enemyClasses[1].contactDamage = 20;
	enemyClasses[1].projectileDamage = 20;

	// ZigZag shooter
	enemyClasses[2].sprite = 0;
	enemyClasses[2].flags = ENEMY_FLAG_CAN_SHOOT | ENEMY_FLAG_ZIG_ZAG_MOVEMENT;
	enemyClasses[2].contactDamage = 20;
	enemyClasses[2].projectileDamage = 10;

}

void gameStart()
{
	preInitAssets();

	setCurrentState(&arenaTransition, arenaUpdate, arenaDraw);
}

void gameUpdate()
{
	gsUpdate();
}

void gameDraw()
{
	gClearScreen(&clearColor);
	gDrawBitmap(&sprBackground, 0, 0);

	gsDraw();
}

/* Menu */

void menuTransition()
{

}

void menuUpdate()
{

}

void menuDraw()
{

}

/* Game */

int clamp(int a, int min, int max)
{
	return a < min ? min : (a > max ? max : a);
}

CEnemy* spawnEnemy(CEnemyClass* _class)
{
	int i;

	for (i = 0; i < sizeof(world.enemyPool) / sizeof(CEnemy*); i++)
	{
		CEnemy* enemy;

		if (world.enemyPool[i])
			continue;

		enemy = (CEnemy*)sysAlloc(sizeof(CEnemy));
		memset(enemy, 0, sizeof(CEnemy));
		enemy->_class = _class;
		enemy->health = _class->maxHealth;
		enemy->x = randRange(0, gGetScreenWidth() - _class->sprite->width);
		enemy->y = randRange(-_class->sprite->height * 4, -_class->sprite->height);

		
		return world.enemyPool[i] = enemy;
	}

	return 0;
}

void spawnProjectile(int x, int y, int direction, int speed, int damage)
{
	int i;

	for (i = 0; i < sizeof(world.projectilePool) / sizeof(CProjectile*); i++)
	{
		CProjectile* proj;

		if (world.projectilePool[i])
			continue;

		proj = (CProjectile*)sysAlloc(sizeof(CProjectile));
		proj->x = x;
		proj->y = y;
		proj->dir = direction;
		proj->speed = speed;
		proj->damage = damage;

		world.projectilePool[i] = proj;
		return;
	}
}

void arenaTransition()
{
	// Setup world
	memset(&world, 0, sizeof(world));

	world.player.health = 3;
	world.player.speed = 3;

	world.player.x = gGetScreenWidth() / 2 - (sprPlayer.width / 2);
	world.player.y = gGetScreenHeight() + (sprPlayer.height * 3);

	world.enemyPool[0] = spawnEnemy(&enemyClasses[0]);
}

// If returns true, then enemy should be destroyed
bool enemyThink(CEnemy* enemy)
{
	enemy->y += enemy->_class->speed;

	if (enemy->y > gGetScreenHeight() || enemy->health <= 0)
		return true;

	return false;
}

void arenaUpdate()
{
	// Player update
	int keys = inGetKeyState();
	int horizInput = 0;

	int i;
	int j;

	// Enemy update
	for (i = 0; i < sizeof(world.enemyPool) / sizeof(CEnemy*); i++)
	{
		if (world.enemyPool[i])
		{
			if (enemyThink(world.enemyPool[i]))
			{
				sysFree(world.enemyPool[i]);
				world.enemyPool[i] = 0;
			}
		}
	}

	// Projectile update
	for (i = 0; i < sizeof(world.projectilePool) / sizeof(CProjectile*); i++)
	{
		if (world.projectilePool[i])
		{
			world.projectilePool[i]->y += world.projectilePool[i]->dir * world.projectilePool[i]->speed;

			for (j = 0; j < sizeof(world.enemyPool) / sizeof(CEnemy*); j++)
			{
				if (world.enemyPool[j])
				{
					if (aabbTest(world.projectilePool[i]->x, world.projectilePool[i]->y, sprLaser.width, sprLaser.height, 
						world.enemyPool[j]->x, world.enemyPool[j]->y, world.enemyPool[j]->_class->sprite->width, world.enemyPool[j]->_class->sprite->height))
					{
						world.enemyPool[j]->health -= world.projectilePool[i]->damage;

						sysFree(world.projectilePool[i]);
						world.projectilePool[i] = 0;
						break;
					}
				}
			}
		}
	}

	world.nextSpawn--;

	if (world.nextSpawn < 0)
	{
		// randRange(0, 3)
		CEnemy* enemy = spawnEnemy(&enemyClasses[0]);
		
		world.nextSpawn = randRange(40, 70);
	}

	if (keys & KEY_LEFT)
		horizInput = -1;

	if (keys & KEY_RIGHT)
		horizInput = 1;

	if(world.player.y > gGetScreenHeight() - sprPlayer.height - 16)
		world.player.y -= world.player.speed;

	world.player.x += horizInput * world.player.speed;
	world.player.x = clamp(world.player.x, 0, gGetScreenWidth() - sprPlayer.width);

	if (keys & KEY_OK && world.player.nextAttack < 0)
	{
		spawnProjectile(world.player.x + (sprPlayer.width / 2), world.player.y, -1, 15, 35);
		world.player.nextAttack = 15;
	}

	world.player.nextAttack--;
}

void arenaDraw()
{
	int i;

	for (i = 0; i < sizeof(world.enemyPool) / sizeof(CEnemy*); i++)
	{
		if (world.enemyPool[i])
		{
			gDrawBitmap(world.enemyPool[i]->_class->sprite, world.enemyPool[i]->x, world.enemyPool[i]->y);
		}
	}

	for (i = 0; i < sizeof(world.projectilePool) / sizeof(CProjectile*); i++)
	{
		if (world.projectilePool[i])
		{
			gDrawBitmap(&sprLaser, world.projectilePool[i]->x, world.projectilePool[i]->y);
		}
	}

	gDrawBitmap(&sprPlayer, world.player.x, world.player.y);
}
