default :

install-clean :
	rm -rf ./verif-tools/verilog2smv-1.1.2 ./verif-tools/verilog2smv.sh	

install :
	cd verif-tools ; \
	tar -vxjf verilog2smv-1.1.2.tar.bz2 ; \
	cd verilog2smv-1.1.2 ; \
	sh build.sh ; \
	cp verilog2smv.sh ../

mem :
	sh build/run "application" true

run :
	sh build/run "application" false

verify :
	sh verif-tools/run

clean :
	rm -rf simv *.mem pmem.inc stimulus.v tmp-build
