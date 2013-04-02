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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

/* stuff for recyling notes */
NOTE_DATA *note_free;

NOTE_DATA *new_note()
{
	NOTE_DATA *note;

	if (note_free == NULL)
	note = alloc_perm(sizeof(*note));
	else
	{ 
	note = note_free;
	note_free = note_free->next;
	}
	VALIDATE(note);
	return note;
}

void free_note(NOTE_DATA *note)
{
	if (!IS_VALID(note))
	return;

	PURGE_DATA( note->text    );
	PURGE_DATA( note->subject );
	PURGE_DATA( note->to_list );
	PURGE_DATA( note->date    );
	PURGE_DATA( note->sender  );
	INVALIDATE(note);

	note->next = note_free;
	note_free   = note;
}

	
/* stuff for recycling ban structures */
BAN_DATA *ban_free;

BAN_DATA *new_ban(void)
{
	static BAN_DATA ban_zero;
	BAN_DATA *ban;

	if (ban_free == NULL)
	ban = alloc_perm(sizeof(*ban));
	else
	{
	ban = ban_free;
	ban_free = ban_free->next;
	}

	*ban = ban_zero;
	VALIDATE(ban);
	ban->name = &str_empty[0];
	return ban;
}

void free_ban(BAN_DATA *ban)
{
	if (!IS_VALID(ban))
	return;

	PURGE_DATA(ban->name);
	INVALIDATE(ban);

	ban->next = ban_free;
	ban_free = ban;
}

/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor(void)
{
	static DESCRIPTOR_DATA d_zero;
	DESCRIPTOR_DATA *d;

	if (descriptor_free == NULL)
	d = alloc_perm(sizeof(*d));
	else
	{
	d = descriptor_free;
	descriptor_free = descriptor_free->next;
	}
	
	*d = d_zero;
	VALIDATE(d);
	return d;
}

void free_descriptor(DESCRIPTOR_DATA *d)
{
	Escape(d);

	PURGE_DATA( d->host );
	PURGE_DATA( d->outbuf );
	PURGE_DATA(d);
}

/* stuff for recycling gen_data */
GEN_DATA *gen_data_free;

GEN_DATA *new_gen_data(void)
{
	static GEN_DATA gen_zero;
	GEN_DATA *gen;

	if (gen_data_free == NULL)
	gen = alloc_perm(sizeof(*gen));
	else
	{
	gen = gen_data_free;
	gen_data_free = gen_data_free->next;
	}
	*gen = gen_zero;
	VALIDATE(gen);
	return gen;
}

void free_gen_data(GEN_DATA *gen)
{
	if (!IS_VALID(gen))
	return;

	INVALIDATE(gen);

	gen->next = gen_data_free;
	gen_data_free = gen;
} 

/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *new_extra_descr(void)
{
	EXTRA_DESCR_DATA *ed;

	if (extra_descr_free == NULL)
	ed = alloc_perm(sizeof(*ed));
	else
	{
	ed = extra_descr_free;
	extra_descr_free = extra_descr_free->next;
	}

	ed->keyword = &str_empty[0];
	ed->description = &str_empty[0];
	VALIDATE(ed);
	return ed;
}

void free_extra_descr(EXTRA_DESCR_DATA *ed)
{
	if (!IS_VALID(ed))
	return;

	PURGE_DATA(ed->keyword);
	PURGE_DATA(ed->description);
	INVALIDATE(ed);
	
	ed->next = extra_descr_free;
	extra_descr_free = ed;
}


/* stuff for recycling affects */
AFFECT_DATA *affect_free;

AFFECT_DATA *new_affect(void)
{
	static AFFECT_DATA af_zero;
	AFFECT_DATA *af;

	if (affect_free == NULL)
	af = alloc_perm(sizeof(*af));
	else
	{
	af = affect_free;
	affect_free = affect_free->next;
	}

	*af = af_zero;


	VALIDATE(af);
	return af;
}

