/***************************************************************************
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer, *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe. *
* *
* Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael *
* Chastain, Michael Quan, and Mitchell Tse. *
* *
* In order to use any part of this Merc Diku Mud, you must comply with *
* both the original Diku license in 'license.doc' as well the Merc *
* license in 'license.txt'. In particular, you may not remove either of *
* these copyright notices. *
* *
* Much time and thought has gone into this software and you are *
* benefitting. We hope that you share your changes too. What goes *
* around, comes around. *
***************************************************************************/
 
/***************************************************************************
* ROM 2.4 is copyright 1993-1995 Russ Taylor *
* ROM has been brought to you by the ROM consortium *
* Russ Taylor (rtaylor@pacinfo.com) *
* Gabrielle Taylor (gtaylor@pacinfo.com) *
* Brian Moore (rom@rom.efn.org) *
* By using this code, you have agreed to follow the terms of the *
* ROM license, in the file Rom24/doc/rom.license *
***************************************************************************/
 
/***************************************************************************
* ROT 1.4 is copyright 1996-1997 by Russ Walsh *
* By using this code, you have agreed to follow the terms of the *
* ROT license, in the file doc/rot.license *
***************************************************************************/
 
/***************************************************************************
* This code was put in by Lucas Clav/Rico Suave/Jed/Bobo the Big fat *
* clown(joke), ported from ROT 1.4 to ROM2.4B(supposedly)... Should *
* work... har har har!!! *
***************************************************************************/
 
#if defined(Macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "merc.h"
#include "recycle.h"
 
WIZ_DATA *wiz_list;
 
char * const wiz_titles [] =
{
	"Implementors",
	"Creators ",
	"Supremacies ",
	"Deities ",
	"Gods ",
	"Immortals ",
	"DemiGods ",
	"Knights ",
	"Squires "
};
 
/*
* Local functions.
*/
void change_wizlist args( (CHAR_DATA *ch, bool add, int level, char *argument));
 
 
void save_wizlist(void)
{
	WIZ_DATA *pwiz;
	FILE *fp;
	bool found = FALSE;

	closeReserve();
	if ( ( fp = fopen( WIZ_FILE, "w" ) ) == NULL )
	{
		perror( WIZ_FILE );
	}

	for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
	{
		found = TRUE;
		fprintf(fp,"%s %d\n",pwiz->name,pwiz->level);
	}

	fclose(fp);
	openReserve();
	if (!found)
		unlink(WIZ_FILE);
}
 
void load_wizlist(void)
{
	FILE *fp;
	WIZ_DATA *wiz_last;

	if ( ( fp = fopen( WIZ_FILE, "r" ) ) == NULL )
	{
		return;
	}

	wiz_last = NULL;
	for ( ; ; )
	{
		WIZ_DATA *pwiz;
		if ( feof(fp) )
		{
			fclose( fp );
			return;
		}

		pwiz = new_wiz();

		pwiz->name = str_dup(fread_word(fp));
		pwiz->level = fread_number(fp);
		fread_to_eol(fp);

		if (wiz_list == NULL)
			wiz_list = pwiz;
		else
			wiz_last->next = pwiz;
		wiz_last = pwiz;
	}
}
 
