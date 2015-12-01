WORKDIR=grammar
TESTDIR=test/grammar

test:sample-tests unittest

sample-tests:unittest test-ok-empty \
             test-err-tab-as-ind test-err-inv-chars test-err-bad-indent test-err-slice \
             test-ok-callable test-ok-slice test-ok-break test-ok-multi-level-lambdas \
             test-ok-block-pipe test-ok-mix

unittest:$(TESTDIR)/test-grammar.out
	$(CHKMEM) $(TESTDIR)/test-grammar.out

include $(WORKDIR)/Makefile

TEST_DEP=grammar phony-semantic.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-semantic.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-grammar.out:$(TEST_DEP) \
                            test-expr-nodes.dt \
                            test-regexp.dt \
                            test-stmt-nodes.dt \
                            test-clause.dt \
                            test-expr-automations.dt \
                            test-stmt-automations.dt \
                            test-clause-builder.dt \
                            test-anony-funcs.dt \
                            test-pipeline.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-expr-nodes.o \
	        $(TESTDIR)/test-regexp.o \
	        $(TESTDIR)/test-stmt-nodes.o \
	        $(TESTDIR)/test-clause.o \
	        $(TESTDIR)/test-expr-automations.o \
	        $(TESTDIR)/test-stmt-automations.o \
	        $(TESTDIR)/test-clause-builder.o \
	        $(TESTDIR)/test-anony-funcs.o \
	        $(TESTDIR)/test-pipeline.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-grammar.out

test-ok-%:$(TEST_DEP) test-%.dt $(TESTDIR)/%.fls
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-$*.o $(TEST_LIBS) -o $(TESTDIR)/test-$*.out
	$(CHKMEM) $(TESTDIR)/test-$*.out < $(TESTDIR)/$*.fls

test-err-%:$(TEST_DEP) test-err-%.dt $(ERRSAMPLEDIR)/%.fls
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-err-$*.o $(TEST_LIBS) -o $(TESTDIR)/test-err-$*.out
	$(CHKMEM) $(TESTDIR)/test-err-$*.out < $(ERRSAMPLEDIR)/$*.fls

cleant:
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
