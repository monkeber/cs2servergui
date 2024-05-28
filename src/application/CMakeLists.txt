project(application LANGUAGES CXX)

set(${PROJECT_NAME}_SRC
	app_environment.h
	AppData.cpp
	AppData.h
	import_qml_plugins.h
	ProcessHandler.cpp
	ProcessHandler.h
	RCONClient.cpp
	RCONClient.h
	Settings.cpp
	Settings.h
)

add_library(${PROJECT_NAME}
	STATIC
		${${PROJECT_NAME}_SRC}
)

# find_library(RCONCPP_LIB rconpp
# 	PATHS
# 		${CMAKE_SOURCE_DIR}/third_party
# )

message("${RCONCPP_LIB}")

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	rconpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)
