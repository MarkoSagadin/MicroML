file BUILD/mbed.elf
target remote localhost:3333
tui enable
mon reset halt

define reset
	mon reset halt
end
