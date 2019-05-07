#ソースファイルを追加した場合ここに記入する
add_library(greetings
	SHARED
	agent.cpp
	field.cpp
	useful.cpp
	disp.cpp
	)

#パッケージを探す
find_package(GLUT)
find_package(OpenGL)

#リンクする
target_link_libraries(greetings ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)

#インクルードディレクトリを指定
target_include_directories(greetings
	PUBLIC ${PROJECT_SOURCE_DIR}/include
	)

#リンクする
add_executable(run main.cpp)
target_link_libraries(run greetings)
