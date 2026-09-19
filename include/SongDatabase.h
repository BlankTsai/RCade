#ifndef SONG_DATABASE_H
#define SONG_DATABASE_H

#include <string>
#include <vector>

// 歌曲結構
struct Song {
    std::string name;
    float bpm;
    std::string filePath;
    float offset; // 音樂偏移（秒）
};

// 歌曲資料庫
struct SongDatabase {
    std::vector<Song> songs;
};

void InitializeSongDatabase(SongDatabase& db);
const Song* GetSongByName(const SongDatabase& db, const std::string& name);
float GetSpawnIntervalFromBPM(float bpm);

#endif