void free_affect(AFFECT_DATA *af)
{
	if (!IS_VALID(af))
	return;

	INVALIDATE(af);
	af->next = affect_free;
	affect_free = af;
}

/* stuff for recycling objects */
OBJ_DATA *obj_free;

OBJ_DATA *new_obj(void)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;

	if (obj_free == NULL)
	obj = alloc_perm(sizeof(*obj));
	else
	{
	obj = obj_free;
	obj_free = obj_free->next;
	}
	*obj = obj_zero;
	VALIDATE(obj);

	return obj;
}

void free_obj(OBJ_DATA *obj)
{
	AFFECT_DATA *paf, *paf_next;
	EXTRA_DESCR_DATA *ed, *ed_next;

	if (!IS_VALID(obj))
	return;

	for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	paf_next = paf->next;
	free_affect(paf);
	}
	obj->affected = NULL;

	for (ed = obj->extra_descr; ed != NULL; ed = ed_next )
	{
	ed_next = ed->next;
	free_extra_descr(ed);
	 }
	 obj->extra_descr = NULL;
   
	PURGE_DATA( obj->name        );
	PURGE_DATA( obj->description );
	PURGE_DATA( obj->short_descr );
	PURGE_DATA( obj->owner     );
	INVALIDATE(obj);

	obj->next   = obj_free;
	obj_free    = obj; 
}


/* stuff for recyling characters */
CHAR_DATA *char_free;

CHAR_DATA *new_char (void)
{
	static CHAR_DATA ch_zero;
	CHAR_DATA *ch;
	int i;

	if (char_free == NULL)
	ch = alloc_perm(sizeof(*ch));
	else
	{
	ch = char_free;
	char_free = char_free->next;
	}

	*ch				= ch_zero;
	VALIDATE(ch);
	ch->name                    = &str_empty[0];
	ch->short_descr             = &str_empty[0];
	ch->long_descr              = &str_empty[0];
	ch->description             = &str_empty[0];
	ch->prompt                  = &str_empty[0];
	ch->prefix			= &str_empty[0];
	ch->logon                   = current_time;
	ch->lines                   = PAGELEN;
	for (i = 0; i < 4; i++)
		ch->armor[i]            = 100;
	ch->position                = POS_STANDING;
	ch->hit                     = 20;
	ch->max_hit                 = 20;
	ch->mana                    = 100;
	ch->max_mana                = 100;
	ch->move                    = 100;
	ch->max_move                = 100;
	for (i = 0; i < MAX_STATS; i ++)
	{
		ch->perm_stat[i] = 13;
		ch->mod_stat[i] = 0;
	}

	return ch;
}


void free_char (CHAR_DATA *ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	if (!IS_VALID(ch))
	return;

	if (IS_NPC(ch))
	mobile_count--;

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	obj_next = obj->next_content;
	extract_obj(obj);
	}

	for (paf = ch->affected; paf != NULL; paf = paf_next)
	{
	paf_next = paf->next;
	affect_remove(ch,paf);
	}

	PURGE_DATA(ch->name);
	PURGE_DATA(ch->short_descr);
	PURGE_DATA(ch->long_descr);
	PURGE_DATA(ch->description);
	PURGE_DATA(ch->prompt);
	PURGE_DATA(ch->prefix);
	free_note  (ch->pnote);
	free_pcdata(ch->pcdata);

	ch->next = char_free;
	char_free  = ch;

	INVALIDATE(ch);
	return;
}

PC_DATA *pcdata_free;

PC_DATA *new_pcdata(void)
{
	int alias;

	static PC_DATA pcdata_zero;
	PC_DATA *pcdata;

	if (pcdata_free == NULL)
	pcdata = alloc_perm(sizeof(*pcdata));
	else
	{
	pcdata = pcdata_free;
	pcdata_free = pcdata_free->next;
	}

	*pcdata = pcdata_zero;

	for (alias = 0; alias < MAX_ALIAS; alias++)
	{
	pcdata->alias[alias] = NULL;
	pcdata->alias_sub[alias] = NULL;
	}

	pcdata->buffer = new_buf();
	
	VALIDATE(pcdata);
	return pcdata;
}
	

