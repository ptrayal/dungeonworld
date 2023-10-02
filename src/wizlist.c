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
* ROM 2.4 is copyright 1993-1998 Russ Taylor                               *
* ROM has been brought to you by the ROM consortium                        *
*     Russ Taylor (rtaylor@hypercube.org)                                  *
*     Gabrielle Taylor (gtaylor@efn.org)                                   *
*     Brian Moore (zump@rom.org)                                           *
* By using this code, you have agreed to follow the terms of the           *
* ROM license, in the file Rom24/doc/rom.license                           *
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
 
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "merc.h"
#include "recycle.h"
 
WIZ_DATA *wiz_list;
 
/**************************************************
 * This needs to match what is found in interp.c  *
 **************************************************/

char * const wiz_titles [] =
{
	"Implementors",
	"Creators",
	"Supremacies",
	"Deities",
	"Gods",
	"Immortals",
	"DemiGods",
	"Knights",
	"Squires"
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
    if ((fp = fopen(WIZ_FILE, "w")) == NULL)
    {
        bug("save_wizlist: Could not open %s for writing", 0);
        return;
    }

    for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
    {
        found = TRUE;
        fprintf(fp, "%s %d\n", pwiz->name, pwiz->level);
    }

    fclose(fp);
    openReserve();

    if (!found)
    {
        if (remove(WIZ_FILE) != 0)
        {
            bug("save_wizlist: Could not remove empty wizlist file", 0);
        }
    }
}
 
