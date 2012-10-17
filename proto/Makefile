WORKDIR=proto

include misc/mf-template.mk

proto:block.d \
      node-base.d \
      stmt-nodes.d \
      expr-nodes.d \
      list-pipe.d \
      operation.d \
      function.d

clean:
	rm -f $(WORKDIR)/*.o
