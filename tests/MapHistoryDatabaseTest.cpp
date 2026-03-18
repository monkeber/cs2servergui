#include "gtest/gtest.h"

#include <MapHistoryDatabase.h>

TEST(MapHistoryDatabaseTest, InsertOfANewMap)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_NO_THROW(db.AddNewMap("1234", "deinferno", "previews/someanotherpath.png"));

	// Because we have a unique constraint for the pairs.
	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });

	ASSERT_NO_THROW(db.AddJournalEntry("123"));
	ASSERT_NO_THROW(db.AddJournalEntry("1234"));

	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });
	ASSERT_NO_THROW(db.AddJournalEntry("1234"));
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
	ASSERT_NO_THROW(db.Select(MapHistoryFilters{}));
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);
	ASSERT_NO_THROW(db.AddNewMap("1233", "notdedust", "previews/somepath.png"));
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 2);
}

TEST(MapHistoryDatabaseTest, Update)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 5;
	ASSERT_NO_THROW(db.Update("123", patch));

	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, patch.m_isBookmarked.value());
	ASSERT_EQ(entries.front().m_rating, patch.m_rating.value());
}

TEST(MapHistoryDatabaseTest, UpdateNonExistingRecord)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 5;
	ASSERT_NO_THROW(db.Update("4321", patch));

	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);
}

TEST(MapHistoryDatabaseTest, UpdateWithIncorrectValues)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));

	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);

	ASSERT_EQ(entries.front().m_isBookmarked, false);
	ASSERT_EQ(entries.front().m_rating, 0);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 10;
	ASSERT_THROW(db.Update("123", patch), SQLite::Exception);
}

TEST(MapHistoryDatabaseTest, SelectOne)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));

	ASSERT_NO_THROW(db.SelectOne("123", entries.front().m_playedAt));
}

TEST(MapHistoryDatabaseTest, Delete)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_NO_THROW(db.AddNewMap("1234", "notdedust", "previews/anothersomepath.png"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 2);

	ASSERT_NO_THROW(db.Delete("123", entries.front().m_playedAt));

	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 1);
	ASSERT_EQ(entries.front().m_workshopID, "1234");
}

TEST(MapHistoryDatabaseTest, DuplicatesFilter)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });
	ASSERT_NO_THROW(db.AddNewMap("1234", "notdedust", "previews/anothersomepath.png"));
	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });
	ASSERT_NO_THROW(db.AddJournalEntry("123"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 3);

	MapHistoryFilters filter;
	filter.m_removeDuplicated = true;
	ASSERT_NO_THROW(entries = db.Select(filter));
	ASSERT_EQ(entries.size(), 2);
}

TEST(MapHistoryDatabaseTest, SortByRatingFilter)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });
	ASSERT_NO_THROW(db.AddNewMap("1234", "notdedust", "previews/anothersomepath.png"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 2);
	ASSERT_EQ(entries.front().m_workshopID, "1234");

	MapHistoryPatch patch;
	patch.m_rating = 5;
	db.Update("123", patch);

	MapHistoryFilters filter;
	filter.m_sortByRating = true;
	ASSERT_NO_THROW(entries = db.Select(filter));
	ASSERT_EQ(entries.size(), 2);
	ASSERT_EQ(entries.front().m_workshopID, "123");
}

TEST(MapHistoryDatabaseTest, ShowBookmarksFilter)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_NO_THROW(db.AddNewMap("1234", "notdedust", "previews/anothersomepath.png"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 2);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	db.Update("123", patch);

	MapHistoryFilters filter;
	filter.m_showOnlyBookmarks = true;
	ASSERT_NO_THROW(entries = db.Select(filter));
	ASSERT_EQ(entries.size(), 1);
	ASSERT_EQ(entries.front().m_workshopID, "123");
}

TEST(MapHistoryDatabaseTest, AllFilters)
{
	MapHistoryDatabase db{ ":memory:" };
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
	ASSERT_NO_THROW(db.AddNewMap("1234", "notdedust", "previews/anothersomepath.png"));
	std::this_thread::sleep_for(std::chrono::milliseconds{ 15 });
	ASSERT_NO_THROW(db.AddJournalEntry("123"));
	ASSERT_NO_THROW(db.AddNewMap("4321", "inferno", "previews/inferno.png"));
	std::vector<MapHistoryEntry> entries;
	ASSERT_NO_THROW(entries = db.Select(MapHistoryFilters{}));
	ASSERT_EQ(entries.size(), 4);

	MapHistoryPatch patch;
	patch.m_isBookmarked = true;
	patch.m_rating = 5;
	db.Update("123", patch);

	patch.m_rating = 4;
	db.Update("4321", patch);

	MapHistoryFilters filter;
	filter.m_removeDuplicated = true;
	filter.m_showOnlyBookmarks = true;
	filter.m_sortByRating = true;
	ASSERT_NO_THROW(entries = db.Select(filter));
	ASSERT_EQ(entries.size(), 2);
	ASSERT_EQ(entries.front().m_workshopID, "123");
}
