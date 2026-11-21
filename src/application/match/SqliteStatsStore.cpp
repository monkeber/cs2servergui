#include "SqliteStatsStore.h"

namespace
{

std::optional<std::string> GetOptionalString(const SQLite::Statement& query, int column)
{
	if (query.isColumnNull(column))
	{
		return std::nullopt;
	}

	return query.getColumn(column).getString();
}

}	 // namespace

SqliteMatchStatsStore::SqliteMatchStatsStore()
	: SqliteMatchStatsStore{ "match_stats.db3" }
{
}

SqliteMatchStatsStore::SqliteMatchStatsStore(const std::string& dbName)
	: m_db{ std::make_unique<SQLite::Database>(
		  dbName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) }
{
	m_db->exec("PRAGMA foreign_keys = ON;");
	InitSchema();
}

std::int64_t SqliteMatchStatsStore::CreateMatch(const MapInfo& map)
{
	SQLite::Statement query{ *m_db, "INSERT INTO match (map_name, workshop_id) VALUES (?, ?);" };
	query.bind(1, map.m_name);
	if (map.m_workshopId)
	{
		query.bind(2, *map.m_workshopId);
	}
	query.exec();

	return m_db->getLastInsertRowid();
}

void SqliteMatchStatsStore::SaveRound(std::int64_t matchId, const RoundStats& stats)
{
	SQLite::Transaction transaction{ *m_db };

	SQLite::Statement matchQuery{ *m_db,
		"UPDATE match SET score_ct = ?, score_t = ? WHERE id = ?;" };
	matchQuery.bind(1, stats.m_scoreCt);
	matchQuery.bind(2, stats.m_scoreT);
	matchQuery.bind(3, matchId);
	matchQuery.exec();

	SQLite::Statement playerQuery{ *m_db, R"__(
	INSERT INTO player_match_stats
		(match_id, account_id, team, kills, deaths, assists, damage, hs_pct, adr, mvp)
		VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
	ON CONFLICT (match_id, account_id) DO UPDATE SET
		team = excluded.team,
		kills = excluded.kills,
		deaths = excluded.deaths,
		assists = excluded.assists,
		damage = excluded.damage,
		hs_pct = excluded.hs_pct,
		adr = excluded.adr,
		mvp = excluded.mvp;
	)__" };

	for (const auto& player : stats.m_players)
	{
		playerQuery.bind(1, matchId);
		playerQuery.bind(2, static_cast<std::int64_t>(player.m_accountId));
		playerQuery.bind(3, player.m_team);
		playerQuery.bind(4, player.m_kills);
		playerQuery.bind(5, player.m_deaths);
		playerQuery.bind(6, player.m_assists);
		playerQuery.bind(7, player.m_damage);
		playerQuery.bind(8, player.m_headshotPercent);
		playerQuery.bind(9, player.m_adr);
		playerQuery.bind(10, player.m_mvp);
		playerQuery.exec();
		playerQuery.reset();
	}

	transaction.commit();
}

void SqliteMatchStatsStore::EndMatch(std::int64_t matchId)
{
	SQLite::Statement query{ *m_db, "UPDATE match SET ended_at = datetime('now') WHERE id = ?;" };
	query.bind(1, matchId);
	query.exec();
}

void SqliteMatchStatsStore::SavePlayerName(std::uint32_t accountId, const std::string& name)
{
	SQLite::Statement query{ *m_db, R"__(
	INSERT INTO player (account_id, name) VALUES (?, ?)
	ON CONFLICT (account_id) DO UPDATE SET
		name = excluded.name,
		updated_at = datetime('now');
	)__" };
	query.bind(1, static_cast<std::int64_t>(accountId));
	query.bind(2, name);
	query.exec();
}

std::vector<MatchRecord> SqliteMatchStatsStore::SelectMatches() const
{
	SQLite::Statement query{ *m_db, R"__(
	SELECT id, map_name, workshop_id, started_at, ended_at, score_ct, score_t
		FROM match ORDER BY id;
	)__" };

	std::vector<MatchRecord> result;
	while (query.executeStep())
	{
		result.push_back(MatchRecord{
			.m_id = query.getColumn(0).getInt64(),
			.m_map =
				MapInfo{
					.m_name = query.getColumn(1).getString(),
					.m_workshopId = GetOptionalString(query, 2),
				},
			.m_startedAt = query.getColumn(3).getString(),
			.m_endedAt = GetOptionalString(query, 4),
			.m_scoreCt = query.getColumn(5).getInt(),
			.m_scoreT = query.getColumn(6).getInt(),
		});
	}

	return result;
}

std::vector<PlayerMatchRecord> SqliteMatchStatsStore::SelectPlayerStats(std::int64_t matchId) const
{
	SQLite::Statement query{ *m_db, R"__(
	SELECT stats.account_id, team, kills, deaths, assists, damage, hs_pct, adr, mvp, player.name
		FROM player_match_stats stats
		LEFT JOIN player ON player.account_id = stats.account_id
		WHERE match_id = ? ORDER BY kills DESC, stats.account_id;
	)__" };
	query.bind(1, matchId);

	std::vector<PlayerMatchRecord> result;
	while (query.executeStep())
	{
		result.push_back(PlayerMatchRecord{
			.m_name = GetOptionalString(query, 9),
			.m_stats =
				PlayerStats{
					.m_accountId = static_cast<std::uint32_t>(query.getColumn(0).getInt64()),
					.m_team = query.getColumn(1).getInt(),
					.m_kills = query.getColumn(2).getInt(),
					.m_deaths = query.getColumn(3).getInt(),
					.m_assists = query.getColumn(4).getInt(),
					.m_damage = query.getColumn(5).getInt(),
					.m_headshotPercent = query.getColumn(6).getDouble(),
					.m_adr = query.getColumn(7).getDouble(),
					.m_mvp = query.getColumn(8).getInt(),
				},
		});
	}

	return result;
}

void SqliteMatchStatsStore::InitSchema() const
{
	SQLite::Transaction transaction{ *m_db };
	m_db->exec(R"__(
	CREATE TABLE IF NOT EXISTS match(
		id INTEGER PRIMARY KEY,
		map_name TEXT NOT NULL,
		workshop_id TEXT,
		started_at DATETIME NOT NULL DEFAULT(datetime('now')),
		ended_at DATETIME,
		score_ct INTEGER NOT NULL DEFAULT 0,
		score_t INTEGER NOT NULL DEFAULT 0
	);

	CREATE TABLE IF NOT EXISTS player(
		account_id INTEGER PRIMARY KEY,
		name TEXT NOT NULL,
		updated_at DATETIME NOT NULL DEFAULT(datetime('now'))
	);

	CREATE TABLE IF NOT EXISTS player_match_stats(
		match_id INTEGER NOT NULL REFERENCES match(id) ON DELETE CASCADE,
		account_id INTEGER NOT NULL,
		team INTEGER NOT NULL,
		kills INTEGER NOT NULL,
		deaths INTEGER NOT NULL,
		assists INTEGER NOT NULL,
		damage INTEGER NOT NULL,
		hs_pct REAL NOT NULL,
		adr REAL NOT NULL,
		mvp INTEGER NOT NULL,
		PRIMARY KEY (match_id, account_id)
	);
	)__");
	transaction.commit();
}
