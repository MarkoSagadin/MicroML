target remote localhost:3333
tui enable
file build/firmware.elf
mon reset halt


define reset
	mon reset halt
end
