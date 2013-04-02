CC      = gcc
PROF    = -O -ggdb
NOCRYPT =
C_FLAGS =  -Wall $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)

O_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o \
	act_wiz.o alias.o ban.o bit.o comm.o const.o db.o db2.o \
	effects.o fight.o flags.o handler.o healer.o hedit.o \
	interp.o lookup.o magic.o magic2.o mem.o mob_cmds.o \
	mob_prog.o music.o note.o olc.o olc_act.o olc_mpcode.o \
	olc_save.o protocol.o recycle.o save.o scan.o skills.o \
	special.o string.o tables.o update.o wizlist.o -lcrypt

rom: $(O_FILES)
	rm -f rom
	rm -f rom.exe
	$(CC) $(L_FLAGS) -o rom $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

####################################################################################################
# clean out our folder of any/all un-necessary files allowing us to ensure our directory is clean.
clean:
	@rm -f *.o
	@echo ".o files removed from directory."
	make
####################################################################################################

####################################################################################################
# Wipes the directory clean of .o files and the executibles.  Helps ensure a clean directory.
wipe:
	@rm -f *.o rom
	@rm -f rom.exe
	@echo "Dungeon World is wiped."
####################################################################################################


####################################################################################################
# Grab our u-name for giggles; used later on to detect possible support
# for the game engine
UNAME = $(shell uname -s)
####################################################################################################