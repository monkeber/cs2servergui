project(tests LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Test Widgets)
include(CTest)
enable_testing(true)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Test
	Qt6::Widgets
)

install(TARGETS ${PROJECT_NAME}
	BUNDLE  DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
	TARGET ${PROJECT_NAME}
	OUTPUT_SCRIPT deploy_script
	NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
