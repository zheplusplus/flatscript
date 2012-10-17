WORKDIR=misc

include misc/mf-template.mk

misc:pos-type.d platform.d
	$(AR) $(LIB_DIR)/libstkn.a $(WORKDIR)/*.o

clean:
	rm -f $(WORKDIR)/*.o
