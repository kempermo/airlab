dat:
	clang-format -i dat.c
	rm -rf ./fs; mkdir -p ./fs
	clang -DDAT_TEST -o dat ../src/dat.c dat.c
	./dat; rm ./dat
