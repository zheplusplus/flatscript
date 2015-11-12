WORKDIR=.

ifndef INSTALL_DIR
	INSTALL_DIR=/usr/local/bin
endif

include misc/mf-template.mk

all:code-gen main.d env.d globals.d lib
	make -f report/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f grammar/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f semantic/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f output/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	$(LINK) *.o \
	        report/*.o \
	        grammar/*.o \
	        semantic/*.o \
	        output/*.o \
	        $(LIBS) \
	     -o flsc

install:all
	@install flsc $(INSTALL_DIR)/flsc || echo "Fail to install to $(INSTALL_DIR)."\
		"Try specifying installation path manully by passing INSTALL_DIR=directory"

uninstall:
	rm -f $(INSTALL_DIR)/flsc

code-gen:
	make -f codegen/Makefile PYTHON=$(PYTHON)

lib:
	mkdir -p libs
	make -f util/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f misc/Makefile MODE=$(MODE) COMPILER=$(COMPILER)

runtest:all test-lib
	make -f test/util/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f test/grammar/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f test/semantic/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	./flsc -e require -i test/sample-test.fls | node

test-lib:code-gen
	mkdir -p libs
	make -f test/Makefile MODE=$(MODE) COMPILER=$(COMPILER)

clean:
	make -f util/Makefile clean
	make -f misc/Makefile clean
	make -f report/Makefile clean
	make -f grammar/Makefile clean
	make -f semantic/Makefile clean
	make -f output/Makefile clean
	make -f codegen/Makefile clean
	make -f test/Makefile clean
	make -f test/util/Makefile cleant
	make -f test/grammar/Makefile cleant
	make -f test/semantic/Makefile cleant
	find -type f -name "*.js" -exec rm {} \;
	find -type f -name "*.o" -exec rm {} \;
	find -type f -name "*.out" -exec rm {} \;
	find -type f -name "tmp.*" -exec rm {} \;
	rm -rf $(LIB_DIR)
	rm -f flsc
