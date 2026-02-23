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

TEST(MapHistoryDatabaseTest, Select)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.Select());
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select());
	ASSERT_EQ(entries.size(), 1);
	ASSERT_NO_THROW(db.AddNewMap("1233", "notdedust", "previews/somepath.png"));
	ASSERT_NO_THROW(entries = db.Select());
	ASSERT_EQ(entries.size(), 2);
}

TEST(MapHistoryDatabaseTest, Update)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select());
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 5;
	ASSERT_NO_THROW(db.Update("123", patch));

	ASSERT_NO_THROW(entries = db.Select());
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, patch.m_isBookmarked.value());
	ASSERT_EQ(entries.front().m_rating, patch.m_rating.value());
}

TEST(MapHistoryDatabaseTest, UpdateWithIncorrectValues)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select());
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 10;
	ASSERT_THROW(db.Update("123", patch), SQLite::Exception);
}
