WORKDIR=semantic

include misc/mf-template.mk

semantic:$(WORKDIR)/const-fold.h $(WORKDIR)/const-fold.cpp \
         stmt-nodes.d \
         expr-nodes.d \
         list-pipe.d \
         const-fold.d \
         block.d \
         compiling-space.d \
         class.d \
         function.d

$(WORKDIR)/const-fold.%:$(CODEGEN)/const-fold.py
	$(PYTHON) $(CODEGEN)/const-fold.py

clean:
	rm -f $(WORKDIR)/*.o
	rm -f semantic/const-fold.*
