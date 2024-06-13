project(cs2serverui)

set(QML_IMPORT_PATH
	${QT_QML_OUTPUT_DIRECTORY}
	CACHE STRING "Import paths for Qt Creator's code model" FORCE)

# make IDEs aware of the QML import path
set(QML_IMPORT_PATH
	${PROJECT_BINARY_DIR}/qml
	CACHE PATH "Path to the custom QML components defined by the project")

qt_add_library(${PROJECT_NAME} STATIC)
set_source_files_properties(Constants.qml PROPERTIES QT_QML_SINGLETON_TYPE true)

qt6_add_qml_module(${PROJECT_NAME}
	URI Main
	VERSION 1.0
	RESOURCE_PREFIX "/qt/qml"
	NO_PLUGIN
	QML_FILES
		App.qml
		ConsolePage.qml
		Constants.qml
		InputField.qml
		Main.qml
		MainMenu.qml
		PowerPanel.qml
		ServerParametersPage.qml
		Spoiler.qml
		TerminalOutput.qml
	RESOURCES
		fonts/fonts.txt
)

qt_add_resources(${PROJECT_NAME} "configuration" PREFIX "/" FILES qtquickcontrols2.conf)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick application)
