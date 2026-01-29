cmake_minimum_required(VERSION 4.0.0)

# Set this variable automatically.
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "cmake/conan_provider.cmake;${CMAKE_PROJECT_TOP_LEVEL_INCLUDES}")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cs2guiserver)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_INT128")

find_package(libb2 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
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
