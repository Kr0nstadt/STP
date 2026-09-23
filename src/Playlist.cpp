#include "Playlist.h"

#include <iomanip>
#include <sstream>

Playlist::Playlist(int capacity)
    : tracks(nullptr), capacity(capacity), count(0) {
    if (capacity <= 0) {
        throw PlaylistException(
            "недопустимое значение вместимости = " + std::to_string(capacity));
    }
    tracks = new Track[capacity];
}

Playlist::Playlist(const Playlist& other)
    : tracks(new Track[other.capacity]),
      capacity(other.capacity),
      count(other.count) {
    for (int i = 0; i < count; ++i) {
        tracks[i] = other.tracks[i];
    }
}

Playlist& Playlist::operator=(const Playlist& other) {
    if (this == &other) {
        return *this;
    }

    Track* newTracks = new Track[other.capacity];
    for (int i = 0; i < other.count; ++i) {
        newTracks[i] = other.tracks[i];
    }

    delete[] tracks;
    tracks = newTracks;
    capacity = other.capacity;
    count = other.count;
    return *this;
}

Playlist::~Playlist() {
    delete[] tracks;
}

void Playlist::CheckIndex(int index) const {
    if (index < 0 || index >= count) {
        throw PlaylistException(
            "неверное значение индекса i = " + std::to_string(index));
    }
}

const Track& Playlist::operator[](int index) const {
    CheckIndex(index);
    return tracks[index];
}

Track& Playlist::operator[](int index) {
    CheckIndex(index);
    return tracks[index];
}

bool Playlist::operator==(const Playlist& other) const {
    if (count != other.count) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (tracks[i] != other.tracks[i]) {
            return false;
        }
    }
    return true;
}

bool Playlist::Contains(const Track& track) const {
    for (int i = 0; i < count; ++i) {
        if (tracks[i] == track) {
            return true;
        }
    }
    return false;
}

void Playlist::Add(const Track& track) {
    if (count >= capacity) {
        throw PlaylistException(
            "плейлист заполнен, вместимость = " + std::to_string(capacity));
    }
    if (Contains(track)) {
        throw PlaylistException(
            "трек уже есть в плейлисте: " + track.artist + " - " + track.title);
    }
    tracks[count] = track;
    ++count;
}

void Playlist::Merge(const Playlist& other) {
    int newTrackCount = 0;
    for (int i = 0; i < other.count; ++i) {
        if (!Contains(other.tracks[i])) {
            ++newTrackCount;
        }
    }

    if (count + newTrackCount > capacity) {
        throw PlaylistException("недостаточно места для объединения плейлистов");
    }

    for (int i = 0; i < other.count; ++i) {
        if (!Contains(other.tracks[i])) {
            Add(other.tracks[i]);
        }
    }
}

void Playlist::RemoveTracksOf(const Playlist& other) {
    int writeIndex = 0;
    int removedCount = 0;

    for (int readIndex = 0; readIndex < count; ++readIndex) {
        if (other.Contains(tracks[readIndex])) {
            ++removedCount;
        } else {
            if (writeIndex != readIndex) {
                tracks[writeIndex] = tracks[readIndex];
            }
            ++writeIndex;
        }
    }

    if (removedCount == 0) {
        throw PlaylistException("ни один трек не найден в плейлисте");
    }
    count = writeIndex;
}

int Playlist::TotalDuration() const {
    int total = 0;
    for (int i = 0; i < count; ++i) {
        total += tracks[i].durationSec;
    }
    return total;
}

int Playlist::FindByArtist(const std::string& artist) const {
    for (int i = 0; i < count; ++i) {
        if (tracks[i].artist == artist) {
            return i;
        }
    }
    throw PlaylistException("исполнитель не найден: " + artist);
}

std::string Playlist::ToString() const {
    std::ostringstream out;
    for (int i = 0; i < count; ++i) {
        const int minutes = tracks[i].durationSec / 60;
        const int seconds = tracks[i].durationSec % 60;
        out << tracks[i].artist << " - " << tracks[i].title
            << " (" << minutes << ':' << std::setw(2) << std::setfill('0')
            << seconds << ")\n";
    }
    return out.str();
}
