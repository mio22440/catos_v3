
@echo on

set project=%1

if "%1" == "" (
    echo "ERROR: no project specified!"
    echo "usage: gdb_start.bat [PROJECT_NAME]"
    exit
)

E:/code/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gdb.exe output/%project%/%project%.elf
