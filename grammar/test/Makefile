WORKDIR=grammar
TESTDIR=$(WORKDIR)/test

test:$(TESTDIR)/test-grammar.out
	$(CHEKC_MEMONRY) $(TESTDIR)/test-grammar.out

include $(WORKDIR)/Makefile

TEST_DEP=grammar phony-flowcheck.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-flowcheck.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-grammar.out:$(TEST_DEP) \
                            test-stmt-nodes.dt \
                            test-expr-nodes.dt \
                            test-acceptor.dt \
                            test-clause-builder.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-stmt-nodes.o \
	        $(TESTDIR)/test-expr-nodes.o \
	        $(TESTDIR)/test-acceptor.o \
	        $(TESTDIR)/test-clause-builder.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-grammar.out

cleant:clean
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
