project(applicationlib LANGUAGES CXX)

set(${PROJECT_NAME}_SRC
	app_environment.h
	AppData.cpp
	AppData.h
	CommandHistory.cpp
	CommandHistory.h
	Log.cpp
	Log.h
	ProcessHandler.cpp
	ProcessHandler.h
	RCONClient.cpp
	RCONClient.h
	Settings.cpp
	Settings.h
)

qt_add_library(${PROJECT_NAME} STATIC)

qt_add_qml_module(${PROJECT_NAME}
	URI application
	VERSION 1.0
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
