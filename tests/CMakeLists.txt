project(tests LANGUAGES CXX)

find_package(GTest REQUIRED)

qt_add_executable(${PROJECT_NAME}
	main.cpp
	MapHistoryDatabaseTest.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	rconpp
	cpr::cpr
	nlohmann_json::nlohmann_json
	SQLiteCpp
	applicationlib
	gtest::gtest
)