void free_pcdata(PC_DATA *pcdata)
{
	int alias;

	if (!IS_VALID(pcdata))
	return;

	PURGE_DATA(pcdata->pwd);
	PURGE_DATA(pcdata->bamfin);
	PURGE_DATA(pcdata->bamfout);
	PURGE_DATA(pcdata->title);
	free_buf(pcdata->buffer);
	
	for (alias = 0; alias < MAX_ALIAS; alias++)
	{
	PURGE_DATA(pcdata->alias[alias]);
	PURGE_DATA(pcdata->alias_sub[alias]);
	}
	INVALIDATE(pcdata);
	pcdata->next = pcdata_free;
	pcdata_free = pcdata;

	return;
}

	


/* stuff for setting ids */
long	last_pc_id;
long	last_mob_id;

long get_pc_id(void)
{
	int val;

	val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
	last_pc_id = val;
	return val;
}

long get_mob_id(void)
{
	last_mob_id++;
	return last_mob_id;
}

MEM_DATA *mem_data_free;

/* procedures and constants needed for buffering */

BUFFER *buf_free;

MEM_DATA *new_mem_data(void)
{
	MEM_DATA *memory;
  
	if (mem_data_free == NULL)
	memory = alloc_mem(sizeof(*memory));
	else
	{
	memory = mem_data_free;
	mem_data_free = mem_data_free->next;
	}

	memory->next = NULL;
	memory->id = 0;
	memory->reaction = 0;
	memory->when = 0;
	VALIDATE(memory);

	return memory;
}

void free_mem_data(MEM_DATA *memory)
{
	if (!IS_VALID(memory))
	return;

	memory->next = mem_data_free;
	mem_data_free = memory;
	INVALIDATE(memory);
}



