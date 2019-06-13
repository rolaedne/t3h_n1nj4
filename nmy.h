/* 
 * File:   nmy.h
 * Author: id10t
 *
 * Created on September 6, 2009, 3:00 PM
 */

#ifndef _NMY_H
#define	_NMY_H

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************
*Roderick Newill
*nmy.h
*This holds the function that the enemies will
*use to move, kill, and live.
********************************************/

#define NMYDLY 15
#define BYSWORD 0
#define BYSTAR  1
#define BYLAVA  2
#define NMY_FRAMES 6
#define NMY_DEATHS 3

struct enemy
  {
  /******************************************
  *defines all parts of the enemies
  *can we ever really define
  *what or whom an enemy is
  *what qualities really make up an enemy
  *******************************************/
  SDL_Surface *enemies[6]; /*frames of animation*/
  SDL_Surface *deaths[3]; /*death frames*/
  SDL_Rect nmydest;
  int nmy_deathtype;
  int nmy_alive, enemy_gravity_compound;
  int nmytype; /*type of enemy*/
  int nmydly;/*a count var NMYDLY in nmy.c tells lenght of delay*/
  int nmyani;/*what animation to use*/
  int nmyanilen;/*how many animations*/
  int dir[10];/*see below*/
  int onscreen;/*if enemy is on screen*/
  int jmpon;/*are they jumping/*/
  int jmp;/*how high enemies jump*/
  int speed;/*how fast an enemy moves*/
  } nmy[NMY];

/*********************
*keeps track of last hit object in
*world. used mainly for ai to handle direction
*and jumping. 5 would be in that air everything
*else would be around enemy.
*789
*456
*123
*******************/

void killplayer(int i);
void nmy_physics(int i);
void nmy_spwn(int i);
void enemyanimation(int i);
void enemyai();



#ifdef	__cplusplus
}
#endif

#endif	/* _NMY_H */

