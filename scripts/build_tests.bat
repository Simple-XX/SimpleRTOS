
:: 构建项目
:: example: build.bat demo

@echo off

:: 设置第一个参数为项目名称
set project=%1
@REM echo project=%project%

if "%1" == "" (
    echo "ERROR: no project specified!"
    echo "usage: build.bat [PROJECT_NAME]"
    exit
)

:: 生成的cmake项目目录存放的位置
set output_dir=output

:: 计算时间标签
set "current_date=%DATE%"
set "current_time=%TIME%"

for /F "tokens=1-3 delims=/ " %%A in ("%current_date%") do (
    set "year=%%A"
    set "month=%%B"
    set "day=%%C"
)

set HH=%current_time:~0,2%
set MM=%current_time:~3,2%
set SS=%current_time:~6,2%
if "%HH:~0,1%" == " " (
    set "HH=0%HH:~1,1%"
)
if "%MM:~0,1%" == " " (
    set "MM=0%MM:~1,1%"
)
if "%SS:~0,1%" == " " (
    set "SS=0%SS:~1,1%"
)

set build_time_tag=%year%%month%%day%_%HH%_%MM%_%SS%

:: cmake 生成的目录
set build_dir_name=%project%

:: 确保output目录存在
if not exist %output_dir% (
    echo %output_dir% folder do not exist, creating...
    mkdir %output_dir%
)
echo %output_dir% folder exists

:: 创建构建目录
cd %output_dir%

echo build_dir_name=%build_dir_name%

if not exist %build_dir_name% (
    echo project not exists, cmaking...

    :: 还没有cmake项目, 先执行cmake命令
    mkdir %build_dir_name%
    cd %build_dir_name%

    :: 构建, 创建 makefile
    cmake ../.. -D USER_PROJECT=%project% -D CATOS_TESTS=ON -G "Unix Makefiles" 
    @REM 加VERBOSE=1打印详细make过程
    @REM make VERBOSE=1
    make
) else (
    echo project exists, making...

    cd %build_dir_name%
    make
)



@REM cmake ../.. -D p=%project% -G "Unix Makefiles" --debug-output
@REM make
