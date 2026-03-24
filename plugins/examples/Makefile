.PHONY: adc config data gfx gpio hello http i2c info pollen screen sen55 servo sprite weather

COMPILE := zig cc -target wasm32-wasi -Wl,-z,stack-size=65536 -Wl,--initial-memory=262144 -Wl,--max-memory=262144 -Os
UPLOAD ?= 0
LAUNCH ?= 0

define build_task
$1:
	$$(COMPILE) -o ./$1/main.wasm ./$1/main.c
	airlab plugin bundle ./$1 ./$1/example.$1.alp
	@if [ "$$(UPLOAD)" = "1" ]; then airlab plugin upload ./$1/example.$1.alp; fi
	@if [ "$$(LAUNCH)" = "1" ]; then airlab plugin launch example.$1.alp; fi
endef

define build_task_screen
$1:
	$$(COMPILE) -o ./$1/main.wasm ./$1/main.c
	airlab plugin bundle ./$1 ./$1/example.$1.alp
	@if [ "$$(UPLOAD)" = "1" ]; then alpctl upload ./$1/example.$1.alp; fi
	@if [ "$$(LAUNCH)" = "1" ]; then alpctl -b screen launch example.$1.alp; fi
endef

all: adc config data gfx gpio hello http i2c info pollen screen sen55 servo sprite weather

$(foreach t,adc data config gfx gpio hello http i2c info pollen sen55 servo sprite weather,$(eval $(call build_task,$t)))
$(foreach t,screen,$(eval $(call build_task_screen,$t)))

upload:
	$(MAKE) UPLOAD=1 all
