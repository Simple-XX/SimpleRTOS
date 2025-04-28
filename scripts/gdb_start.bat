
@echo on

set project=%1

if "%1" == "" (
    echo "ERROR: no project specified!"
    echo "usage: gdb_start.bat [PROJECT_NAME]"
    exit
)

arm-none-eabi-gdb.exe output/%project%/%project%.elf
