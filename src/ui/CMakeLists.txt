project(uilib)

set(QML_IMPORT_PATH
	${QT_QML_OUTPUT_DIRECTORY}
	CACHE PATH "Import paths for Qt Creator's code model" FORCE)

qt_add_library(${PROJECT_NAME} STATIC)
set_source_files_properties(Globals.qml PROPERTIES QT_QML_SINGLETON_TYPE true)
set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE true)

qt_add_qml_module(${PROJECT_NAME}
	URI ui
	VERSION 1.0
	RESOURCE_PREFIX "/qt/qml"
	QML_FILES
		App.qml
		AppMenuBar.qml
		ConsolePage.qml
		Globals.qml
		InputField.qml
		LogWindow.qml
		Main.qml
		MainMenu.qml
		MapHistoryWindow.qml
		PowerPanel.qml
		QuickCommand.qml
		ServerParametersPage.qml
		Spoiler.qml
		TerminalOutput.qml
		Theme.qml
)

qt_add_resources(${PROJECT_NAME} "configuration" PREFIX "/" FILES qtquickcontrols2.conf)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	applicationlibplugin
)
