CONTRACTS = examples/increment examples/token

all:
	@for dir in $(CONTRACTS); do $(MAKE) -C $$dir; done

clean:
	@for dir in $(CONTRACTS); do $(MAKE) -C $$dir clean; done

.PHONY: all clean
