WORKDIR=flowcheck
TESTDIR=$(WORKDIR)/test

test:$(TESTDIR)/test-flowcheck.out
	$(CHEKC_MEMONRY) $(TESTDIR)/test-flowcheck.out

include $(WORKDIR)/Makefile

TEST_DEP=flowcheck phony-proto.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-proto.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-flowcheck.out:$(TEST_DEP) \
                              test-expr-nodes.dt \
                              test-symbol-table.dt \
                              test-filter.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-expr-nodes.o \
	        $(TESTDIR)/test-symbol-table.o \
	        $(TESTDIR)/test-filter.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-flowcheck.out

cleant:clean
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
