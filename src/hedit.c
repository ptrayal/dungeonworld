/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			                   *
*	ROM has been brought to you by the ROM consortium		               *
*	    Russ Taylor (rtaylor@hypercube.org)				                   *
*	    Gabrielle Taylor (gtaylor@efn.org)				                   *
*	    Brian Moore (zump@rom.org)					                       *
*	By using this code, you have agreed to follow the terms of the	       *
*	ROM license, in the file Rom24/doc/rom.license			               *
***************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "lookup.h"
#include "recycle.h"

#define HEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_HELP(ch, help)	( help = (HELP_DATA *) ch->desc->pEdit )

extern HELP_AREA * had_list;

const struct olc_cmd_type hedit_table[] =
{
/*	{	command		function	}, */

	{	"keyword",	hedit_keyword	},
	{	"texto",	hedit_text	},
	{	"create",	hedit_new	},
	{	"nivel",	hedit_level	},
	{	"commands",	show_commands	},
	{	"delete",	hedit_delete	},
	{	"list",		hedit_list	},
	{	"show",		hedit_show	},
	{	"?",		show_help	},

	{	NULL,		0		}
};

HELP_AREA * get_help_area( HELP_DATA *help )
{
	HELP_AREA * temp;
	HELP_DATA * thelp;

	for ( temp = had_list; temp; temp = temp->next )
		for ( thelp = temp->first; thelp; thelp = thelp->next_area )
			if ( thelp == help )
				return temp;

	return NULL;
}

HEDIT(hedit_show)
{
	HELP_DATA * help;
	
	EDIT_HELP(ch, help);

	send_to_char( Format("Keyword : [%s]\n\r"
		"Level   : [%d]\n\r"
		"Text    :\n\r"
		"%s-END-\n\r",
		help->keyword,
		help->level,
		help->text), ch );

	return FALSE;
}

HEDIT(hedit_level)
{
	HELP_DATA *help;
	int lev = 0;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) || !is_number(argument) )
	{
		send_to_char( "Syntax : level [-1..MAX_LEVEL]\n\r", ch );
		return FALSE;
	}

	lev = atoi(argument);

	if ( lev < -1 || lev > MAX_LEVEL )
	{
		printf_to_char( ch, "HEdit : Level must be between -1 and %d, inclusive.\n\r", MAX_LEVEL );
		return FALSE;
	}

	help->level = lev;
	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_keyword)
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : keyword [keyword(s)]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA(help->keyword);
	help->keyword = str_dup(argument);

	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_new)
{
	char arg[MIL]={'\0'};
	char fullarg[MIL]={'\0'};
	HELP_AREA *had;
	HELP_DATA *help;
	extern HELP_DATA *help_last;

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : create [name]\n\r", ch );
		send_to_char( "         create [area] [name]\n\r", ch );
		return FALSE;
	}

	strcpy( fullarg, argument );
	argument = one_argument( argument, arg );

	if ( !(had = had_lookup(arg)) )
	{
		had = ch->in_room->area->helps;
		argument = fullarg;
	}

	if ( help_lookup(argument) )
	{
		send_to_char( "HEdit : A helpfile with that name already exists.\n\r", ch );
		return FALSE;
	}

	if (!had) /* the area currently has no helps */
	{
		had		= new_had();
		had->filename	= str_dup(ch->in_room->area->file_name);
		had->area	= ch->in_room->area;
		had->first	= NULL;
		had->last	= NULL;
		had->changed	= TRUE;
		had->next	= had_list;
		had_list	= had;
		ch->in_room->area->helps = had;
		SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
	}

	help		= new_help();
	help->level	= 0;
	help->keyword	= str_dup(argument);
	help->text	= str_dup( "(None)" );

	if (help_last)
		help_last->next	= help;

	if (help_first == NULL)
		help_first = help;

	help_last	= help;
	help->next	= NULL;

	if ( !had->first )
		had->first	= help;
	if ( !had->last )
		had->last	= help;

	had->last->next_area	= help;
	had->last		= help;
	help->next_area		= NULL;

	ch->desc->pEdit		= (HELP_DATA *) help;
	ch->desc->editor	= ED_HELP;

	send_to_char( "Ok.\n\r", ch );
	return FALSE;
}

HEDIT( hedit_text )
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( !IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax: text\n\r", ch );
		return FALSE;
	}

	string_append( ch, &help->text );

	return TRUE;
}

