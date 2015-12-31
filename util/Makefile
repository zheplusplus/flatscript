WORKDIR=util

include misc/mf-template.mk

util:uid.d str-comprehension.d string.d
	$(AR) $(LIB_DIR)/libstkn.a $(WORKDIR)/*.o

clean:
	rm -f $(WORKDIR)/*.o
