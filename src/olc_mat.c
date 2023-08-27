// olc materials

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
#include "recycle.h"
#include "lookup.h"

MAT_TYPE *mat_list;
int matValue;

#define MATEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )

const struct olc_cmd_type matedit_table[] =
{
/*  {   command		function	}, */

	{	"create",	matedit_create	},
    {   "name",		matedit_name	},
    {	"show",		matedit_show	},
    {   "?",		show_help		},
    {   "version",	show_version	},

    {	NULL,		0,		}
};

MAT_TYPE *new_mat()
{
	MAT_TYPE *mat;
	
	ALLOC_DATA(mat, MAT_TYPE, 1);

	mat->next = NULL;
	mat->name = NULL;
	mat->assignedValue = 0;

	return mat;
}

void free_mat(MAT_TYPE *mat) {
	Assert(mat, "Material non-existant! Crashing");
	
	PURGE_DATA(mat->name);
	PURGE_DATA(mat);
}

MAT_TYPE *find_mat(const char *name) {
	MAT_TYPE *mat, *mat_next;
	
	if(IS_NULLSTR(name))
		return NULL;
	
	for(mat = mat_list; mat; mat = mat_next) {
		mat_next = mat->next;
		
		if(IS_NULLSTR(mat->name))
			continue;
		
		if(!str_cmp(mat->name, name))
			return mat;
	}
	return NULL;
}

void clear_materials(void) {
	MAT_TYPE *mat, *mat_next;
		
	log_string("Cleaning: mat_list");		

	for(mat = mat_list; mat; mat = mat_next) {
		mat_next = mat->next;
		
		free_mat(mat);
	}
	mat_list = NULL;
	
	return;
}

void confirm_material(const char *name) {
	MAT_TYPE *mat, *mat_next;
	
	if(IS_NULLSTR(name))
		return;
	
	// materials that are '0' are NULL
	if(is_number(name))
		return;
	
	for(mat = mat_list; mat; mat = mat_next) {
		mat_next = mat->next;

		if(IS_NULLSTR(mat->name))
			continue;

		if(!str_cmp(mat->name, name))
			return;
	}
	
	matValue++;
	mat = new_mat();

	mat->name = str_dup(name);
	mat->assignedValue = matValue;

	mat->next = mat_list;
	mat_list = mat;

	return;
}

void save_materials(void) {
	closeReserve();
	FILE *fp = fopen(MATERIAL_FILE, "w");
	Assert(fp, "unable to open the materials file");
	
	// handle our materials
	MAT_TYPE *mat, *mat_next;
	
	for(mat = mat_list; mat; mat = mat_next) {
		mat_next = mat->next;
		fprintf(fp, "MAT\n");
		fprintf(fp, "NAME %s~\n", mat->name);
		fprintf(fp, "VAL %d\n", mat->assignedValue);
		fprintf(fp, "ENDMAT\n");
	}
	fprintf(fp, "End\n");
	
	fclose(fp);
	openReserve();
}

void load_materials(void) {
	bool fMatch;

	closeReserve();
	FILE *fp = fopen(MATERIAL_FILE, "r");
	if(!fp) return; // no materials to load.

	while(true)
	{
		const char *word  = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		if(!str_cmp(word, "End")) {
			break;
		}

		if(!str_cmp(word, "MAT")) {
			MAT_TYPE *mat = new_mat();

			while(true) {
				word = feof( fp ) ? "End" : fread_word(fp);
				if(!str_cmp(word, "End")) {
					bug(Format("Prematurely reached end of materials data file; data corrupted!"), 0);
					fclose(fp);
					openReserve();
					return;
				}
				bool finished = false;
				switch ( UPPER(word[0]) )
				{
				case '*':
					fMatch = TRUE;
					fread_to_eol( fp );
					break;
				case 'E':
					if(!str_cmp(word, "ENDMAT")) {

						mat->next = mat_list;
						mat_list = mat;

						matValue++;
						finished = true;
						break;
					}
				case 'V':
					KEY("VAL",	mat->assignedValue, fread_number(fp) );
					break;
				case 'N':
					KEYS( "NAME",		mat->name,		fread_string(fp) );
					break;
				}
				if(finished)
					break;
			}
		}
	}
	fclose(fp);
	openReserve();
}

