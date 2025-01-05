cmake_minimum_required(VERSION 3.21.1)

# Set this variable automatically.
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "cmake/conan_provider.cmake;${CMAKE_PROJECT_TOP_LEVEL_INCLUDES}")
set(CMAKE_CXX_STANDARD 20)

project(cs2guiserver)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidcsv REQUIRED)

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
