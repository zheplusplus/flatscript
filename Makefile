WORKDIR=.

ifndef INSTALL_DIR
	INSTALL_DIR=/usr/bin
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
	     -o flatsc

install:all
	@install flatsc $(INSTALL_DIR)/flatsc || echo "Fail to install to $(INSTALL_DIR),"\
		"permission denied or directory not existed."\
		"Try specify installation path manully by passing INSTALL_DIR=directory"

uninstall:
	rm -f $(INSTALL_DIR)/flatsc

code-gen:
	make -f codegen/Makefile PYTHON=$(PYTHON)

lib:
	mkdir -p libs
	make -f util/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f misc/Makefile MODE=$(MODE) COMPILER=$(COMPILER)

runtest:all test-lib
	make -f util/test/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f grammar/test/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	make -f semantic/test/Makefile MODE=$(MODE) COMPILER=$(COMPILER)
	bash test/sample-test.sh -cm
	bash test/sample-report-test.sh -cm
	./flatsc -e require < test/sample-modules-test.fls | node

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
	find -type f -name "*.js" -exec rm {} \;
	find -type f -name "*.o" -exec rm {} \;
	find -type f -name "*.out" -exec rm {} \;
	find -type f -name "tmp.*" -exec rm {} \;
	rm -rf $(LIB_DIR)
	rm -f flatsc

cleant:clean
	make -f test/Makefile clean
	make -f util/test/Makefile cleant
	make -f grammar/test/Makefile cleant
	make -f semantic/test/Makefile cleant
