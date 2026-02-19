project(tests LANGUAGES CXX)

find_package(GTest REQUIRED)

include(CTest)

qt_add_executable(${PROJECT_NAME}
	main.cpp
	DatabaseTest.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	rconpp
	cpr::cpr
	nlohmann_json::nlohmann_json
	rapidcsv::rapidcsv
	SQLiteCpp
	applicationlib
	gtest::gtest
)
