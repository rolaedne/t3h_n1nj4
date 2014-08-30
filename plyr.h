/* 
 * File:   plyr.h
 * Author: id10t
 *
 * Created on September 6, 2009, 3:01 PM
 */

#ifndef _PLYR_H
#define	_PLYR_H

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************
*Roderick Newill
*plr.h
*Header file for all of the ninja's functions
********************************************/

#define JUMPMAX -47
/*set up verlocity on jump*/
#define ATTLEN 20


typedef enum {
    FALSE = 0, TRUE = 1
} BOOLEAN;


SDL_Rect dest, spdest;
SDL_Surface *ninja;
SDL_Surface *blood1;
SDL_Surface *sweapon1_1, *sweapon1_2;
SDL_Rect ninja_src;
int gravity_compound;
int jump, attack, sattack, attacklen;
int score;

void jumping(int jump);
void physics();
void dead();
void killenemy();
void special();
void special_throw();
void winner();



#ifdef	__cplusplus
}
#endif

#endif	/* _PLYR_H */

