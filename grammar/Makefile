WORKDIR=grammar

include misc/mf-template.mk

grammar:tokenize.d syntax.d tokens.d automation-base.d stmt-automations.d \
        node-base.d stmt-nodes.d expr-nodes.d expr-automations.d function.d \
        block.d reducing-env.d clause-builder.d clauses.d class.d

clean:
	rm -f $(WORKDIR)/*.o
