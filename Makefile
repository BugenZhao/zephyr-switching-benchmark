all:
	west build -p auto -b rpi_pico .

emu:
	west build --build-dir build-emu -p auto -b qemu_cortex_a53 .

emu-run: emu
	west build --build-dir build-emu -t run
