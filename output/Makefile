WORKDIR=output

include misc/mf-template.mk

output:block.d \
       stmt-nodes.d \
       expr-nodes.d \
       list-pipe.d \
       name-mangler.d \
       function.d

clean:
	rm -f $(WORKDIR)/*.o
