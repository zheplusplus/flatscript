WORKDIR=proto

include misc/mf-template.mk

proto:block.d \
      stmt-nodes.d \
      expr-nodes.d \
      list-pipe.d \
      name-mangler.d \
      operation.d \
      function.d

clean:
	rm -f $(WORKDIR)/*.o
