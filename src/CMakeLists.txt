set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qt/qml)

add_subdirectory(application)
add_subdirectory(ui)

set(app_icon_resource_windows "${CMAKE_SOURCE_DIR}/resources/resources.rc")

qt_add_executable(${PROJECT_NAME} main.cpp  ${app_icon_resource_windows})

# Hide terminal window on Windows.
if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE TRUE
	)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	applicationlib
	applicationlibplugin
	uilibplugin
)

if (LINUX)
	qt_import_plugins(${PROJECT_NAME}
		INCLUDE
			Qt6::QWaylandIntegrationPlugin
			Qt6::QXcbIntegrationPlugin
	)
endif ()
