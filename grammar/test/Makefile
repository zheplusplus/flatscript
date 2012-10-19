WORKDIR=grammar
TESTDIR=$(WORKDIR)/test

test:parsing-tests $(TESTDIR)/test-grammar.out
	$(CHKMEM) $(TESTDIR)/test-grammar.out

parsing-tests:test-ok-empty test-err-tab-as-ind test-err-inv-chars test-err-bad-indent \
              test-err-slice test-err-modify-name \
              test-ok-mix test-ok-callable test-ok-slice test-ok-break

include $(WORKDIR)/Makefile

TEST_DEP=grammar phony-flowcheck.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-flowcheck.o $(TESTDIR)/test-common.o

$(TESTDIR)/test-grammar.out:$(TEST_DEP) \
                            test-stmt-nodes.dt \
                            test-acceptor.dt \
                            test-clause-builder.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-stmt-nodes.o \
	        $(TESTDIR)/test-acceptor.o \
	        $(TESTDIR)/test-clause-builder.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-grammar.out

test-ok-%:$(TEST_DEP) test-%.dt $(TESTDIR)/%.stkn
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-$*.o $(TEST_LIBS) -o $(TESTDIR)/test-$*.out
	$(CHKMEM) $(TESTDIR)/test-$*.out < $(TESTDIR)/$*.stkn

test-err-%:$(TEST_DEP) test-err-%.dt $(ERRSAMPLEDIR)/%.stkn
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-err-$*.o $(TEST_LIBS) -o $(TESTDIR)/test-err-$*.out
	$(CHKMEM) $(TESTDIR)/test-err-$*.out < $(ERRSAMPLEDIR)/$*.stkn

cleant:clean
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