void do_wizlist(CHAR_DATA *ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MSL]={'\0'};
	char title[MSL]={'\0'};
	BUFFER *buffer;
	int level = 0;
	WIZ_DATA *pwiz;
	int lngth = 0;
	int amt = 0;
	bool found;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ((arg1[0] != '\0') && (ch->level == MAX_LEVEL))
	{
		if ( !str_prefix( arg1, "add" ) )
		{
			if ( !is_number( arg2 ) || ( arg3[0] == '\0' ) )
			{
				send_to_char( "Syntax: wizlist add <level> <name>\n\r", ch );
				return;
			}
			level = atoi(arg2);
			change_wizlist( ch, TRUE, level, arg3 );
			return;
		}
		if ( !str_prefix( arg1, "delete" ) )
		{
			if ( arg2[0] == '\0' )
			{
				send_to_char( "Syntax: wizlist delete <name>\n\r", ch );
				return;
			}
			change_wizlist( ch, FALSE, 0, arg2 );
			return;
		}
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( " wizlist delete <name>\n\r", ch );
		send_to_char( " wizlist add <level> <name>\n\r", ch );
		return;
	}

	if (wiz_list == NULL)
	{
		send_to_char("No immortals listed at this time.\n\r",ch);
		return;
	}
	buffer = new_buf();
	sprintf(title,"The Gods of Dungeon World");
	sprintf(buf,"___________________________________________________________________________\n\r");
	add_buf(buffer,buf);
	sprintf(buf," /\\_\\%70s\\_\\\n\r", " ");
	add_buf(buffer,buf);
	lngth = (70 - strlen(title))/2;
	for( ; lngth >= 0; lngth--)
	{
		strcat(title, " ");
	}
	sprintf(buf,"|/\\\\_\\\tW%70s\tn\\_\\\n\r", title);
	add_buf(buffer,buf);
	sprintf(buf,"\\_/_|_|%69s|_|\n\r", " ");
	add_buf(buffer,buf);
	for (level = IMPLEMENTOR; level > HERO; level--)
	{
		found = FALSE;
		amt = 0;
		for (pwiz = wiz_list;pwiz != NULL;pwiz = pwiz->next)
		{
			if (pwiz->level == level)
			{
				amt++;
				found = TRUE;
			}
		}
		if (!found)
		{
			if (level == HERO+1)
			{
				sprintf(buf," ___|_|%69s|_|\n\r", " ");
				add_buf(buffer,buf);
			}
			continue;
		}
		sprintf(buf," |_|%37s [%d]%30s|_|\n\r", wiz_titles[IMPLEMENTOR-level], level, " ");
		add_buf(buffer,buf);
		sprintf(buf," |_|\tY%25s******************\tn%29s|_|\n\r", " ", " ");
		add_buf(buffer,buf);
		lngth = 0;
		for (pwiz = wiz_list;pwiz != NULL;pwiz = pwiz->next)
		{
			if (pwiz->level == level)
			{
				if (lngth == 0)
				{
					if (amt > 2)
					{
						sprintf(buf, " |_|%12s%-23s ", pwiz->name, " ");
						add_buf(buffer, buf);
						lngth = 1;
					} 
					else if (amt > 1)
					{
						sprintf(buf, " |_|%21s%-23s ", pwiz->name, " ");
						add_buf(buffer, buf);
						lngth = 1;
					} 
					else
					{
						sprintf(buf, " |_|%30s%-42s|_|\n\r", pwiz->name, " ");
						add_buf(buffer, buf);
						lngth = 0;
					}
				} 
				else if (lngth == 1)
				{
					if (amt > 2)
					{
						sprintf(buf, "%-23s ", pwiz->name);
						add_buf(buffer, buf);
						lngth = 2;
					} 
					else
					{
						sprintf(buf, "%-30s|_|\n\r", pwiz->name);
						add_buf(buffer, buf);
						lngth = 0;
					}
				} 
				else
				{
					sprintf(buf, "%-21s|_|\n\r", pwiz->name);
					add_buf(buffer, buf);
					lngth = 0;
					amt -= 3;
				}
			}
		}
		if (level == HERO+1)
		{
			sprintf(buf," ___|_|%72s|_|\n\r", " ");
		} 
		else
		{
			sprintf(buf," |_|%72s|_|\n\r", " ");
		}
		add_buf(buffer,buf);
	}
	sprintf(buf,"/ \\ |_|%69s|_|\n\r", " ");
	add_buf(buffer,buf);
	sprintf(buf,"|\\//_/%70s/_/\n\r", " ");
	add_buf(buffer,buf);
	sprintf(buf," \\/_/______________________________________________________________________/_/\n\r");
	add_buf(buffer,buf);
	page_to_char( buf_string(buffer), ch );
	free_buf(buffer);
	return;
}
 
void update_wizlist(CHAR_DATA *ch, int level)
{
	WIZ_DATA *prev;
	WIZ_DATA *curr;

	if (IS_NPC(ch))
	{
		return;
	}
	prev = NULL;
	for ( curr = wiz_list; curr != NULL; prev = curr, curr = curr->next )
	{
		if ( !str_cmp( ch->name, curr->name ) )
		{
			if ( prev == NULL )
				wiz_list = wiz_list->next;
			else
				prev->next = curr->next;

			free_wiz(curr);
			save_wizlist();
		}
	}
	if (level <= HERO)
	{
		return;
	}
	curr = new_wiz();
	curr->name = str_dup(ch->name);
	curr->level = level;
	curr->next = wiz_list;
	wiz_list = curr;
	save_wizlist();
	return;
}
 
void change_wizlist(CHAR_DATA *ch, bool add, int level, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	WIZ_DATA *curr;

	one_argument( argument, arg );
	if (arg[0] == '\0')
	{
		send_to_char( "Syntax:\n\r", ch );
		if ( !add )
			send_to_char( " wizlist delete <name>\n\r", ch );
		else
			send_to_char( " wizlist add <level> <name>\n\r", ch );
		return;
	}
	if ( add )
	{
		if ( ( level <= HERO ) || ( level > MAX_LEVEL ) )
		{
			send_to_char( "Syntax:\n\r", ch );
			send_to_char( " wizlist add <level> <name>\n\r", ch );
			return;
		}
	}
	if ( !add )
	{
		WIZ_DATA *prev;
		prev = NULL;
		for ( curr = wiz_list; curr != NULL; prev = curr, curr = curr->next )
		{
			if ( !str_cmp( capitalize( arg ), curr->name ) )
			{
				if ( prev == NULL )
					wiz_list = wiz_list->next;
				else
					prev->next = curr->next;

				free_wiz(curr);
				save_wizlist();
			}
		}
	} else
	{
		curr = new_wiz();
		curr->name = str_dup( capitalize( arg ) );
		curr->level = level;
		curr->next = wiz_list;
		wiz_list = curr;
		save_wizlist();
	}
	return;
}


void clear_wizlist(void) {
	WIZ_DATA *wiz, *wiz_next;

	log_string("Cleaning: wiz_list");
	for(wiz = wiz_list; wiz; wiz = wiz_next) {
		wiz_next = wiz->next;
		UNLINK_SINGLE(wiz, next, WIZ_DATA, wiz_list);
		free_wiz(wiz);
	}
	wiz_list = NULL;
}