/* buffer sizes */
const int buf_size[MAX_BUF_LIST] =
{
	16,32,64,128,256,1024,2048,4096,8192,16384
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size (int val)
{
	int i;

	for (i = 0; i < MAX_BUF_LIST; i++)
	if (buf_size[i] >= val)
	{
		return buf_size[i];
	}
	
	return -1;
}

BUFFER *__new_buf( const char *file, const char *function, int line)
{
	BUFFER *buffer;

	ALLOC_DATA(buffer, BUFFER, 1);

	buffer->next	= NULL;
	buffer->state	= BUFFER_SAFE;
	buffer->size	= get_size(BASE_BUF);

	//  FOR DEBUGGIN PURPOSES
	buffer->file = str_dup(file);
	buffer->function = str_dup(function);
	buffer->line = line;

	ALLOC_DATA(buffer->string, char, buffer->size);

	buffer->string[0]	= '\0';
	top_buffer++;
	LINK_SINGLE(buffer, next, buffer_list);

	VALIDATE(buffer);

	return buffer;
}

BUFFER *new_buf_size(int size)
{
	BUFFER *buffer;
 
	ALLOC_DATA(buffer, BUFFER, 1);
 
	buffer->next        = NULL;
	buffer->state       = BUFFER_SAFE;
	buffer->size        = get_size(size);
	buffer->file		= NULL;
	buffer->function	= NULL;

	if (buffer->size == -1)
	{
		bug("new_buf: buffer size %d too large.",size);
		exit(1);
	}
	ALLOC_DATA(buffer->string, char, buffer->size);
	buffer->string[0]   = '\0';
	VALIDATE(buffer);
 
	return buffer;
}


void free_buf(BUFFER *buffer)
{
	Escape(buffer);

	free(buffer->file);
	buffer->file = NULL;
	free(buffer->function);
	buffer->function = NULL;

	top_buffer--;
	UNLINK_SINGLE(buffer, next, BUFFER, buffer_list);

	PURGE_DATA(buffer->file);
	PURGE_DATA(buffer->function);
	PURGE_DATA(buffer->string);
	buffer->string = NULL;
	buffer->size   = 0;
	buffer->state  = BUFFER_FREED;

	INVALIDATE(buffer);
	PURGE_DATA(buffer);
}


bool add_buf(BUFFER *buffer, char *string)
{
	int len = 0;
	char *oldstr;
	int oldsize = buffer->size;

	oldstr = buffer->string;

	if (buffer->state == BUFFER_OVERFLOW) /* don't waste time on bad strings! */
	return FALSE;

	len = strlen(buffer->string) + strlen(string) + 1;

	while (len >= buffer->size) /* increase the buffer size */
	{
		buffer->size 	= get_size(buffer->size + 1);
		{
		if (buffer->size == -1) /* overflow */
			{
				buffer->size = oldsize;
				buffer->state = BUFFER_OVERFLOW;
				bug("buffer overflow past size %d",buffer->size);
				return FALSE;
			}
		}
	}

	if (buffer->size != oldsize)
	{
		ALLOC_DATA(buffer->string, char, buffer->size);

		strncpy(buffer->string,oldstr, buffer->size);
		PURGE_DATA(oldstr);
	}

	strncat(buffer->string,string, buffer->size);
	return TRUE;
}

void BufPrintf ( BUFFER * buffer, char * fmt, ...)
{
	char buf[MSL] = {'\0'};
	va_list args;

	va_start (args, fmt);
	vsnprintf( buf, sizeof(buf), fmt, args);
	va_end (args);

	add_buf(buffer, buf);
	return;
}


void clear_buf(BUFFER *buffer)
{
	buffer->string[0] = '\0';
	buffer->state     = BUFFER_SAFE;
}


char *buf_string(BUFFER *buffer)
{
	return buffer->string;
}

/* stuff for recycling mobprograms */
MPROG_LIST *mprog_free;

MPROG_LIST *new_mprog(void)
{
   static MPROG_LIST mp_zero;
   MPROG_LIST *mp;

   if (mprog_free == NULL)
	   mp = alloc_perm(sizeof(*mp));
   else
   {
	   mp = mprog_free;
	   mprog_free=mprog_free->next;
   }

   *mp = mp_zero;
   mp->vnum             = 0;
   mp->trig_type        = 0;
   mp->code             = str_dup("");
   VALIDATE(mp);
   return mp;
}

void free_mprog(MPROG_LIST *mp)
{
   if (!IS_VALID(mp))
	  return;

   INVALIDATE(mp);
   mp->next = mprog_free;
   mprog_free = mp;
}

HELP_AREA * had_free;

HELP_AREA * new_had ( void )
{
	HELP_AREA * had;
static	HELP_AREA   zHad;

	if ( had_free )
	{
		had		= had_free;
		had_free	= had_free->next;
	}
	else
		had		= alloc_perm( sizeof( *had ) );

	*had = zHad;

	return had;
}

HELP_DATA * help_free;

HELP_DATA * new_help ( void )
{
	HELP_DATA * help;

	if ( help_free )
	{
		help		= help_free;
		help_free	= help_free->next;
	}
	else
		help		= alloc_perm( sizeof( *help ) );

	return help;
}

void free_help(HELP_DATA *help)
{
	PURGE_DATA(help->keyword);
	PURGE_DATA(help->text);
	help->next = help_free;
	help_free = help;
}

// New stuff to recycle
/* stuff for recycling wizlist structures */
WIZ_DATA *wiz_free;

WIZ_DATA *new_wiz(void)
{
	static WIZ_DATA wiz_zero;
	WIZ_DATA *wiz;

	if (wiz_free == NULL)
		wiz = alloc_perm(sizeof(*wiz));
	else
	{
		wiz = wiz_free;
		wiz_free = wiz_free->next;
	}

	*wiz = wiz_zero;
	VALIDATE(wiz);
	wiz->name = &str_empty[0];
	return wiz;
}

void free_wiz(WIZ_DATA *wiz)
{
	if (!IS_VALID(wiz))
		return;

	PURGE_DATA(wiz->name);
	INVALIDATE(wiz);

	wiz->next = wiz_free;
	wiz_free = wiz;
}
