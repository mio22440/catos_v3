@echo off

:: 设置第一个参数为项目名称
set project=%1
@REM echo project=%project%

:: 生成的cmake项目目录存放的位置
set output_dir=output

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

set build_dir_name=build_%project%_%year%%month%%day%_%HH%_%MM%_%SS%

:: 创建构建目录

cd %output_dir%
mkdir %build_dir_name%
cd %build_dir_name%

:: 构建, 创建 makefile
cmake ../.. -D USER_PROJ=%project% -G "Unix Makefiles"
@REM cmake ../.. -D p=%project% -G "Unix Makefiles" --debug-output
@REM make