MATEDIT( matedit_create )
{
	MAT_TYPE *pMat;
	
	if ( IS_NULLSTR(argument))
	{
	send_to_char( "Syntax:  create [name]\n\r", ch );
	return FALSE;
	}

	pMat = find_mat( argument );
	if ( pMat )
	{
		send_to_char( "MATEdit:  That material already exists\n\r", ch );
		return FALSE;
	}
	
	pMat = new_mat();

	pMat->name = str_dup(argument);
	pMat->assignedValue = 0;
	
	pMat->next = mat_list;
	mat_list = pMat;

	ch->desc->pEdit		= (void *)pMat;

	send_to_char( "Material created.\n\r", ch );
	save_materials();
	return TRUE;
}

MATEDIT( matedit_name )
{
	MAT_TYPE *pMat;

	EDIT_MAT(ch, pMat);

	if ( IS_NULLSTR(argument) )
	{
	send_to_char( "Syntax:  name [name]\n\r", ch );
	return FALSE;
	}

	PURGE_DATA( pMat->name );
	pMat->name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch );
	return TRUE;
}

MATEDIT(matedit_show) {
	MAT_TYPE *pMat;	
	EDIT_MAT(ch, pMat);

	send_to_char(Format("Name: %s\n\r", pMat->name), ch);
	send_to_char(Format("assignedValue: %d (Non-Settable)\n\r", pMat->assignedValue), ch);
	return FALSE;
}

/* Area Interpreter, called by do_aedit. */
void matedit( CHAR_DATA *ch, char *argument )
{
    MAT_TYPE *pMat;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int  cmd;

    EDIT_MAT(ch, pMat);
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !str_cmp(command, "done") )
    {
    	// save our materials
    	save_materials();
    	send_to_char("Materials Saved to disk.\n\r", ch);
		edit_done( ch );
		return;
    }

    if ( IS_NULLSTR(command) )
    {
		matedit_show( ch, argument );
		return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; matedit_table[cmd].name != NULL; cmd++ )
    {
	if ( !str_prefix( command, matedit_table[cmd].name ) )
	{
	    if ( (*matedit_table[cmd].olc_fun) ( ch, argument ) )
	    {
			return;
	    }
	    else
			return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}

/* Entry point for editing obj_index_data. */
void do_matedit( CHAR_DATA *ch, char *argument )
{
    char arg1[MSL]={'\0'};

    if ( IS_NPC(ch) )
	return;

    argument = one_argument( argument, arg1 );

    if ( find_mat(arg1) )
    {
	ch->desc->pEdit = (void *)find_mat(arg1);
	ch->desc->editor = ED_MAT;
	return;
    }
    else
    {
	if ( !str_cmp( arg1, "create" ) )
	{
		if(find_mat(argument)) {
			send_to_char("That material already exists!\n\r", ch);
			return;
		}
	    if ( IS_NULLSTR(argument))
	    {
		send_to_char( "Syntax:  edit material create [name]\n\r", ch );
		return;
	    }

		matedit_create(ch, argument);
	    return;
	}
    }

    send_to_char( "MATEdit:  There is no default material to edit.\n\r", ch );
    return;
}

void do_matlist(CHAR_DATA *ch, char *argument) {
	MAT_TYPE *mat, *mat_next;
	BUFFER *output = new_buf();
	int col = 0;
	for(mat = mat_list; mat; mat = mat_next) {
		mat_next = mat->next;

		add_buf(output, Format("%-15s", mat->name) );
		if(++col == 5) {
			col = 0;
			add_buf(output, "\n\r");
		}
	}
	page_to_char(buf_string(output), ch);
	free_buf(output);
	return;
}

// EOF
