WORKDIR=output

include misc/mf-template.mk

output:block.d \
       stmt-nodes.d \
       expr-nodes.d \
       list-pipe.d \
       methods.d \
       name-mangler.d \
       function.d \
       global.d

clean:
	rm -f $(WORKDIR)/*.o
