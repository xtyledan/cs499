#pragma once
#include <string>
#include <vector>
#include <utility> // pair
#include <ctime>
#include <tuple>   
#include <stdint.h>
#include <sqlite3.h>

class DataAccess {
public:
    bool open(const std::string& path);   // opens or creates
    void close();

    // schema
    bool initSchema(); // creates tables if not exist

    // settings
    bool setSetting(const std::string& key, const std::string& value);
    bool getSetting(const std::string& key, std::string& out);

    // scores
    bool addScore(const std::string& player, int points, int seconds);
    std::vector<std::tuple<std::string, int, std::string, int>> topScores(int limit); // name, points, isoDate, duration

    // utility
    std::string lastError() const { return err_; }

private:
    sqlite3* db_{ nullptr };
    std::string err_;

    bool exec(const char* sql);
};