void load_wizlist(void)
{
    FILE *fp;
    WIZ_DATA *wiz_last;

    if ((fp = fopen(WIZ_FILE, "r")) == NULL)
    {
        bug("load_wizlist: Could not open %s for reading", 0);
        return;
    }

    wiz_last = NULL;
    for (;;)
    {
        WIZ_DATA *pwiz;
        if (feof(fp))
        {
            fclose(fp);
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
    char arg1[MIL] = {'\0'};
    char arg2[MIL] = {'\0'};
    char arg3[MIL] = {'\0'};
    char buf[MSL] = {'\0'};
    char title[MSL] = {'\0'};
    BUFFER *buffer;
    WIZ_DATA *pwiz;
    int level = 0;
    int lngth = 0;
    int amt = 0;
    bool found;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if ((arg1[0] != '\0') && (ch->level == MAX_LEVEL))
    {
        if (!str_prefix(arg1, "add"))
        {
            if (!is_number(arg2) || (arg3[0] == '\0'))
            {
                send_to_char("Syntax: wizlist add <level> <name>\n\r", ch);
                return;
            }
            level = atoi(arg2);
            change_wizlist(ch, TRUE, level, arg3);
            return;
        }
        if (!str_prefix(arg1, "delete"))
        {
            if (arg2[0] == '\0')
            {
                send_to_char("Syntax: wizlist delete <name>\n\r", ch);
                return;
            }
            change_wizlist(ch, FALSE, 0, arg2);
            return;
        }
        send_to_char("Syntax:\n\r", ch);
        send_to_char(" wizlist delete <name>\n\r", ch);
        send_to_char(" wizlist add <level> <name>\n\r", ch);
        return;
    }

    if (wiz_list == NULL)
    {
        send_to_char("No immortals listed at this time.\n\r", ch);
        return;
    }

    buffer = new_buf();

    snprintf(title, sizeof(title), "The Gods of Dungeon World");

    snprintf(buf, sizeof(buf), "___________________________________________________________________________\n\r");
    add_buf(buffer, buf);
    snprintf(buf, sizeof(buf), " /\\_\\%70s\\_\\\n\r", " ");
    add_buf(buffer, buf);
    lngth = (70 - strlen(title)) / 2;
    for (; lngth >= 0; lngth--)
    {
        strcat(title, " ");
    }
    
    // Clear the buffer
    buf[0] = '\0';

    // Concatenate the parts to the buffer
    strncat(buf, "|/\\\\_\\                      \tW", sizeof(buf) - strlen(buf) - 1); // Leave space for null terminator
    strncat(buf, title, sizeof(buf) - strlen(buf) - 1);        // Leave space for null terminator
    strncat(buf, "\tn\\_|\n\r", sizeof(buf) - strlen(buf) - 1);   // Leave space for null terminator

    // snprintf(buf, sizeof(buf), "|/\\\\_\\\tW%-76s\tn\\_\\\n\r", title);
    add_buf(buffer, buf);
    snprintf(buf, sizeof(buf), "\\_/_|_|%69s|_|\n\r", " ");
    add_buf(buffer, buf);

    for (level = IMPLEMENTOR; level > HERO; level--)
    {
        found = FALSE;
        amt = 0;

        for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
        {
            if (pwiz->level == level)
            {
                amt++;
                found = TRUE;
            }
        }

        if (!found)
        {
            if (level == HERO + 1)
            {
                snprintf(buf, sizeof(buf), " ___|_|%69s|_|\n\r", " ");
                add_buf(buffer, buf);
            }
            continue;
        }

        snprintf(buf, sizeof(buf), " |_|%37s [%d]%30s|_|\n\r", wiz_titles[IMPLEMENTOR - level], level, " ");
        add_buf(buffer, buf);
        snprintf(buf, sizeof(buf), " |_|\tY%25s******************\tn%29s|_|\n\r", " ", " ");
        add_buf(buffer, buf);
        lngth = 0;

        for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
        {
            if (pwiz->level == level)
            {
                if (lngth == 0)
                {
                    if (amt > 2)
                    {
                        snprintf(buf, sizeof(buf), " |_|%12s%-23s ", pwiz->name, " ");
                        add_buf(buffer, buf);
                        lngth = 1;
                    }
                    else if (amt > 1)
                    {
                        snprintf(buf, sizeof(buf), " |_|%21s%-23s ", pwiz->name, " ");
                        add_buf(buffer, buf);
                        lngth = 1;
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), " |_|%30s%-42s|_|\n\r", pwiz->name, " ");
                        add_buf(buffer, buf);
                        lngth = 0;
                    }
                }
                else if (lngth == 1)
                {
                    if (amt > 2)
                    {
                        snprintf(buf, sizeof(buf), "%-23s ", pwiz->name);
                        add_buf(buffer, buf);
                        lngth = 2;
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "%-30s|_|\n\r", pwiz->name);
                        add_buf(buffer, buf);
                        lngth = 0;
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "%-21s|_|\n\r", pwiz->name);
                    add_buf(buffer, buf);
                    lngth = 0;
                    amt -= 3;
                }
            }
        }

        if (level == HERO + 1)
        {
            snprintf(buf, sizeof(buf), " ___|_|%72s|_|\n\r", " ");
        }
        else
        {
            snprintf(buf, sizeof(buf), " |_|%72s|_|\n\r", " ");
        }
        add_buf(buffer, buf);
    }

    snprintf(buf, sizeof(buf), "/ \\ |_|%69s|_|\n\r", " ");
    add_buf(buffer, buf);
    snprintf(buf, sizeof(buf), "|\\//_/%70s/_/\n\r", " ");
    add_buf(buffer, buf);
    snprintf(buf, sizeof(buf), " \\/_/______________________________________________________________________/_/\n\r");
    add_buf(buffer, buf);

    // Display the buffer content to the character and free the buffer
    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
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
    char arg[MIL] = {'\0'};
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
    }
    else
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


void clear_wizlist(void) 
{
    WIZ_DATA * wiz, *wiz_next;

    log_string("Cleaning: wiz_list");
    for(wiz = wiz_list; wiz; wiz = wiz_next)
    {
        wiz_next = wiz->next;
        UNLINK_SINGLE(wiz, next, WIZ_DATA, wiz_list);
        free_wiz(wiz);
    }
    wiz_list = NULL;
}

