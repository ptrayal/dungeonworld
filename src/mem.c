/***************************************************************************
 *  File: mem.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/


#if defined(Macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 * Globals
 */
extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          int                     top_room;
extern		int			top_mprog_index;

HELP_DATA		*	help_free;

HELP_DATA		*	help_last;

void	free_extra_descr	args( ( EXTRA_DESCR_DATA *pExtra ) );
void	free_affect		args( ( AFFECT_DATA *af ) );
void	free_mprog              args ( ( MPROG_LIST *mp ) );


RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;

    ALLOC_DATA(pReset, RESET_DATA, 1);

    pReset->next        =   NULL;
    pReset->command     =   'X';
    pReset->arg1        =   0;
    pReset->arg2        =   0;
    pReset->arg3        =   0;

    return pReset;
}



void free_reset_data( RESET_DATA *pReset )
{
	PURGE_DATA(pReset);
    return;
}



AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;

    ALLOC_DATA(pArea, AREA_DATA, 1);

    pArea->name = str_dup( "New Area");
    pArea->builders = str_dup("None");
    pArea->file_name    =   str_dup( (char *)Format("area%d.are", pArea->vnum) );
    
    pArea->next             =   NULL;
    /*    pArea->recall           =   ROOM_VNUM_TEMPLE;      ROM OLC */
    pArea->area_flags       =   AREA_ADDED;
    pArea->security         =   1;
    pArea->min_vnum            =   0;
    pArea->max_vnum            =   0;
    pArea->age              =   0;
    pArea->nplayer          =   0;
    pArea->empty            =   TRUE;              /* ROM patch */
    pArea->vnum             =   top_area-1;

    return pArea;
}



void free_area( AREA_DATA *pArea )
{
    Escape(pArea);

    PURGE_DATA( pArea->name );
    PURGE_DATA( pArea->file_name );
    PURGE_DATA( pArea->builders );
    PURGE_DATA( pArea->credits );

    PURGE_DATA(pArea);
    return;
}



EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

    ALLOC_DATA(pExit, EXIT_DATA, 1);

    pExit->u1.to_room   =   NULL;                  /* ROM OLC */
    pExit->next         =   NULL;
/*  pExit->vnum         =   0;                        ROM OLC */
    pExit->exit_info    =   0;
    pExit->key          =   0;
    pExit->keyword      =   NULL;
    pExit->description  =   NULL;
    pExit->rs_flags     =   0;

    return pExit;
}



void free_exit( EXIT_DATA *pExit )
{
    Escape(pExit);

    PURGE_DATA( pExit->keyword );
    PURGE_DATA( pExit->description );

    PURGE_DATA( pExit );
    return;
}


ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door = 0;

    ALLOC_DATA(pRoom, ROOM_INDEX_DATA, 1);

    pRoom->next             =   NULL;
    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;
    pRoom->extra_descr      =   NULL;
    pRoom->area             =   NULL;

    for ( door=0; door < MAX_DIR; door++ )
        pRoom->exit[door]   =   NULL;

    pRoom->name             =   str_dup( "Unnamed Room");
    pRoom->description      =   str_dup( "None");
    pRoom->owner	    =	NULL;
    pRoom->vnum             =   0;
    pRoom->room_flags       =   0;
    pRoom->light            =   0;
    pRoom->sector_type      =   0;
    pRoom->clan		    =	0;
    pRoom->heal_rate	    =   100;
    pRoom->mana_rate	    =   100;

    return pRoom;
}



void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int door = 0;
    EXTRA_DESCR_DATA *pExtra, *pExtra_next;
    RESET_DATA *pReset, *pReset_next;

    PURGE_DATA( pRoom->name );
    PURGE_DATA( pRoom->description );
    PURGE_DATA( pRoom->owner );

    for ( door = 0; door < MAX_DIR; door++ )
    {
        if ( pRoom->exit[door] )
            free_exit( pRoom->exit[door] );
    }

    for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra_next )
    {
    	pExtra_next = pExtra->next;
        free_extra_descr( pExtra );
    }

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset_next)
    {
    	pReset_next = pReset->next;
        free_reset_data( pReset );
    }

	PURGE_DATA(pRoom);
    return;
}

extern AFFECT_DATA *affect_free;


SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy = 0;

    ALLOC_DATA(pShop, SHOP_DATA, 1);

    pShop->next         =   NULL;
    pShop->keeper       =   0;

    for ( buy=0; buy<MAX_TRADE; buy++ )
        pShop->buy_type[buy]    =   0;

    pShop->profit_buy   =   100;
    pShop->profit_sell  =   100;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;

    return pShop;
}



