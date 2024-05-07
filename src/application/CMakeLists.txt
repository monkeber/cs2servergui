project(application LANGUAGES CXX)

set(${PROJECT_NAME}_SRC
	app_environment.h
	import_qml_plugins.h
	ProcessHandler.cpp
	ProcessHandler.h
)

add_library(${PROJECT_NAME}
	STATIC
		${${PROJECT_NAME}_SRC}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)
