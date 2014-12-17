#####################################################################################################
## Updated Makefile by David Simmerson (Omega)
## Fundamental changes: now supports automated revision
## automated backup of successful compiles, so you can restore back to older builds if necessary
##
#####################################################################################################
## Now use wildcards, all .c files will be compiled if located within the source directory
## further futureproofing of the Entity known as Dungeonworld.
#####################################################################################################

CC	= gcc
PROF	= -O -ggdb
NOCRYPT	=
C_FLAGS	= -Wall $(PROF) $(NOCRYPT)
L_FLAGS	= $(PROF) -DASSERT
INCLUDE_FLAGS	=
LIBRARIES	= -lcrypt

#####################################################################################################
# VERSION SUPPORT: allow us to control our revisions of the executable being built.
# name of our version file
VERSION_FILE		= .version

# Ensure we are built properly with the correct data
BUILD_DATE		= $(shell date +'%Y%m%d')
BUILD_NUM		= $(shell cat $(VERSION_FILE))

# define our version (within the compiler flags)
C_FLAGS	+= -DBUILD_DATE=$(BUILD_DATE) -DBUILD_NUM=$(BUILD_NUM)

# define our backup filename: this is broken down into multiple blocks as there was an issue
# with using long names / groups : this error might have been compiler specific, but this was the
# fix so to retain functionality of the backup, this is our goal.
BACKUP_NAME_PT1		= "backup-$(BUILD_DATE)"
BACKUP_NAME_PT2		= "-$(BUILD_NUM).tgz"
# put both groups together in our final attempt to create a unified backup
BACKUP_NAME		= "$(BACKUP_NAME_PT1)$(BACKUP_NAME_PT2)"

C_FILES			= ${wildcard *.c}
OBJECT_FILES		= $(patsubst %.c, %.o, ${C_FILES})

ENGINE	= rom
BINDIR	= ../area/

all: version ${ENGINE}

#####################################################################################################
# Generate our engine for execution, this will build, and force the system into the next steps listed
# below in the .c.o section.  Note that this is epic build state.
.PHONY: $(ENGINE)
${ENGINE}: $(OBJECT_FILES)
		rm -f ${ENGINE}
		$(CC) $(L_FLAGS) -o rom $(OBJECT_FILES) ${LIBRARIES}
		@rm $(BINDIR)/$(ENGINE)
		@cp -p $(ENGINE) $(BINDIR)
		@make backup # Force the Makefile to backup ALL successfully completed builds (failed don't backup)
		@echo "Compiling completed for build $(BUILD_NUM) at $(shell date +"%y-%m-%d @ %X") and copied rom TO ../area."
######################################################################################################

######################################################################################################
# Generate our .o (object) files based off our .cpp files (dependant upon Core.hpp) this will allow
# us to actually compile our executable.
.PHONY: .c.o
%.o: %.c
		@echo "$(shell date +%X)  >>  Building $<..."
		@$(CC) ${INCLUDE_FLAGS} -c $(C_FLAGS) $<
######################################################################################################

######################################################################################################
# Generate a backup of the source code for today's date and move it to the backup folder!
.PHONY: backup
backup:
		@tar -czf $(BACKUP_NAME) *.c *.h Makefile .version
		@mv *.tgz ../
		@echo "Backup completed."
######################################################################################################

######################################################################################################
# Generate our version file (generates a .version file, and a .hpp file)
.PHONY: version
version:
		@if ! test -f $(VERSION_FILE); then echo 0 > $(VERSION_FILE); fi
		@echo $$(($$(cat $(VERSION_FILE)) +1)) > $(VERSION_FILE)
		@echo "#ifndef __Version_Hpp" > version.h
		@echo "#define __Version_hpp" >> version.h
		@echo "const unsigned long mudVersion = $(shell cat $(VERSION_FILE))+1;" >> version.h
		@echo "#endif" >> version.h
		@echo "Revision control completed!"
######################################################################################################

######################################################################################################
# Clean out our folder of any/all unnecessary files, allowing us to ensure our directory is clean.
clean:
		@rm -f *.o
		@echo ".o files removed from directory"
		make
######################################################################################################

######################################################################################################
# Wipes the directory clean of .o files and the executibles.  Helps ensure a clean directory.
wipe:
		@rm -f *.o
		@rm -f rom.exe
		@echo "Dungeon World is wiped."
######################################################################################################
