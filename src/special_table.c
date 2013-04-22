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
 
 
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "special.h"
 
const struct rmob_orc_name_type rmob_orc_name_table[] = 
{
 
{"Rushnak"},	{"Vugor"},	{"Vragdish"},	/* 3 */
{"Prugdish"},	{"Vrork"},	{"Erilo"},	/* 6 */
{"Prik"},	{"Grodash"},	{"Vorg"},	/* 9 */
{"Vunk"},	{"Zhomhath"},   {"Mekshak"}, /* 12 */
{"Mugshak"},	{"Ratthug"},   {"Dreggrod"}, /* 15 */
{"Snagafang"},	{"Nargrim"},   {"Brubbog"}, /* 18 */
{"Tharburz"},	{"Shakgriig"},    /* 20 */
};
 
const struct rmob_orc_long_type rmob_orc_long_table[] = 
{
{"an orcish warrior stands here.\n\r"},	/* 1 */
{"a huge orcish warrior stands here.\n\r"}, /* 2 */
{"An orc patrols the area here.\n\r"}, /* 3 */
{"A strong-looking orcish warrior stands here.\n\r"},	/* 4 */
{"A medium-sized orc patrols the area here.\n\r"},	/* 5 */
{"A tusked orc warrior stands here.\n\r"},	/* 6 */
 
};
 
const struct rmob_orc_other_type rmob_orc_other_table[] = 
{
 
{-3,	3,	3,	5,	3,	5,	8,
AFF_DETECT_HIDDEN|AFF_INFRARED|AFF_BERSERK},
 
{-4,	4,	4,	6,	2,	8,	9,
AFF_DETECT_HIDDEN|AFF_INFRARED|AFF_BERSERK},
};
