WORKDIR=test

include misc/mf-template.mk

test-commons:phony-errors.d common.d
	$(AR) $(LIB_DIR)/libstkntest.a $(WORKDIR)/*.o globals.o

clean:
	rm -f $(WORKDIR)/*.o
