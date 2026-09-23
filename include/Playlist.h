#pragma once

#include <stdexcept>
#include <string>

struct Track {
    std::string title;
    std::string artist;
    int durationSec;

    bool operator==(const Track& other) const {
        return title == other.title && artist == other.artist;
    }

    bool operator!=(const Track& other) const {
        return !(*this == other);
    }
};

class PlaylistException : public std::runtime_error {
public:
    explicit PlaylistException(const std::string& message)
        : std::runtime_error(message) {}
};

class Playlist {
public:
    explicit Playlist(int capacity);
    Playlist(const Playlist& other);
    Playlist& operator=(const Playlist& other);
    ~Playlist();

    int Capacity() const { return capacity; }
    int Count() const { return count; }

    const Track& operator[](int index) const;
    Track& operator[](int index);

    bool operator==(const Playlist& other) const;
    bool operator!=(const Playlist& other) const { return !(*this == other); }

    void Add(const Track& track);
    void Merge(const Playlist& other);
    void RemoveTracksOf(const Playlist& other);
    int TotalDuration() const;
    int FindByArtist(const std::string& artist) const;
    std::string ToString() const;

private:
    Track* tracks;
    int capacity;
    int count;

    bool Contains(const Track& track) const;
    void CheckIndex(int index) const;
};
