#include "gtest/gtest.h"

#include <MapHistoryDatabase.h>

TEST(MapHistoryDatabaseTest, InsertOfANewMap)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_NO_THROW(db.AddNewMap("1234", "deinferno", "previews/someanotherpath.png"));
	ASSERT_NO_THROW(db.AddJournalEntry("1234"));
	ASSERT_NO_THROW(db.AddJournalEntry("1234"));
	ASSERT_NO_THROW(db.AddJournalEntry("123"));
	ASSERT_NO_THROW(db.AddJournalEntry("123"));
}

TEST(MapHistoryDatabaseTest, AddingTheSameMapTwice)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"), SQLite::Exception);
}
