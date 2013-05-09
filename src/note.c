/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
 
/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif
extern FILE *                  fpArea;
extern char                    strArea[MAX_INPUT_LENGTH];

/* local procedures */
void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time);
void parse_note(CHAR_DATA *ch, char *argument, int type);
bool hide_note(CHAR_DATA *ch, NOTE_DATA *pnote);
bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote );

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;

void clear_notes(void) {
	NOTE_DATA *curr, *next;

	log_string("Cleaning: note_list");
	for(curr = note_list; curr; curr = next) {
		next = curr->next;
		free_note(curr);
	}
	note_list = NULL;

	log_string("Cleaning: idea_list");
	for(curr = idea_list; curr; curr = next) {
		next = curr->next;
		free_note(curr);
	}
	idea_list = NULL;

	log_string("Cleaning: penalty_list");
	for(curr = penalty_list; curr; curr = next) {
		next = curr->next;
		free_note(curr);
	}
	penalty_list = NULL;

	log_string("Cleaning: news_list");
	for(curr = news_list; curr; curr = next) {
		next = curr->next;
		free_note(curr);
	}
	news_list = NULL;

	log_string("Cleaning: changes_list");
	for(curr = changes_list; curr; curr = next) {
		next = curr->next;
		free_note(curr);
	}
	changes_list = NULL;

}

int count_spool(CHAR_DATA *ch, NOTE_DATA *spool)
{
	int count = 0;
	NOTE_DATA *pnote;

	for (pnote = spool; pnote != NULL; pnote = pnote->next)
	if (!hide_note(ch,pnote))
		count++;

	return count;
}

bool hide_note_ignore_stamp ( CHAR_DATA *ch, NOTE_DATA *pnote )
{
		time_t last_read;
						
		if ( IS_NPC ( ch ) )  
		{ return true; }
						
		switch ( pnote->type ) {
				default:
						return true;
				case NOTE_NOTE:
						last_read = ch->pcdata->last_note;
						break;
				case NOTE_IDEA:
						last_read = ch->pcdata->last_idea;
						break;
				case NOTE_PENALTY:
						last_read = ch->pcdata->last_penalty;
						break;
				case NOTE_NEWS:
						last_read = ch->pcdata->last_news;
						break;
				case NOTE_CHANGES:
						last_read = ch->pcdata->last_changes;
						break;
		}
 
		if ( !str_cmp ( ch->name, pnote->sender ) )
		{ return true; }
		
		if ( !is_note_to ( ch, pnote ) )
		{ return true; }
		
		return false;   
}
				

int count_spool_ignore_stamp ( CHAR_DATA *ch, NOTE_DATA *spool )
{
		int count = 0;
		NOTE_DATA *pnote;

		for ( pnote = spool; pnote != NULL; pnote = pnote->next )
				if ( !hide_note_ignore_stamp ( ch, pnote ) )
				{ count++; }

		return count;
}


void do_unread(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC(ch))
	return; 

		send_to_char ( "\ay+\aW-------------------------------\ay+\n\r", ch );
		send_to_char ( Format ( "\aW| \aGNews              \ay%3d  \aW| \aY%3d  \aW|\n\r", count_spool_ignore_stamp ( ch, news_list ), count_spool ( ch, news_list ) ), ch );
		send_to_char ( Format ( "\aW| \aGChanges           \ay%3d  \aW| \aY%3d  \aW|\n\r", count_spool_ignore_stamp ( ch, changes_list ), count_spool ( ch, changes_list ) ), ch );
		send_to_char ( Format ( "\aW| \aGIdeas             \ay%3d  \aW| \aY%3d  \aW|\n\r", count_spool_ignore_stamp ( ch, idea_list ), count_spool ( ch, idea_list ) ), ch );
		send_to_char ( Format ( "\aW| \aGNotes             \ay%3d  \aW| \aY%3d  \aW|\n\r", count_spool_ignore_stamp ( ch, note_list ), count_spool ( ch, note_list ) ), ch );
		if ( IS_TRUSTED ( ch, ANGEL ) )
		{ send_to_char ( Format ( "\aW| \aGPenalties         \ay%3d  \aW| \aY%3d  \aW|\n\r", count_spool_ignore_stamp ( ch, penalty_list ), count_spool ( ch, penalty_list ) ), ch ); }
		send_to_char ( "\ay+\aW-------------------------------\ay+\an\n\r", ch );
}

