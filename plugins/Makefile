.PHONY: gfx hello gpio sen55 http

COMPILE := zig cc -target wasm32-wasi -Wl,-z,stack-size=65536 -Wl,--initial-memory=262144 -Wl,--max-memory=262144 -Os

gfx:
	$(COMPILE) -o ./gfx/main.wasm ./gfx/main.c
	alpctl bundle ./gfx plugin.alp
	alpctl upload plugin.alp

hello:
	$(COMPILE) -o ./hello/main.wasm ./hello/main.c
	alpctl bundle ./hello plugin.alp
	alpctl upload plugin.alp

gpio:
	$(COMPILE) -o ./gpio/main.wasm ./gpio/main.c
	alpctl bundle ./gpio plugin.alp
	alpctl upload plugin.alp

sen55:
	$(COMPILE) -o ./sen55/main.wasm ./sen55/main.c
	alpctl bundle ./sen55 plugin.alp
	alpctl upload plugin.alp

http:
	$(COMPILE) -o ./http/main.wasm ./http/main.c
	alpctl bundle ./http plugin.alp
	alpctl upload plugin.alp

fmt:
	clang-format -i al.h **/*.c -style="{BasedOnStyle: Google, ColumnLimit: 120, SortIncludes: false}"
