@echo on

:: 设置第一个参数为项目名称
set project=%1

:: 创建构建目录 
mkdir build
cd build

:: 构建, 创建 makefile
cmake .. -Dp=%project% -G "Unix Makefiles"
make
