WORKDIR=.

include misc/mf-template.mk

all:stekin.d lib
	make -f report/Makefile MODE=$(MODE)
	make -f grammar/Makefile MODE=$(MODE)
	make -f flowcheck/Makefile MODE=$(MODE)
	make -f proto/Makefile MODE=$(MODE)
	$(LINK) stekin.o \
	        report/*.o \
	        grammar/*.o \
	        flowcheck/*.o \
	        proto/*.o \
	        $(LIBS) \
	     -o stekin

lib:
	mkdir -p libs
	make -f util/Makefile MODE=$(MODE)
	make -f misc/Makefile MODE=$(MODE)

runtest:all test-lib
	make -f util/test/Makefile MODE=$(MODE)
	make -f grammar/test/Makefile MODE=$(MODE)
	make -f flowcheck/test/Makefile MODE=$(MODE)
	bash test/sample-test.sh -cm

test-lib:
	mkdir -p libs
	make -f test/Makefile MODE=$(MODE)

clean:
	make -f util/Makefile clean
	make -f misc/Makefile clean
	make -f report/Makefile clean
	make -f grammar/Makefile clean
	make -f flowcheck/Makefile clean
	make -f proto/Makefile clean
	rm -f tmp.*
	rm -f *.o
	rm -f *.out
	rm -rf $(LIB_DIR)
	rm -f stekin

cleant:clean
	make -f test/Makefile clean
	make -f util/test/Makefile cleant
	make -f grammar/test/Makefile cleant
	make -f flowcheck/test/Makefile cleant
