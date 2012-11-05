WORKDIR=codegen

include misc/mf-template.mk

codegen:report/errors.h report/errors.cpp test/phony-errors.h test/phony-errors.cpp

report/%:$(WORKDIR)/gen-report.py
	$(PYTHON) $(WORKDIR)/gen-report.py

test/%:$(WORKDIR)/gen-report.py
	$(PYTHON) $(WORKDIR)/gen-report.py

clean:
	rm -f $(WORKDIR)/*.pyc
	rm -f report/errors.*
	rm -f test/phony-errors.*
