WORKDIR=inspect

include misc/mf-template.mk

ifeq ($(MODE), inspect)
	INSPECT_OBJ=trace.o
else
	INSPECT_OBJ=not-trace.o
endif

inspect:trace.d not-trace.d
	$(AR) $(LIB_DIR)/libtrace.a $(WORKDIR)/$(INSPECT_OBJ)

clean:
	rm -f $(WORKDIR)/*.o
