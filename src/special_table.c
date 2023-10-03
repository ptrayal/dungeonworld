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
 
 
#include <sys/types.h>
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
    {"an orcish warrior stands here.\n\r"}, /* 1 */
    {"a huge orcish warrior stands here.\n\r"}, /* 2 */
    {"An orc patrols the area here.\n\r"}, /* 3 */
    {"A strong-looking orcish warrior stands here.\n\r"}, /* 4 */
    {"A medium-sized orc patrols the area here.\n\r"}, /* 5 */
    {"A tusked orc warrior stands here.\n\r"}, /* 6 */
    {"A battle-scarred orc veteran stands defiant.\n\r"}, /* 7 */
    {"An orc raider prowls the area hungrily.\n\r"}, /* 8 */
    {"A scar-faced orc raider eyes you suspiciously.\n\r"}, /* 9 */
    {"A grizzled orc mercenary sharpens a blade.\n\r"}, /* 10 */
    {"A bloodthirsty orc marauder seeks prey.\n\r"}, /* 11 */
    {"A hulking orc berserker looms menacingly.\n\r"}, /* 12 */
    {"A cunning orcish warlord surveys the surroundings.\n\r"}, /* 13 */
    {"An orcish chieftain stands here, commanding respect.\n\r"}, /* 14 */
    {"A one-eyed orc brute flexes his muscles aggressively.\n\r"}, /* 15 */
    {"A swift and agile orc assassin lurks in the shadows.\n\r"}, /* 16 */
    {"A relentless orc berserker charges at you ferociously.\n\r"}, /* 17 */
    {"An orcish gladiator brandishes a gleaming weapon.\n\r"}, /* 18 */
    {"An of orcish soldiers forms a defensive stance.\n\r"}, /* 19 */
    {"A fearsome orc warlord leads his horde with an iron fist.\n\r"}, /* 20 */
};
 
const struct rmob_orc_other_type rmob_orc_other_table[] = 
{
 
{-3,	3,	3,	5,	3,	5,	8,
AFF_INFRARED},
 
{-4,	4,	4,	6,	2,	8,	9,
AFF_INFRARED},
};

