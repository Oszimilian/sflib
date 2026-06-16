EXAMPLES = model feedback submodel module

all:
	for d in $(EXAMPLES); do $(MAKE) -C examples/$$d; done

clean:
	for d in $(EXAMPLES); do $(MAKE) -C examples/$$d clean; done

.PHONY: all clean $(EXAMPLES)

$(EXAMPLES):
	$(MAKE) -C examples/$@
