cmake_minimum_required(VERSION 3.21.1)

project(cs2guiserver)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_policy(SET QTP0001 NEW)

if (Qt6_VERSION VERSION_GREATER_EQUAL 6.3)
	qt_standard_project_setup()
endif ()

add_subdirectory(third_party/rconpp)
add_subdirectory(src)

# Tests
if (BUILD_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif ()
