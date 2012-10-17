WORKDIR=util

include misc/mf-template.mk

util:pointer.d str-comprehension.d string.d sn.d
	$(AR) $(LIB_DIR)/libstkn.a $(WORKDIR)/*.o

clean:
	rm -f $(WORKDIR)/*.o

