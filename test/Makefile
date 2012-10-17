WORKDIR=test

include misc/mf-template.mk

test-commons:phony-errors.d phony-warnings.d common.d
	$(AR) $(LIB_DIR)/libstkntest.a test/*.o

clean:
	rm -f $(WORKDIR)/*.o
