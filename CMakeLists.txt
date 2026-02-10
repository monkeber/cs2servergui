cmake_minimum_required(VERSION 4.0.0)

# Set this variable automatically.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cs2guiserver VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# A way to provide a path to statically built Qt installation without changing
# presets or passing additional variables, mostly intended for development.
if (NOT Qt6_ROOT)
	set(ENV_FILE "${CMAKE_SOURCE_DIR}/.env")
	if (EXISTS ${ENV_FILE})
		file(STRINGS ${ENV_FILE} ENV_VARS)
		foreach(VAR ${ENV_VARS})
			# Split the line into KEY and VALUE.
			if (VAR MATCHES "([^=]+)=(.*)")
				set(ENV{${CMAKE_MATCH_1}} ${CMAKE_MATCH_2})
			endif ()
		endforeach()
		message(STATUS ".env file loaded.")
	else ()
		message(STATUS ".env file not found at ${ENV_FILE}.")
	endif ()
endif ()

find_package(ICU REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidcsv REQUIRED)

qt_policy(SET QTP0001 NEW)
qt_standard_project_setup()

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_SHARED_LIBS FALSE)
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
# set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib:\$ORIGIN/")
install(TARGETS ${PROJECT_NAME}
	DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# find_library(
#     SYSTEM_LIB_LOCATION
#     stdc++
# )

# install(
#     PROGRAMS ${SYSTEM_LIB_LOCATION}
#     DESTINATION ${CMAKE_INSTALL_LIBDIR} # Installs to ${CMAKE_INSTALL_PREFIX}/lib
#     # You can specify other destinations as needed, e.g., "bin" for DLLs on Windows
# )

#
# Packaging for the distribution.
#

include(CPack)
set(CPACK_PACKAGE_VENDOR "Monkeber")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple manager for CS2 server that you can use to send rcon commands or create 'actions' with some predefined commands")