void do_note(CHAR_DATA *ch,char *argument)
{
	parse_note(ch,argument,NOTE_NOTE);
}

void do_idea(CHAR_DATA *ch,char *argument)
{
	parse_note(ch,argument,NOTE_IDEA);
}

void do_penalty(CHAR_DATA *ch,char *argument)
{
	parse_note(ch,argument,NOTE_PENALTY);
}

void do_news(CHAR_DATA *ch,char *argument)
{
	parse_note(ch,argument,NOTE_NEWS);
}

void do_changes(CHAR_DATA *ch,char *argument)
{
	parse_note(ch,argument,NOTE_CHANGES);
}

void save_notes(int type)
{
	FILE *fp;
	char *name;
	NOTE_DATA *pnote;

	switch (type)
	{
	default:
		return;
	case NOTE_NOTE:
		name = NOTE_FILE;
		pnote = note_list;
		break;
	case NOTE_IDEA:
		name = IDEA_FILE;
		pnote = idea_list;
		break;
	case NOTE_PENALTY:
		name = PENALTY_FILE;
		pnote = penalty_list;
		break;
	case NOTE_NEWS:
		name = NEWS_FILE;
		pnote = news_list;
		break;
	case NOTE_CHANGES:
		name = CHANGES_FILE;
		pnote = changes_list;
		break;
	}

	closeReserve();
	if ( ( fp = fopen( name, "w" ) ) == NULL )
	{
	perror( name );
	}
	else
	{
	for ( ; pnote != NULL; pnote = pnote->next )
	{
		fprintf( fp, "Sender  %s~\n", pnote->sender);
		fprintf( fp, "Date    %s~\n", pnote->date);
		fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
		fprintf( fp, "To      %s~\n", pnote->to_list);
		fprintf( fp, "Subject %s~\n", pnote->subject);
		fprintf( fp, "Text\n%s~\n",   pnote->text);
	}
	fclose( fp );
	openReserve();
	return;
	}
}
void load_notes(void)
{
	load_thread(NOTE_FILE,&note_list, NOTE_NOTE, 14*24*60*60);
	load_thread(IDEA_FILE,&idea_list, NOTE_IDEA, 28*24*60*60);
	load_thread(PENALTY_FILE,&penalty_list, NOTE_PENALTY, 0);
	load_thread(NEWS_FILE,&news_list, NOTE_NEWS, 0);
	load_thread(CHANGES_FILE,&changes_list,NOTE_CHANGES, 0);
}

void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time)
{
	FILE *fp;
	NOTE_DATA *pnotelast;
 
	if ( ( fp = fopen( name, "r" ) ) == NULL )
	return;
	 
	pnotelast = NULL;
	for ( ; ; )
	{
	NOTE_DATA *pnote;
	char letter;
	 
	do
	{
		letter = getc( fp );
			if ( feof(fp) )
			{
				fclose( fp );
				return;
			}
		}
		while ( isspace(letter) );
		ungetc( letter, fp );
 
		ALLOC_DATA(pnote, NOTE_DATA, 1);
 
		if ( str_cmp( fread_word( fp ), "sender" ) )
			break;
		pnote->sender   = fread_string( fp );
 
		if ( str_cmp( fread_word( fp ), "date" ) )
			break;
		pnote->date     = fread_string( fp );
 
		if ( str_cmp( fread_word( fp ), "stamp" ) )
			break;
		pnote->date_stamp = fread_number(fp);
 
		if ( str_cmp( fread_word( fp ), "to" ) )
			break;
		pnote->to_list  = fread_string( fp );
 
		if ( str_cmp( fread_word( fp ), "subject" ) )
			break;
		pnote->subject  = fread_string( fp );
 
		if ( str_cmp( fread_word( fp ), "text" ) )
			break;
		pnote->text     = fread_string( fp );
 
		if (free_time && pnote->date_stamp < current_time - free_time)
		{
		free_note(pnote);
			continue;
		}

	pnote->type = type;
 
		if (*list == NULL)
			*list           = pnote;
		else
			pnotelast->next     = pnote;
 
		pnotelast       = pnote;
	}
 
	strcpy( strArea, NOTE_FILE );
	fpArea = fp;
	bug( "Load_notes: bad key word.", 0 );
	exit( 1 );
	return;
}

