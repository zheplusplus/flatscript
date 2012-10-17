WORKDIR=parser
TESTDIR=$(WORKDIR)/test

TEST_DEP=parser phony-grammar.dt test-common.dt
TEST_OBJ=$(WORKDIR)/*.o $(TESTDIR)/phony-grammar.o $(TESTDIR)/test-common.o

runtest:test-empty test-err-tab-as-ind test-err-inv-chars test-err-bad-indent test-mix

include $(WORKDIR)/Makefile

test-empty:$(TESTDIR)/test-empty.out $(SAMPLEDIR)/empty.stkn
	$(CHEKC_MEMONRY) $(TESTDIR)/test-empty.out < $(SAMPLEDIR)/empty.stkn

$(TESTDIR)/test-empty.out:$(TEST_DEP) test-empty.dt
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-empty.o $(TEST_LIBS) -o $(TESTDIR)/test-empty.out

test-err-tab-as-ind:$(TESTDIR)/test-err-tab-as-ind.out $(ERRSAMPLEDIR)/tab-as-ind.stkn
	$(CHEKC_MEMONRY) $(TESTDIR)/test-err-tab-as-ind.out < $(ERRSAMPLEDIR)/tab-as-ind.stkn

$(TESTDIR)/test-err-tab-as-ind.out:$(TEST_DEP) test-err-tab-as-ind.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-err-tab-as-ind.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-err-tab-as-ind.out

test-err-inv-chars:$(TESTDIR)/test-err-inv-chars.out $(ERRSAMPLEDIR)/inv-chars.stkn
	$(CHEKC_MEMONRY) $(TESTDIR)/test-err-inv-chars.out < $(ERRSAMPLEDIR)/inv-chars.stkn

$(TESTDIR)/test-err-inv-chars.out:$(TEST_DEP) test-err-inv-chars.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-err-inv-chars.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-err-inv-chars.out

test-err-bad-indent:$(TESTDIR)/test-err-bad-indent.out $(ERRSAMPLEDIR)/bad-indent.stkn
	$(CHEKC_MEMONRY) $(TESTDIR)/test-err-bad-indent.out < $(ERRSAMPLEDIR)/bad-indent.stkn

$(TESTDIR)/test-err-bad-indent.out:$(TEST_DEP) test-err-bad-indent.dt
	$(LINK) $(TEST_OBJ) \
	        $(TESTDIR)/test-err-bad-indent.o \
	        $(TEST_LIBS) \
	     -o $(TESTDIR)/test-err-bad-indent.out

test-mix:$(TESTDIR)/test-mix.out $(SAMPLEDIR)/mix.stkn
	$(CHEKC_MEMONRY) $(TESTDIR)/test-mix.out < $(SAMPLEDIR)/mix.stkn

$(TESTDIR)/test-mix.out:$(TEST_DEP) test-mix.dt
	$(LINK) $(TEST_OBJ) $(TESTDIR)/test-mix.o $(TEST_LIBS) -o $(TESTDIR)/test-mix.out

cleant:clean
	rm -f $(TESTDIR)/*.o
	rm -f $(TESTDIR)/*.out
