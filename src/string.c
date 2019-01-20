/***************************************************************************
 *  File: string.c                                                         *
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
#include "tables.h"
#include "olc.h"

char *string_linedel( char *, int );
char *string_lineadd( char *, char *, int );
char *numlineas( char * );

/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit( CHAR_DATA *ch, char **pString )
{
		send_to_char( "-========- Entering EDIT Mode -=========-\n\r", ch );
		send_to_char( "    Type .h on a new line for help\n\r", ch );
		send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
		send_to_char( "-=======================================-\n\r", ch );

		if ( *pString == NULL )
		{
				*pString = NULL;
		}
		else
		{
				**pString = '\0';
		}

		ch->desc->pString = pString;

		return;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append( CHAR_DATA *ch, char **pString )
{
		send_to_char( "-=======- Entering APPEND Mode -========-\n\r", ch );
		send_to_char( "    Type .h on a new line for help\n\r", ch );
		send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
		send_to_char( "-=======================================-\n\r", ch );

		if ( *pString == NULL )
		{
				*pString = NULL;
		}
		send_to_char( numlineas(*pString), ch );

/* numlineas entrega el string con \n\r */
/*  if ( *(*pString + strlen( *pString ) - 1) != '\r' )
	send_to_char( "\n\r", ch ); */

		ch->desc->pString = pString;

		return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace( char * orig, char * old, char * inew )
{
		char xbuf[MSL]={'\0'};
		
		xbuf[0] = '\0';
		strcpy( xbuf, orig );
		if ( strstr( orig, old ) != NULL )
		{
				int i = 0;
				i = strlen( orig ) - strlen( strstr( orig, old ) );
				xbuf[i] = '\0';
				strcat( xbuf, inew );
				strcat( xbuf, &orig[i+strlen( old )] );
				PURGE_DATA( orig );
		}

		return str_dup( xbuf );
}



