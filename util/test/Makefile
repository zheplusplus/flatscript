WORKDIR=util
TESTDIR=$(WORKDIR)/test

include misc/mf-template.mk

test:$(TESTDIR)/test-utilities.out
	$(CHKMEM) $(TESTDIR)/test-utilities.out

include $(WORKDIR)/Makefile

$(TESTDIR)/test-utilities.out:util \
                              test-map-compare.dt \
                              test-string.dt \
                              test-pointer.dt \
                              test-vector-append.dt
	$(LINK) $(TESTDIR)/test-map-compare.o \
	        $(TESTDIR)/test-string.o \
	        $(TESTDIR)/test-pointer.o \
	        $(TESTDIR)/test-vector-append.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-utilities.out

cleant:
	rm -f $(UTILDIR)/*.o
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
