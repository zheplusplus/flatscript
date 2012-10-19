WORKDIR=grammar
AWK=awk

include misc/mf-template.mk

grammar:yy-misc.d syntax-types.d lexical syntactically stmt-nodes.d function.d \
        block.d clause-builder.d acceptor.d

syntactically:
	$(RESOLVE_DEP) $(WORKDIR)/syn-include.h | $(AWK) -f $(WORKDIR)/syn-script > $(MKTMP)
	make -f $(MKTMP)

lexical:syntactically
	$(RESOLVE_DEP) $(WORKDIR)/lex-include.h | $(AWK) -f $(WORKDIR)/lex-script > $(MKTMP)
	make -f $(MKTMP)

clean:
	rm -f $(WORKDIR)/syn.cpp
	rm -f $(WORKDIR)/syn.hpp
	rm -f $(WORKDIR)/lex.cpp
	rm -f $(WORKDIR)/*.o
