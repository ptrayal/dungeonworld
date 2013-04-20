/* File - special.h
  * Description - This file contains all header information
  * for random mobs. Condition of use of
  * this file is as follows:
  * 1. All Diku/Merc/Rom licences must be followed
  * 2. A help file with the keyword 'RMOB'
  * containing my AIM/ICQ number and credit for
  * this code.
  * 3. An email/AIM telling me you are using this code.
  * Please do not use this code if you cannot follow these simple
  * steps. I have put in alot of work on this, and i would like credit
  * where credit is due.
  *
  * -Thri
  * AIM: CalibanL
  * ICQ: 5335308
  * Email: cyhawk@comcast.net
 */
 
#define MAX_RMOB_ORC_NAME 20 // Max Names in Table
#define MAX_RMOB_ORC_LONG 6 // Max Long_descr's in table
#define MAX_RMOB_OTHER 2 // Max 'other' stats
 
extern const struct rmob_orc_name_type rmob_orc_name_table [];
extern const struct rmob_orc_long_type rmob_orc_long_table [];
extern const struct rmob_orc_other_type rmob_orc_other_table [];
 
struct rmob_orc_name_type
{
	char * name;	// Possible names of Mob
};
 
struct rmob_orc_long_type
{
	char * long_descr;	// Possible long descriptions
};	// End note: Good to have alot of these, 1.5x names is good.
 
struct rmob_orc_other_type
{
	sh_int low_level_range;	// Lowest number to subtract from level
	sh_int hig_level_range;	// Highest number to add to level
	sh_int l_hitroll;	// Lowest Hitroll
	sh_int h_hitroll;	// Highest Hitroll
	sh_int l_damroll;	// Lowest Damroll
	sh_int h_damroll;	// Highest Damroll
	sh_int hit_dice;	// Size of Hitdice. dice(level, hit_dice)
	long affect_flags;	// Affect flags to set on the mob
};
