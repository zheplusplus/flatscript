WORKDIR=util
TESTDIR=$(WORKDIR)/test

include misc/mf-template.mk

test:$(TESTDIR)/test-utilities.out
	$(CHKMEM) $(TESTDIR)/test-utilities.out

include $(WORKDIR)/Makefile

$(TESTDIR)/test-utilities.out:util \
                              test-string.dt \
                              test-pointer.dt \
                              test-arrays.dt
	$(LINK) $(TESTDIR)/test-*.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-utilities.out

cleant:
	rm -f $(UTILDIR)/*.o
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
