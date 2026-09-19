#include "SongDatabase.h"
#include "raylib.h" // Added to resolve TraceLog and log level errors

void InitializeSongDatabase(SongDatabase& db) {
    TraceLog(LOG_INFO, "Initializing song database...");
    db.songs.clear();
    TraceLog(LOG_INFO, "Adding song: LoveTheWorld");
    db.songs.push_back({"LoveTheWorld", 110.0f, "resources/LoveTheWorld.mp3", 0.0f});

    TraceLog(LOG_INFO, "Adding song: Courage Delivery");
    db.songs.push_back({"Courage Delivery", 125.0f, "resources/CourageDelivery.mp3", 0.0f});

    TraceLog(LOG_INFO, "Adding song: Re:verse");
    db.songs.push_back({"Re:verse", 130.0f, "resources/Reverse.mp3", 0.0f});

    TraceLog(LOG_INFO, "Adding song: Fresh Afternoon");
    db.songs.push_back({"Fresh Afternoon", 130.0f, "resources/fresh afternoon.mp3", 0.0f});

    TraceLog(LOG_INFO, "Adding song: Ryuko");
    db.songs.push_back({"Ryuko(hard)", 180.0f, "resources/ryuko.mp3", 0.0f});

    TraceLog(LOG_INFO, "Song database initialized with %d songs", (int)db.songs.size());
}

const Song* GetSongByName(const SongDatabase& db, const std::string& name) {
    //TraceLog(LOG_INFO, "Searching song: %s", name.c_str());
    for (const auto& song : db.songs) {
        if (song.name == name) {
            //TraceLog(LOG_INFO, "Song found: %s", song.name.c_str());
            return &song;
        }
    }
    TraceLog(LOG_WARNING, "Song not found: %s", name.c_str());
    return nullptr;
}

float GetSpawnIntervalFromBPM(float bpm) {
    TraceLog(LOG_DEBUG, "Calculating spawn interval for BPM: %.2f", bpm);
    float interval = 60.0f / bpm; // 每拍的秒數
    TraceLog(LOG_DEBUG, "Spawn interval: %.2f seconds", interval);
    return interval;
}