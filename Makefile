all:
	gcc w6.c -ow6 -Ofast
run:
	./w6 input.txt
bench:
	@echo "Running benchmark this might take awhile..."
	perf stat -r 100000 -d ./w6 input.txt > /dev/null