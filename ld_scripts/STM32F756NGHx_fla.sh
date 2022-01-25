#!/bin/sh

openocd -f interface/stlink.cfg -f target/stm32f7x.cfg -c "init" -c "reset init" -c "flash write_image erase $1" -c "reset run" -c "shutdown"