void free_shop( SHOP_DATA *pShop )
{
    Escape(pShop);
    // UNLINK_SINGLE(pShop, next, SHOP_DATA, shop_list);

    PURGE_DATA(pShop);
    return;
}



OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value = 0;

    ALLOC_DATA(pObj, OBJ_INDEX_DATA, 1);

    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->name          =   str_dup( "(no name)" );
    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    pObj->extra_flags   =   0;
    pObj->wear_flags    =   0;
    pObj->count         =   0;
    pObj->weight        =   0;
    pObj->cost          =   0;
    pObj->material      =   str_dup( "unknown" );      /* ROM */
    pObj->condition     =   100;                        /* ROM */
    for ( value = 0; value < 5; value++ )               /* 5 - ROM */
        pObj->value[value]  =   0;

    pObj->new_format    = TRUE; /* ROM */

    return pObj;
}



void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra, *pExtra_next;
    AFFECT_DATA *pAf, *pAf_next;

    PURGE_DATA( pObj->name );
    PURGE_DATA( pObj->short_descr );
    PURGE_DATA( pObj->description );

    for ( pAf = pObj->affected; pAf; pAf = pAf_next )
    {
    	pAf_next = pAf->next;
        free_affect( pAf );
    }

    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra_next )
    {
    	pExtra_next = pExtra->next;
        free_extra_descr( pExtra );
    }
    
    
    PURGE_DATA(pObj);
    return;
}



MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;

    ALLOC_DATA(pMob, MOB_INDEX_DATA, 1);

    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->pShop         =   NULL;
    pMob->area          =   NULL;
    pMob->player_name   =   str_dup( "(no name)" );
    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)\n\r" );
    pMob->description   =   str_dup( "(no description)");
    pMob->vnum          =   0;
    pMob->count         =   0;
    pMob->killed        =   0;
    pMob->sex           =   0;
    pMob->level         =   0;
    pMob->act           =   ACT_IS_NPC;
    pMob->affected_by   =   0;
    pMob->alignment     =   0;
    pMob->hitroll	=   0;
    pMob->race          =   race_lookup( "human" ); /* - Hugin */
    pMob->form          =   0;           /* ROM patch -- Hugin */
    pMob->parts         =   0;           /* ROM patch -- Hugin */
    pMob->imm_flags     =   0;           /* ROM patch -- Hugin */
    pMob->res_flags     =   0;           /* ROM patch -- Hugin */
    pMob->vuln_flags    =   0;           /* ROM patch -- Hugin */
    pMob->material      =   str_dup("unknown"); /* -- Hugin */
    pMob->off_flags     =   0;           /* ROM patch -- Hugin */
    pMob->size          =   SIZE_MEDIUM; /* ROM patch -- Hugin */
    pMob->ac[AC_PIERCE]	=   0;           /* ROM patch -- Hugin */
    pMob->ac[AC_BASH]	=   0;           /* ROM patch -- Hugin */
    pMob->ac[AC_SLASH]	=   0;           /* ROM patch -- Hugin */
    pMob->ac[AC_EXOTIC]	=   0;           /* ROM patch -- Hugin */
    pMob->hit[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
    pMob->hit[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
    pMob->hit[DICE_BONUS]	=   0;   /* ROM patch -- Hugin */
    pMob->mana[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
    pMob->mana[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
    pMob->mana[DICE_BONUS]	=   0;   /* ROM patch -- Hugin */
    pMob->damage[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
    pMob->damage[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
    pMob->damage[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
    pMob->start_pos             =   POS_STANDING; /*  -- Hugin */
    pMob->default_pos           =   POS_STANDING; /*  -- Hugin */
    pMob->wealth                =   0;

    pMob->new_format            = TRUE;  /* ROM */

    return pMob;
}



void free_mob_index( MOB_INDEX_DATA *pMob )
{
    Escape(pMob);
    
    PURGE_DATA( pMob->player_name );
    PURGE_DATA( pMob->short_descr );
    PURGE_DATA( pMob->long_descr );
    PURGE_DATA( pMob->description );
    free_mprog( pMob->mprogs );

    free_shop( pMob->pShop );

	PURGE_DATA(pMob);
    return;
}

MPROG_CODE *new_mpcode(void)
{
     MPROG_CODE *NewCode;

     ALLOC_DATA(NewCode, MPROG_CODE, 1);

     NewCode->vnum    = 0;
     NewCode->code    = NULL;
     NewCode->next    = NULL;

     return NewCode;
}

void free_mpcode(MPROG_CODE *pMcode)
{
    PURGE_DATA(pMcode->code);
	PURGE_DATA(pMcode);
    return;
}
