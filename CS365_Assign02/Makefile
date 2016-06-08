CFLAGS = -g -Wall -std=gnu99 -D_REENTRANT -I/usr/include/openmpi

SUBMIT_URL = https://cs.ycp.edu/marmoset/bluej/SubmitProjectViaBlueJSubmitter
COURSE_NAME = CS 365
SEMESTER = Spring 2015

TESTSRCS = testgrid.c tctest.c
SRCS = grid.c life.c
OBJS = $(SRCS:%.c=%.o)
MAINSRCS = testgrid.c life_seq.c life_par.c
PROGS = $(MAINSRCS:%.c=%)

test% : test%.o $(OBJS)
	$(CC) -o $@ test$*.o $(OBJS) tctest.o

all : $(PROGS)

testgrid : testgrid.o grid.o tctest.o

life_seq : life_seq.o $(OBJS)
	$(CC) -o $@ life_seq.o $(OBJS)

life_par : life_par.o $(OBJS)
	$(CC) -o $@ life_par.o $(OBJS) -lmpi

submit :
	@echo "Please run 'make submit_ms1' or 'make submit_ms2'"

submit_ms1 :
	PROJECT_NUMBER=assign02_ms1 $(MAKE) _submit

submit_ms2 :
	PROJECT_NUMBER=assign02_ms2 $(MAKE) _submit

_submit : submit.properties solution.zip
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
				-or -name '*~' \
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
	rm -f *.o $(PROGS) depend.mak submit.properties solution.zip

depend :
	$(CC) $(CFLAGS) -M $(SRCS) $(TESTSRCS) $(MAINSRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
