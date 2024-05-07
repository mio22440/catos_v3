
@echo off

set project=%1

if "%1" == "" (
    echo "ERROR: no project specified!"
    echo "usage: gdb_start.bat [PROJECT_NAME]"
    exit
)

openocd^
 -f interface/cmsis-dap.cfg^
 -f target/stm32f1x.cfg^
 -c init^
 -c halt^
 -c "flash write_image erase output/%project%/%project%.elf"^
 -c reset^
 -c shutdown



