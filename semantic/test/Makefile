WORKDIR=semantic
TESTDIR=$(WORKDIR)/test

test:$(TESTDIR)/test-semantic.out
	$(CHKMEM) $(TESTDIR)/test-semantic.out

include $(WORKDIR)/Makefile

TEST_DEP=semantic phony-output.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-output.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-semantic.out:$(TEST_DEP) \
                             test-expr-nodes.dt \
                             test-symbol-table.dt \
                             test-filter.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-expr-nodes.o \
	        $(TESTDIR)/test-symbol-table.o \
	        $(TESTDIR)/test-filter.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-semantic.out

cleant:clean
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
