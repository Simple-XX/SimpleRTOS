
@echo off


:: 生成的cmake项目目录存放的位置
set output_dir=output

:: cmake 生成的目录
set build_dir_name=catos_test

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
    cmake ../.. -G "Unix Makefiles"
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
