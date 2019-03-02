CC = gcc
TARGETS = $(basename $(wildcard *.cm))

.SILENT: cmc clean all
.PHONY: clean all

all: $(TARGETS)

%: %.s
	gcc $< -o $@
%.s: %.cm ../cmc
	../cmc $< -o $@

clean:
	rm -f *.s
	rm -f $(TARGETS)

../cmc:
	$(MAKE) -C ..
