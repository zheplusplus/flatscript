WORKDIR=output

include misc/mf-template.mk

output:block.d \
       stmt-nodes.d \
       expr-nodes.d \
       list-pipe.d \
       methods.d \
       name-mangler.d \
       function-impl.d \
       class.d \
       global.d

clean:
	rm -f $(WORKDIR)/*.o
