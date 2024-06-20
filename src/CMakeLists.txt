# Language
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# App build

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qt/qml)

add_subdirectory(application)
add_subdirectory(ui)

qt_add_executable(${PROJECT_NAME} WIN32 main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
	applicationlib
	applicationlibplugin
	uilibplugin
)
