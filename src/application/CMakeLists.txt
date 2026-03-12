project(applicationlib LANGUAGES CXX)

set(${PROJECT_NAME}_SRC
	app_environment.h
	AppData.cpp
	AppData.h
	CommandHistory.cpp
	CommandHistory.h
	Log.cpp
	Log.h
	MapHistory.cpp
	MapHistory.h
	MapHistoryDatabase.cpp
	MapHistoryDatabase.h
	MapHistoryModel.cpp
	MapHistoryModel.h
	ProcessHandler.cpp
	ProcessHandler.h
	ProcessImpl.h
	RCONClient.cpp
	RCONClient.h
	Settings.cpp
	Settings.h
	Utilities.h
)

if (WIN32)
	list(APPEND ${PROJECT_NAME}_SRC ProcessImpl_Windows.cpp Utilities_Windows.cpp)
elseif (LINUX)
	list(APPEND ${PROJECT_NAME}_SRC ProcessImpl_Linux.cpp Utilities_Linux.cpp)
endif ()

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
	cpr::cpr
	nlohmann_json::nlohmann_json
	SQLiteCpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
	 $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wpedantic>
	 $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)

target_precompile_headers(${PROJECT_NAME} PUBLIC
	pch.h
)
