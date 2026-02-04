cmake_minimum_required(VERSION 4.0.0)

# Set this variable automatically.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cs2guiserver VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidcsv REQUIRED)

qt_policy(SET QTP0001 NEW)

qt_standard_project_setup()

add_subdirectory(third_party/rconpp)
add_subdirectory(src)

# Tests
if (BUILD_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif ()

#
# Installation steps.
#

install(TARGETS ${PROJECT_NAME}
	DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
	TARGET ${PROJECT_NAME}
	OUTPUT_SCRIPT deploy_script
	NO_TRANSLATIONS
	NO_COMPILER_RUNTIME
	NO_UNSUPPORTED_PLATFORM_ERROR
	# Excluding these plugins because install failed for them not being able to modify RPATH.
	EXCLUDE_PLUGIN_TYPES imageformats platformthemes
)

# Call the deployment script on "cmake --install".
install(SCRIPT ${deploy_script})

#
# Packaging for the distribution.
#

include(CPack)
set(CPACK_PACKAGE_VENDOR "Monkeber")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple manager for CS2 server that you can use to send rcon commands or create 'actions' with some predefined commands")
