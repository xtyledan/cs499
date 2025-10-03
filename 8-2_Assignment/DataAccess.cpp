#include "DataAccess.h"
#include <sstream>

static std::string isoNow() {
    time_t t = time(nullptr);
    struct tm tmv;
#if defined(_WIN32)
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
    return buf;
}

bool DataAccess::open(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        err_ = sqlite3_errmsg(db_ ? db_ : nullptr);
        return false;
    }
    // reliability & safety
    exec("PRAGMA journal_mode=WAL;");
    exec("PRAGMA foreign_keys=ON;");
    return true;
}

void DataAccess::close() {
    if (db_) { sqlite3_close(db_); db_ = nullptr; }
}

bool DataAccess::exec(const char* sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        err_ = errmsg ? errmsg : "exec failed";
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

bool DataAccess::initSchema() {
    const char* ddl =
        "CREATE TABLE IF NOT EXISTS settings ("
        "  key TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS scores ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  player TEXT NOT NULL,"
        "  points INTEGER NOT NULL,"
        "  when_utc TEXT NOT NULL,"
        "  duration_sec INTEGER NOT NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_scores_points ON scores(points DESC);";
    return exec(ddl);
}

bool DataAccess::setSetting(const std::string& key, const std::string& value) {
    const char* sql = "INSERT INTO settings(key,value) VALUES(?,?) "
        "ON CONFLICT(key) DO UPDATE SET value=excluded.value;";
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &st, nullptr) != SQLITE_OK) {
        err_ = sqlite3_errmsg(db_); return false;
    }
    sqlite3_bind_text(st, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, value.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);
    if (rc != SQLITE_DONE) { err_ = sqlite3_errmsg(db_); return false; }
    return true;
}

bool DataAccess::getSetting(const std::string& key, std::string& out) {
    const char* sql = "SELECT value FROM settings WHERE key=?;";
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &st, nullptr) != SQLITE_OK) {
        err_ = sqlite3_errmsg(db_); return false;
    }
    sqlite3_bind_text(st, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        const unsigned char* v = sqlite3_column_text(st, 0);
        out = v ? reinterpret_cast<const char*>(v) : "";
        sqlite3_finalize(st);
        return true;
    }
    sqlite3_finalize(st);
    err_.clear();
    return false; // not found is not an error here
}

bool DataAccess::addScore(const std::string& player, int points, int seconds) {
    const char* sql = "INSERT INTO scores(player,points,when_utc,duration_sec) VALUES(?,?,?,?);";
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &st, nullptr) != SQLITE_OK) {
        err_ = sqlite3_errmsg(db_); return false;
    }
    std::string now = isoNow();
    sqlite3_bind_text(st, 1, player.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, points);
    sqlite3_bind_text(st, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 4, seconds);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);
    if (rc != SQLITE_DONE) { err_ = sqlite3_errmsg(db_); return false; }
    return true;
}

std::vector<std::tuple<std::string, int, std::string, int>>
DataAccess::topScores(int limit) {
    std::vector<std::tuple<std::string, int, std::string, int>> out;
    std::string sql = "SELECT player, points, when_utc, duration_sec "
        "FROM scores ORDER BY points DESC, id ASC LIMIT ?;";
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &st, nullptr) != SQLITE_OK) {
        err_ = sqlite3_errmsg(db_); return out;
    }
    sqlite3_bind_int(st, 1, limit);

    while (sqlite3_step(st) == SQLITE_ROW) {
        std::string player = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
        int points = sqlite3_column_int(st, 1);
        std::string when = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));
        int duration = sqlite3_column_int(st, 3);
        out.emplace_back(player, points, when, duration);
    }
    sqlite3_finalize(st);
    return out;
}
