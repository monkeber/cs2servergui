project(tests LANGUAGES CXX)

find_package(GTest REQUIRED)

qt_add_executable(${PROJECT_NAME}
	main.cpp
	MapHistoryDatabaseTest.cpp
	GuardTest.cpp
	HttpMatchLogReceiverTest.cpp
	MatchLogSplitterTest.cpp
	RawLogWriterTest.cpp
	RoundStatsExtractorTest.cpp
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
