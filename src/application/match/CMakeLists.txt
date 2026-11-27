set(${PROJECT_NAME}_APP_MATCH_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/Guard.h
	${CMAKE_CURRENT_SOURCE_DIR}/HttpLogReceiver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/HttpLogReceiver.h
	${CMAKE_CURRENT_SOURCE_DIR}/LogSource.h
	${CMAKE_CURRENT_SOURCE_DIR}/MatchLogSplitter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MatchLogSplitter.h
	${CMAKE_CURRENT_SOURCE_DIR}/RawLogWriter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RawLogWriter.h
	${CMAKE_CURRENT_SOURCE_DIR}/RoundStatsExtractor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RoundStatsExtractor.h
	${CMAKE_CURRENT_SOURCE_DIR}/StatsRecorder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StatsRecorder.h
)

# Make the list visible to the parent CMakeLists.txt.
set(${PROJECT_NAME}_APP_MATCH_SRC ${${PROJECT_NAME}_APP_MATCH_SRC} PARENT_SCOPE)
