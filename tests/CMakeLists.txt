project(tests LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Test Widgets)
include(CTest)
enable_testing(true)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Test
	Qt6::Widgets
	rconpp
)
