all:
	(cd TME2; $(MAKE) all)
	(cd TME3; $(MAKE) all)
	(cd TME4; $(MAKE) all)
	(cd TME5; $(MAKE) all)

proper:
	(cd TME2; $(MAKE) proper)
	(cd TME3; $(MAKE) proper)
	(cd TME4; $(MAKE) proper)
	(cd TME5; $(MAKE) proper)

clean:
	(cd TME2; $(MAKE) clean)
	(cd TME3; $(MAKE) clean)
	(cd TME4; $(MAKE) clean)
	(cd TME5; $(MAKE) clean)

bench:
	(cd TME3; $(MAKE) bench)
	(cd TME4; $(MAKE) bench)
	(cd TME5; $(MAKE) bench)

.PHONY: all proper clean bench