void append_note(NOTE_DATA *pnote)
{
	FILE *fp;
	char *name;
	NOTE_DATA **list;
	NOTE_DATA *last;

	switch(pnote->type)
	{
	default:
		return;
	case NOTE_NOTE:
		name = NOTE_FILE;
		list = &note_list;
		break;
	case NOTE_IDEA:
		name = IDEA_FILE;
		list = &idea_list;
		break;
	case NOTE_PENALTY:
		name = PENALTY_FILE;
		list = &penalty_list;
		break;
	case NOTE_NEWS:
		 name = NEWS_FILE;
		 list = &news_list;
		 break;
	case NOTE_CHANGES:
		 name = CHANGES_FILE;
		 list = &changes_list;
		 break;
	}

	if (*list == NULL)
	*list = pnote;
	else
	{
	for ( last = *list; last->next != NULL; last = last->next);
	last->next = pnote;
	}

	closeReserve();
	if ( ( fp = fopen(name, "a" ) ) == NULL )
	{
		perror(name);
	}
	else
	{
		fprintf( fp, "Sender  %s~\n", pnote->sender);
		fprintf( fp, "Date    %s~\n", pnote->date);
		fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
		fprintf( fp, "To      %s~\n", pnote->to_list);
		fprintf( fp, "Subject %s~\n", pnote->subject);
		fprintf( fp, "Text\n%s~\n", pnote->text);
		fclose( fp );
	}
	openReserve();
}

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
	if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

	if ( is_exact_name( "all", pnote->to_list ) )
	return TRUE;

	if ( IS_IMMORTAL(ch) && is_exact_name( "immortal", pnote->to_list ) )
	return TRUE;

	if (ch->clan && is_exact_name(clan_table[ch->clan].name,pnote->to_list))
	return TRUE;

	if (is_exact_name( ch->name, pnote->to_list ) )
	return TRUE;

	return FALSE;
}



void note_attach( CHAR_DATA *ch, int type )
{
	NOTE_DATA *pnote;

	if ( ch->pnote != NULL )
	return;

	pnote = new_note();

	pnote->next		= NULL;
	pnote->sender	= str_dup( ch->name );
	pnote->date		= NULL;
	pnote->to_list	= NULL;
	pnote->subject	= NULL;
	pnote->text		= NULL;
	pnote->type		= type;
	ch->pnote		= pnote;
	return;
}

void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool idelete)
{
	char to_new[MAX_INPUT_LENGTH];
	char to_one[MAX_INPUT_LENGTH];
	NOTE_DATA *prev;
	NOTE_DATA **list;
	char *to_list;

	if (!idelete)
	{
	/* make a new list */
		to_new[0]	= '\0';
		to_list	= pnote->to_list;
		while ( *to_list != '\0' )
		{
			to_list	= one_argument( to_list, to_one );
			if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
		{
			strcat( to_new, " " );
			strcat( to_new, to_one );
		}
		}
		/* Just a simple recipient removal? */
	   if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
	   {
	   PURGE_DATA( pnote->to_list );
	   pnote->to_list = str_dup( to_new + 1 );
	   return;
	   }
	}
	/* nuke the whole note */

	switch(pnote->type)
	{
	default:
		return;
	case NOTE_NOTE:
		list = &note_list;
		break;
	case NOTE_IDEA:
		list = &idea_list;
		break;
	case NOTE_PENALTY:
		list = &penalty_list;
		break;
	case NOTE_NEWS:
		list = &news_list;
		break;
	case NOTE_CHANGES:
		list = &changes_list;
		break;
	}

	/*
	 * Remove note from linked list.
	 */
	if ( pnote == *list )
	{
	*list = pnote->next;
	}
	else
	{
	for ( prev = *list; prev != NULL; prev = prev->next )
	{
		if ( prev->next == pnote )
		break;
	}

	if ( prev == NULL )
	{
		bug( "Note_remove: pnote not found.", 0 );
		return;
	}

	prev->next = pnote->next;
	}

	save_notes(pnote->type);
	free_note(pnote);
	return;
}

