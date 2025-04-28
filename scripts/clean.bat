
:: 清除特定项目的生成bin文件
:: example: clean.bat demo

@echo off

set project=%1

if "%1" == "" (
    echo ERROR: no project specified!
    echo usage: build.bat [PROJECT_NAME]
    exit
)

:: 生成的cmake项目目录存放的位置
set output_dir=output
:: 确保output目录存在
if not exist %output_dir% (
    echo ERROR: %output_dir% folder do not exist
    exit
)

cd %output_dir%
if not exist %project% (
    echo ERROR: project %project% do not exist
    exit
)

cd %project%
make clean

echo target project %project% clean done
