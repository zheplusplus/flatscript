WORKDIR=grammar
TESTDIR=test/grammar

test:$(TESTDIR)/test-grammar.out
	$(CHKMEM) $(TESTDIR)/test-grammar.out

include $(WORKDIR)/Makefile

TEST_DEP=grammar phony-semantic.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-semantic.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-grammar.out: $(TEST_DEP) \
            test-tokenize.dt \
            test-syntax.dt \
            test-expr-nodes.dt \
            test-regexp.dt \
            test-stmt-nodes.dt \
            test-clause.dt \
            test-expr-automations.dt \
            test-stmt-automations.dt \
            test-clause-builder.dt \
            test-anony-funcs.dt \
            test-pipeline.dt \
            test-empty.dt \
            test-callable.dt \
            test-slice.dt \
            test-break.dt \
            test-multi-level-lambdas.dt \
            test-block-pipe.dt \
            test-mix.dt \
            test-samples.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-tokenize.o \
	        $(TESTDIR)/test-syntax.o \
	        $(TESTDIR)/test-expr-nodes.o \
	        $(TESTDIR)/test-regexp.o \
	        $(TESTDIR)/test-stmt-nodes.o \
	        $(TESTDIR)/test-clause.o \
	        $(TESTDIR)/test-expr-automations.o \
	        $(TESTDIR)/test-stmt-automations.o \
	        $(TESTDIR)/test-clause-builder.o \
	        $(TESTDIR)/test-anony-funcs.o \
	        $(TESTDIR)/test-pipeline.o \
	        $(TESTDIR)/test-empty.o \
	        $(TESTDIR)/test-callable.o \
	        $(TESTDIR)/test-slice.o \
	        $(TESTDIR)/test-break.o \
	        $(TESTDIR)/test-multi-level-lambdas.o \
	        $(TESTDIR)/test-block-pipe.o \
	        $(TESTDIR)/test-mix.o \
	        $(TESTDIR)/test-samples.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-grammar.out

cleant:
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