/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
		char buf[MSL]={'\0'};

		/*
		 * Thanks to James Seng
		 */
		smash_tilde( argument );

		if ( *argument == '.' )
		{
				char arg1 [MIL]={'\0'};
				char arg2 [MIL]={'\0'};
				char arg3 [MIL]={'\0'};
				char tmparg3 [MIL]={'\0'};

				argument = one_argument( argument, arg1 );
				argument = first_arg( argument, arg2, FALSE );
	strcpy( tmparg3, argument );
				argument = first_arg( argument, arg3, FALSE );

				if ( !str_cmp( arg1, ".c" ) )
				{
						send_to_char( "String cleared.\n\r", ch );
			PURGE_DATA(*ch->desc->pString);
			*ch->desc->pString = NULL;
						return;
				}

				if ( !str_cmp( arg1, ".s" ) )
				{
						send_to_char( "String so far:\n\r", ch );
						send_to_char( numlineas(*ch->desc->pString), ch );
						return;
				}

				if ( !str_cmp( arg1, ".r" ) )
				{
						if ( arg2[0] == '\0' )
						{
								send_to_char(
										"usage:  .r \"old string\" \"new string\"\n\r", ch );
								return;
						}

						*ch->desc->pString = string_replace( *ch->desc->pString, arg2, arg3 );
						send_to_char( Format("'%s' replaced with '%s'.\n\r", arg2, arg3), ch );
						return;
				}

				if ( !str_cmp( arg1, ".f" ) )
				{
						*ch->desc->pString = format_string( *ch->desc->pString );
						send_to_char( "String formatted.\n\r", ch );
						return;
				}
				
	if ( !str_cmp( arg1, ".ld" ) )
	{
		*ch->desc->pString = string_linedel( *ch->desc->pString, atoi(arg2) );
		send_to_char( "Line deleted.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg1, ".li" ) )
	{
		*ch->desc->pString = string_lineadd( *ch->desc->pString, tmparg3, atoi(arg2) );
		send_to_char( "Line inserted.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg1, ".lr" ) )
	{
		*ch->desc->pString = string_linedel( *ch->desc->pString, atoi(arg2) );
		*ch->desc->pString = string_lineadd( *ch->desc->pString, tmparg3, atoi(arg2) );
		send_to_char( "Line replaced.\n\r", ch );
		return;
	}

				if ( !str_cmp( arg1, ".h" ) )
				{
						send_to_char( "Sedit help (commands on blank line):   \n\r", ch );
						send_to_char( ".r 'old' 'new'   - replace a substring \n\r", ch );
						send_to_char( "                   (requires '', \"\") \n\r", ch );
						send_to_char( ".h               - get help (this info)\n\r", ch );
						send_to_char( ".s               - show string so far  \n\r", ch );
						send_to_char( ".f               - (word wrap) string  \n\r", ch );
						send_to_char( ".c               - clear string so far \n\r", ch );
						send_to_char( ".ld <num>        - delete line number <num>\n\r", ch );
						send_to_char( ".li <num> <str>  - insert <str> on line <num>\n\r", ch );
			send_to_char( ".lr <num> <str>  - replace line <num> with <str>\n\r", ch );
						send_to_char( "@                - end string          \n\r", ch );
						return;
				}

				send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
				return;
		}

		if ( *argument == '~' || *argument == '@' )
		{
	if ( ch->desc->editor == ED_MPCODE ) /* para los mobprogs */
	{
		MOB_INDEX_DATA *mob;
		int hash = 0;
		MPROG_LIST *mpl;
		MPROG_CODE *mpc;

		EDIT_MPCODE(ch, mpc);

		if ( mpc != NULL )
			for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
				for ( mob = mob_index_hash[hash]; mob; mob = mob->next )
					for ( mpl = mob->mprogs; mpl; mpl = mpl->next )
						if ( mpl->vnum == mpc->vnum )
						{
							send_to_char( Format("Updated mob %d.\n\r", mob->vnum), ch );
							mpl->code = mpc->code;
						}
	}

				ch->desc->pString = NULL;
				return;
		}

		strcpy ( buf, *ch->desc->pString ? *ch->desc->pString : "" ); 
		/*
		 * Truncate strings to MAX_STRING_LENGTH.
		 * --------------------------------------
		 */
		if ( strlen( buf ) + strlen( argument ) >= ( MSL - 4 ) )
		{
				send_to_char( "String too long, last line skipped.\n\r", ch );

	/* Force character out of editing mode. */
				ch->desc->pString = NULL;
				return;
		}

		/*
		 * Ensure no tilde's inside string.
		 * --------------------------------
		 */
		smash_tilde( argument );

		strcat( buf, argument );
		strcat( buf, "\n\r" );
		PURGE_DATA( *ch->desc->pString );
		*ch->desc->pString = str_dup( buf );
		return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 * Modifications for color codes and blank lines by Geoff.
 */
/*****************************************************************************
 Name: format_string
 Purpose: Special string formating and word-wrapping.
 Called by: string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string (char *oldstring /*, bool fSpace */ )
 {
 	char xbuf[MSL]={'\0'};
 	char xbuf2[MSL]={'\0'};
 	char *rdesc;
 	int i = 0;
 	int end_of_line;
 	bool cap = TRUE;
 	bool bFormat = TRUE;

 	xbuf[0] = xbuf2[0] = 0;

 	for (rdesc = oldstring; *rdesc; rdesc++)
 	{

 		if (*rdesc != '`')
 		{
 			if (bFormat)
 			{
 				if (*rdesc == '\n')
 				{
 					if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' && *(rdesc + 3) == '\n' && xbuf[i - 1] != '\r')
 					{
 						xbuf[i] = '\n';
 						xbuf[i + 1] = '\r';
 						xbuf[i + 2] = '\n';
 						xbuf[i + 3] = '\r';
 						i += 4;
 						rdesc += 2;
 					}
 					else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' && *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r')
 					{
 						xbuf[i] = '\n';
 						xbuf[i + 1] = '\r';
 						i += 2;
 					}
 					else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' && xbuf[i - 1] != '\r')
 					{
 						xbuf[i] = '\n';
 						xbuf[i + 1] = '\r';
 						xbuf[i + 2] = '\n';
 						xbuf[i + 3] = '\r';
 						i += 4;
 						rdesc += 1;
 					}
 					else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r')
 					{
 						xbuf[i] = '\n';
 						xbuf[i + 1] = '\r';
 						i += 2;
 					}
 					else if (xbuf[i - 1] != ' ' && xbuf[i - 1] != '\r')
 					{
 						xbuf[i] = ' ';
 						i++;
 					}
 				}
 				else if (*rdesc == '\r') ;
 				else if (*rdesc == 'i' && *(rdesc + 1) == '.' && *(rdesc + 2) == 'e' && *(rdesc + 3) == '.')
 				{
 					xbuf[i] = 'i';
 					xbuf[i + 1] = '.';
 					xbuf[i + 2] = 'e';
 					xbuf[i + 3] = '.';
 					i += 4;
 					rdesc += 3;
 				}
 				else if (*rdesc == ' ')
 				{
 					if (xbuf[i - 1] != ' ')
 					{
 						xbuf[i] = ' ';
 						i++;
 					}
 				}
 				else if (*rdesc == ')')
 				{
 					if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' '
 						&& (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
 					{
 						xbuf[i - 2] = *rdesc;
 						xbuf[i - 1] = ' ';
 						xbuf[i] = ' ';
 						i++;
 					}
 					else if (xbuf[i - 1] == ' ' && (xbuf[i - 2] == ',' || xbuf[i - 2] == ';'))
 					{
 						xbuf[i - 1] = *rdesc;
 						xbuf[i] = ' ';
 						i++;
 					}
 					else
 					{
 						xbuf[i] = *rdesc;
 						i++;
 					}
 				}
 				else if (*rdesc == ',' || *rdesc == ';')
 				{
 					if (xbuf[i - 1] == ' ')
 					{
 						xbuf[i - 1] = *rdesc;
 						xbuf[i] = ' ';
 						i++;
 					}
 					else
 					{
 						xbuf[i] = *rdesc;
 						if (*(rdesc + 1) != '\"')
 						{
 							xbuf[i + 1] = ' ';
 							i += 2;
 						}
 						else
 						{
 							xbuf[i + 1] = '\"';
 							xbuf[i + 2] = ' ';
 							i += 3;
 							rdesc++;
 						}
 					}

 				}
 				else if (*rdesc == '.' || *rdesc == '?' || *rdesc == '!')
 				{
 					if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' '
 						&& (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
 					{
 						xbuf[i - 2] = *rdesc;
 						if (*(rdesc + 1) != '\"')
 						{
 							xbuf[i - 1] = ' ';
 							xbuf[i] = ' ';
 							i++;
 						}
 						else
 						{
 							xbuf[i - 1] = '\"';
 							xbuf[i] = ' ';
 							xbuf[i + 1] = ' ';
 							i += 2;
 							rdesc++;
 						}
 					}
 					else
 					{
 						xbuf[i] = *rdesc;
 						if (*(rdesc + 1) != '\"')
 						{
 							xbuf[i + 1] = ' ';
 							xbuf[i + 2] = ' ';
 							i += 3;
 						}
 						else
 						{
 							xbuf[i + 1] = '\"';
 							xbuf[i + 2] = ' ';
 							xbuf[i + 3] = ' ';
 							i += 4;
 							rdesc++;
 						}
 					}
 					cap = TRUE;
 				}
 				else
 				{
 					xbuf[i] = *rdesc;
 					if (cap)
 					{
 						cap = FALSE;
 						xbuf[i] = UPPER (xbuf[i]);
 					}
 					i++;
 				}
 			}
 			else
 			{
 				xbuf[i] = *rdesc;
 				i++;
 			}
 		}
 		else
 		{
 			if (*(rdesc + 1) == 'Z')
 				bFormat = !bFormat;
 			xbuf[i] = *rdesc;
 			i++;
 			rdesc++;
 			xbuf[i] = *rdesc;
 			i++;
 		}
 	}
 	xbuf[i] = 0;
 	strcpy (xbuf2, xbuf);

 	rdesc = xbuf2;

 	xbuf[0] = 0;

 	for (;;)
 	{
 		end_of_line = 77;
 		for (i = 0; i < end_of_line; i++)
 		{
 			if (*(rdesc + i) == '`')
 			{
 				end_of_line += 2;
 				i++;
 			}

 			if (!*(rdesc + i))
 				break;

 			if (*(rdesc + i) == '\r')
 				end_of_line = i;
 		}
 		if (i < end_of_line)
 		{
 			break;
 		}
 		if (*(rdesc + i - 1) != '\r')
 		{
 			for (i = (xbuf[0] ? (end_of_line - 1) : (end_of_line - 4)); i; i--)
 			{
 				if (*(rdesc + i) == ' ')
 					break;
 			}
 			if (i)
 			{
 				*(rdesc + i) = 0;
 				strcat (xbuf, rdesc);
 				strcat (xbuf, "\n\r");
 				rdesc += i + 1;
 				while (*rdesc == ' ')
 					rdesc++;
 			}
 			else
 			{
 				bug ("`5Wrap_string: `@No spaces``", 0);
 				*(rdesc + (end_of_line - 2)) = 0;
 				strcat (xbuf, rdesc);
 				strcat (xbuf, "-\n\r");
 				rdesc += end_of_line - 1;
 			}
 		}
 		else
 		{
 			*(rdesc + i - 1) = 0;
 			strcat (xbuf, rdesc);
 			strcat (xbuf, "\r");
 			rdesc += i;
 			while (*rdesc == ' ')
 				rdesc++;
 		}
 	}
 	while (*(rdesc + i) && (*(rdesc + i) == ' ' ||
 		*(rdesc + i) == '\n' ||
 		*(rdesc + i) == '\r'))
 		i--;
 	*(rdesc + i + 1) = 0;
 	strcat (xbuf, rdesc);
 	if (xbuf[strlen (xbuf) - 2] != '\n')
 		strcat (xbuf, "\n\r");

 	PURGE_DATA (oldstring);
 	return (str_dup (xbuf));
 }



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
		Understands quates, parenthesis (barring ) ('s) and
		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg( char *argument, char *arg_first, bool fCase )
{
		char cEnd;

		while ( *argument == ' ' )
	argument++;

		cEnd = ' ';
		if ( *argument == '\'' || *argument == '"'
			|| *argument == '%'  || *argument == '(' )
		{
				if ( *argument == '(' )
				{
						cEnd = ')';
						argument++;
				}
				else cEnd = *argument++;
		}

		while ( *argument != '\0' )
		{
	if ( *argument == cEnd )
	{
			argument++;
			break;
	}
		if ( fCase ) *arg_first = LOWER(*argument);
						else *arg_first = *argument;
	arg_first++;
	argument++;
		}
		*arg_first = '\0';

		while ( *argument == ' ' )
	argument++;

		return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
		char buf[MSL]={'\0'};
		char *s;

		s = argument;

		while ( *s == ' ' )
				s++;

		strcpy( buf, s );
		s = buf;

		if ( *s != '\0' )
		{
				while ( *s != '\0' )
						s++;
				s--;

				while( *s == ' ' )
						s--;
				s++;
				*s = '\0';
		}

		PURGE_DATA( argument );
		return str_dup( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
		char *s;

		s = argument;

		while ( *s != '\0' )
		{
				if ( *s != ' ' )
				{
						*s = UPPER(*s);
						while ( *s != ' ' && *s != '\0' )
								s++;
				}
				else
				{
						s++;
				}
		}

		return argument;
}

char *string_linedel( char *string, int line )
{
	char *strtmp = string;
	char buf[MSL]={'\0'};
	int cnt = 1, tmp = 0;

	buf[0] = '\0';

	for ( ; *strtmp != '\0'; strtmp++ )
	{
		if ( cnt != line )
			buf[tmp++] = *strtmp;

		if ( *strtmp == '\n' )
		{
			if ( *(strtmp + 1) == '\r' )
			{
				if ( cnt != line )
					buf[tmp++] = *(++strtmp);
				else
					++strtmp;
			}

			cnt++;
		}
	}

	buf[tmp] = '\0';

	PURGE_DATA(string);
	return str_dup(buf);
}

char *string_lineadd( char *string, char *newstr, int line )
{
	char *strtmp = string;
	int cnt = 1, tmp = 0;
	bool done = FALSE;
	char buf[MSL]={'\0'};

	buf[0] = '\0';

	for ( ; *strtmp != '\0' || (!done && cnt == line); strtmp++ )
	{
		if ( cnt == line && !done )
		{
			strcat( buf, newstr );
			strcat( buf, "\n\r" );
			tmp += strlen(newstr) + 2;
			cnt++;
			done = TRUE;
		}

		buf[tmp++] = *strtmp;

		if ( done && *strtmp == '\0' )
			break;

		if ( *strtmp == '\n' )
		{
			if ( *(strtmp + 1) == '\r' )
				buf[tmp++] = *(++strtmp);

			cnt++;
		}

		buf[tmp] = '\0';
	}

	PURGE_DATA(string);
	return str_dup(buf);
}

/* buf queda con la linea sin \n\r */
char *get_line( char *str, char *buf )
{
	int tmp = 0;
	bool found = FALSE;

	while ( *str )
	{
		if ( *str == '\n' )
		{
			found = TRUE;
			break;
		}

		buf[tmp++] = *(str++);
	}

	if ( found )
	{
		if ( *(str + 1) == '\r' )
			str += 2;
		else
			str += 1;
	} /* para que quedemos en el inicio de la prox linea */

	buf[tmp] = '\0';

	return str;
}

char *numlineas( char *string )
{
	int cnt = 1;
	static char buf[MSL*2]={'\0'};
	char buf2[MSL]={'\0'};
	char tmpb[MSL]={'\0'};

	buf[0] = '\0';

	if(IS_NULLSTR(string))
		return "";

	while ( *string )
	{
		string = get_line( string, tmpb );
		sprintf( buf2, "%2d. %s\n\r", cnt++, tmpb );
		strcat( buf, buf2 );
	}

	return buf;
}