bool hide_note (CHAR_DATA *ch, NOTE_DATA *pnote)
{
	time_t last_read;

	if (IS_NPC(ch))
	return TRUE;

	switch (pnote->type)
	{
	default:
		return TRUE;
	case NOTE_NOTE:
		last_read = ch->pcdata->last_note;
		break;
	case NOTE_IDEA:
		last_read = ch->pcdata->last_idea;
		break;
	case NOTE_PENALTY:
		last_read = ch->pcdata->last_penalty;
		break;
	case NOTE_NEWS:
		last_read = ch->pcdata->last_news;
		break;
	case NOTE_CHANGES:
		last_read = ch->pcdata->last_changes;
		break;
	}
	
	if (pnote->date_stamp <= last_read)
	return TRUE;

	if (!str_cmp(ch->name,pnote->sender))
	return TRUE;

	if (!is_note_to(ch,pnote))
	return TRUE;

	return FALSE;
}

void update_read(CHAR_DATA *ch, NOTE_DATA *pnote)
{
	time_t stamp;

	if (IS_NPC(ch))
	return;

	stamp = pnote->date_stamp;

	switch (pnote->type)
	{
		default:
			return;
		case NOTE_NOTE:
		ch->pcdata->last_note = UMAX(ch->pcdata->last_note,stamp);
			break;
		case NOTE_IDEA:
		ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea,stamp);
			break;
		case NOTE_PENALTY:
		ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty,stamp);
			break;
		case NOTE_NEWS:
		ch->pcdata->last_news = UMAX(ch->pcdata->last_news,stamp);
			break;
		case NOTE_CHANGES:
		ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes,stamp);
			break;
	}
}

