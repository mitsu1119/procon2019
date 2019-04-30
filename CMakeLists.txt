#--------使い方--------
#buildディレクトリに移動してcmake .を実行
#buildディレクトリ内のsrcディレクトリに移動してmakeする
#実行ファイルrunが作成される

cmake_minimum_required(VERSION 3.1)
project(procon2019 CXX)

#オプション指定など
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions("-I/foo/dir1")
add_definitions("-DMACRO1=bar")

#サブディレクトリを追加
add_subdirectory(src)
