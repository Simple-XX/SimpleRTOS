
:: 清除所有生成的cmake项目

@echo off

:: 生成的cmake项目目录存放的位置
set output_dir=output

cd %output_dir%

for /D %%D in (.\*) do (
    echo deleting"%%D"
    rd /S /Q "%%D"
)

echo clean done
