#include "gtest/gtest.h"

#include "Database.h"

TEST(DatabaseTest, Insert)
{
	Database db;
	ASSERT_NO_THROW(db.AddNewMap("123", "dedust", "previews/somepath.png"));
}
