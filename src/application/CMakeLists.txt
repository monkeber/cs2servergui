project(application LANGUAGES CXX)

set(${PROJECT_NAME}_SRC
	app_environment.h
	AppData.cpp
	AppData.h
	CommandHistory.cpp
	CommandHistory.h
	ProcessHandler.cpp
	ProcessHandler.h
	RCONClient.cpp
	RCONClient.h
	Settings.cpp
	Settings.h
)

qt_add_library(${PROJECT_NAME} STATIC)

# add_library(${PROJECT_NAME}
# 	STATIC
# 		${${PROJECT_NAME}_SRC}
# )

qt6_add_qml_module(${PROJECT_NAME}
	URI cs2server
	VERSION 1.0
	NO_PLUGIN
	SOURCES
		${${PROJECT_NAME}_SRC}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
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
