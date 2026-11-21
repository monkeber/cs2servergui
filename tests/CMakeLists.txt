project(tests LANGUAGES CXX)

find_package(GTest REQUIRED)

qt_add_executable(${PROJECT_NAME}
	main.cpp
	MapHistoryDatabaseTest.cpp
	Cs2MatchLogParserTest.cpp
	HttpMatchLogReceiverTest.cpp
	MatchStatsServiceTest.cpp
	RawLogWriterTest.cpp
	SqliteMatchStatsStoreTest.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
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
	httplib::httplib
	applicationlib
	gtest::gtest
)
