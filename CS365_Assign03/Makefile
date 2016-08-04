SRCS = nbody_seq.c nbody_par.c mtqueue.c
PROGS = nbody_seq nbody_par

CFLAGS = -g -Wall -std=gnu99 -D_REENTRANT

SUBMIT_URL = https://cs.ycp.edu/marmoset/bluej/SubmitProjectViaBlueJSubmitter
COURSE_NAME = CS 365
SEMESTER = Spring 2015
PROJECT_NUMBER = assign03

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all : $(PROGS)

nbody_seq : nbody_seq.o
	$(CC) -o nbody_seq nbody_seq.o -lforms -lpthread -lm

nbody_par : nbody_par.o mtqueue.o
	$(CC) -o nbody_par nbody_par.o mtqueue.o -lforms -lpthread -lm

submit : submit.properties solution.zip
	perl submitToMarmoset.pl solution.zip submit.properties

solution.zip : collected-files.txt
	@echo "Creating a solution zip file"
	@zip -9 $@ `cat collected-files.txt`
	@rm -f collected-files.txt

# Create the submit.properties file that describes how
# the project should be uploaded to the Marmoset server.
submit.properties : nonexistent
	@echo "Creating submit.properties file"
	@rm -f $@
	@echo "submitURL = $(SUBMIT_URL)" >> $@
	@echo "courseName = $(COURSE_NAME)" >> $@
	@echo "semester = $(SEMESTER)" >> $@
	@echo "projectNumber = $(PROJECT_NUMBER)" >> $@

# Collect the names of all files that don't appear
# to be generated files.
collected-files.txt :
	@echo "Collecting the names of files to be submitted"
	@rm -f $@
	@find . -not \( \
				-name '*\.o' \
				-or -name '*\.exe' \
				-or -name 'nbody_seq' \
				-or -name 'nbody_par' \
				-or -name '*~' \
				-or -name 'depend\.mak' \
				-or -name 'collected-files.txt' \
				-or -name 'submit.properties' \
				-or -type d \
			\) -print \
		| perl -ne 's,\./,,; print' \
		> $@

# This is just a dummy target to force other targets
# to always be out of date.
nonexistent :

clean :
	rm -f nbody nbody_par nbody_seq *.o *~ depend.mak

depend.mak :
	touch $@

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

include depend.mak