void hedit( CHAR_DATA *ch, char *argument)
{
	HELP_DATA * pHelp;
	HELP_AREA *had;
	char arg[MIL]={'\0'};
	char command[MIL]={'\0'};
	int cmd = 0;

	smash_tilde(argument);
	strcpy(arg, argument);
	argument = one_argument( argument, command);

	EDIT_HELP(ch, pHelp);

	had = get_help_area(pHelp);

	if (had == NULL)
	{
		bugf( "hedit : helpfil '%s' is NULL", pHelp->keyword );
		edit_done(ch);
		return;
	}

	if (ch->pcdata->security < 9)
	{
		send_to_char("HEdit: Sorry, but your security rating is too low.\n\r",ch);
		edit_done(ch);
		return;
	}

	if (command[0] == '\0')
	{
		hedit_show(ch, argument);
		return;
	}

	if (!str_cmp(command, "done") )
	{
		edit_done(ch);
		return;
	}

	for (cmd = 0; hedit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix(command, hedit_table[cmd].name) )
		{
			if ((*hedit_table[cmd].olc_fun) (ch, argument))
				had->changed = TRUE;
			return;
		}
	}

	interpret(ch, arg);
	return;
}

void do_hedit(CHAR_DATA *ch, char *argument)
{
	HELP_DATA * pHelp;

	if ( IS_NPC(ch) )
		return;

	if ( (pHelp = help_lookup( argument )) == NULL )
	{
		send_to_char( "HEdit : That helpfile does not exist.\n\r", ch );
		return;
	}

	ch->desc->pEdit		= (void *) pHelp;
	ch->desc->editor	= ED_HELP;

	return;
}

HEDIT(hedit_delete)
{
	HELP_DATA * pHelp, * temp;
	HELP_AREA * had;
	DESCRIPTOR_DATA *d;
	bool found = FALSE;

	EDIT_HELP(ch, pHelp);

	for ( d = descriptor_list; d; d = d->next )
		if ( d->editor == ED_HELP && pHelp == (HELP_DATA *) d->pEdit )
			edit_done(d->character);

	if (help_first == pHelp)
		help_first = help_first->next;
	else
	{
		for ( temp = help_first; temp; temp = temp->next )
			if ( temp->next == pHelp )
				break;

		if ( !temp )
		{
			bugf( "hedit_delete : help '%s' not found in the help_first list!", pHelp->keyword );
			return FALSE;
		}

		temp->next = pHelp->next;
	}

	for ( had = had_list; had; had = had->next )
		if ( pHelp == had->first )
		{
			found = TRUE;
			had->first = had->first->next_area;
		}
		else
		{
			for ( temp = had->first; temp; temp = temp->next_area )
				if ( temp->next_area == pHelp )
					break;

			if ( temp )
			{
				temp->next_area = pHelp->next_area;
				found = TRUE;
				break;
			}
		}

	if ( !found )
	{
		bugf( "hedit_delete : help %s not found in had_list!", pHelp->keyword );
		return FALSE;
	}

	free_help(pHelp);

	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_list)
{
	char buf[MIL]={'\0'};
	int cnt = 0;
	HELP_DATA *pHelp;
	BUFFER *buffer;

	EDIT_HELP(ch, pHelp);

	if ( !str_cmp( argument, "all" ) )
	{
		buffer = new_buf();

		for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
		{
			sprintf( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
				cnt % 4 == 3 ? "\n\r" : " " );
			add_buf( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
			add_buf( buffer, "\n\r" );

		page_to_char( buf_string(buffer), ch );
		return FALSE;
	}

	if ( !str_cmp( argument, "area" ) )
	{
		if ( ch->in_room->area->helps == NULL )
		{
			send_to_char( "There are no helps in this area.\n\r", ch );
			return FALSE;
		}

		buffer = new_buf();

		for ( pHelp = ch->in_room->area->helps->first; pHelp; pHelp = pHelp->next_area )
		{
			sprintf( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
				cnt % 4 == 3 ? "\n\r" : " " );
			add_buf( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
			add_buf( buffer, "\n\r" );

		page_to_char( buf_string(buffer), ch );
		return FALSE;
	}

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : list all\n\r", ch );
		send_to_char( "         list area\n\r", ch );
		return FALSE;
	}

	return FALSE;
}