void parse_note( CHAR_DATA *ch, char *argument, int type )
{
	char arg[MAX_INPUT_LENGTH];
	NOTE_DATA *pnote;
	NOTE_DATA **list;
	char *list_name;
	int vnum = 0;
	int anum = 0;

	if ( IS_NPC(ch) )
		return;

	switch(type)
	{
		default:
		return;
		case NOTE_NOTE:
		list = &note_list;
		list_name = "notes";
		break;
		case NOTE_IDEA:
		list = &idea_list;
		list_name = "ideas";
		break;
		case NOTE_PENALTY:
		list = &penalty_list;
		list_name = "penalties";
		break;
		case NOTE_NEWS:
		list = &news_list;
		list_name = "news";
		break;
		case NOTE_CHANGES:
		list = &changes_list;
		list_name = "changes";
		break;
	}

	argument = one_argument( argument, arg );
	smash_tilde( argument );

	if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
	{
		bool fAll;

		if ( !str_cmp( argument, "all" ) )
		{
			fAll = TRUE;
			anum = 0;
		}

		else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
		/* read next unread note */
		{
			vnum = 0;
			for ( pnote = *list; pnote != NULL; pnote = pnote->next)
			{
				if (!hide_note(ch,pnote))
				{
					send_to_char( Format("[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
						vnum,
						pnote->sender,
						pnote->subject,
						pnote->date,
						pnote->to_list), ch );
					page_to_char( pnote->text, ch );
					update_read(ch,pnote);
					return;
				}
				else if (is_note_to(ch,pnote))
					vnum++;
			}
			send_to_char( Format("You have no unread %s.\n\r",list_name),ch);
			return;
		}

		else if ( is_number( argument ) )
		{
			fAll = FALSE;
			anum = atoi( argument );
		}
		else
		{
			send_to_char( "Read which number?\n\r", ch );
			return;
		}

		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next )
		{
			if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
			{
				send_to_char( Format("[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
					vnum - 1,
					pnote->sender,
					pnote->subject,
					pnote->date,
					pnote->to_list), ch );
				page_to_char( pnote->text, ch );
				update_read(ch,pnote);
				return;
			}
		}

		send_to_char( Format("There aren't that many %s.\n\r",list_name),ch);
		return;
	}

	if ( !str_prefix( arg, "list" ) )
	{
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next )
		{
			if ( is_note_to( ch, pnote ) )
			{
				send_to_char( Format("[%3d%s] %s: %s\n\r",
					vnum, hide_note(ch,pnote) ? " " : "N", 
					pnote->sender, pnote->subject), ch );
				vnum++;
			}
		}
		if (!vnum)
		{
			switch(type)
			{
				case NOTE_NOTE:	
				send_to_char("There are no notes for you.\n\r",ch);
				break;
				case NOTE_IDEA:
				send_to_char("There are no ideas for you.\n\r",ch);
				break;
				case NOTE_PENALTY:
				send_to_char("There are no penalties for you.\n\r",ch);
				break;
				case NOTE_NEWS:
				send_to_char("There is no news for you.\n\r",ch);
				break;
				case NOTE_CHANGES:
				send_to_char("There are no changes for you.\n\r",ch);
				break;
			}
		}
		return;
	}

	if ( !str_prefix( arg, "remove" ) )
	{
		if ( !is_number( argument ) )
		{
			send_to_char( "Note remove which number?\n\r", ch );
			return;
		}

		anum = atoi( argument );
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next )
		{
			if ( is_note_to( ch, pnote ) && vnum++ == anum )
			{
				note_remove( ch, pnote, FALSE );
				send_to_char( "Ok.\n\r", ch );
				return;
			}
		}

		send_to_char( Format("There aren't that many %s.",list_name),ch);
		return;
	}

	if ( !str_prefix( arg, "delete" ) && get_trust(ch) >= MAX_LEVEL - 1)
	{
		if ( !is_number( argument ) )
		{
			send_to_char( "Note delete which number?\n\r", ch );
			return;
		}

		anum = atoi( argument );
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next )
		{
			if ( is_note_to( ch, pnote ) && vnum++ == anum )
			{
				note_remove( ch, pnote,TRUE );
				send_to_char( "Ok.\n\r", ch );
				return;
			}
		}

		send_to_char( Format("There aren't that many %s.",list_name),ch);
		return;
	}

	if (!str_prefix(arg,"catchup"))
	{
		switch(type)
		{
			case NOTE_NOTE:	
			ch->pcdata->last_note = current_time;
			break;
			case NOTE_IDEA:
			ch->pcdata->last_idea = current_time;
			break;
			case NOTE_PENALTY:
			ch->pcdata->last_penalty = current_time;
			break;
			case NOTE_NEWS:
			ch->pcdata->last_news = current_time;
			break;
			case NOTE_CHANGES:
			ch->pcdata->last_changes = current_time;
			break;
		}
		return;
	}

	/* below this point only certain people can edit notes */
	if ((type == NOTE_NEWS && !IS_TRUSTED(ch,ANGEL))
		||  (type == NOTE_CHANGES && !IS_TRUSTED(ch,CREATOR)))
	{
		send_to_char( Format("You aren't high enough level to write %s.",list_name),ch);
		return;
	}

	if ( !str_cmp ( arg, "new" ) || !str_cmp ( arg, "write" ) ) {
		note_attach ( ch, type );
		if ( ch->pnote->type != type ) {
			send_to_char ( "You already have a different note in progress.\n\r", ch );
			return;
		}
		ch->desc->connected = CON_NOTE_TO;
		send_to_char ( "Address this message to whom: (all, staff, <name>) ", ch );
		return;
	}

	send_to_char( "You can't do that.\n\r", ch );
	return;
}

