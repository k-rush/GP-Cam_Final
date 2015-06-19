LEVEL = ../

include makeinclude

#########################

LOCAL_FILES = EVILib.o\
	EVI-D70g.o\
	EVI-D70.o\
	EVI-D70P.o\
	EVILib.a\
	eviDriver.o

#########################

ALL = $(LOCAL_FILES)

main: $(ALL)

all: $(ALL)

#########################

EVILib.o: EVILib.c++ EVILib.h EVI-D70g.o
	@echo Compiling $@
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ -c $<

EVI-D70g.o: EVI-D70g.c++ EVI-D70g.h EVI-D70.o EVI-D70P.o
	@echo Compiling $@
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ -c $<

EVI-D70.o: EVI-D70.c++ EVI-D70.h
	@echo Compiling $@
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ -c $<

EVI-D70P.o: EVI-D70P.c++ EVI-D70P.h 
	@echo Compiling $@
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ -c $<

eviDriver.o: eviDriver.cpp eviDriver.h
	@echo Compiling $@
	$(CC) $(OPTIONS) $(INCLUDES) -o $@ -c $<
	#g++ -c -fPIC -o eviDriver.o eviDriver.cpp 

EVILib_Files = EVILib.o EVI-D70g.o EVI-D70.o EVI-D70P.o eviDriver.o
EVILib.a : $(EVILib_Files)
	@echo == making $@ ==
	$(AR) -rc $@ $(EVILib_Files);
#########################

clean:
	-$(RM) *~
	rm *.o

rclean:
	$(MAKE) clean
	@for files in $(LOCAL_FILES); do\
		($(RM) $$files);\
	done

clock:
	$(TOUCH) *
