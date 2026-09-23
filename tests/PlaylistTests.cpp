#include <gtest/gtest.h>

#include "Playlist.h"

namespace {

const Track kFirst{"First Song", "Artist A", 200};
const Track kSecond{"Second Song", "Artist B", 125};
const Track kThird{"Third Song", "Artist A", 59};

TEST(TrackTests, EqualityUsesOnlyTitleAndArtist) {
    EXPECT_EQ(kFirst, (Track{"First Song", "Artist A", 999}));
    EXPECT_NE(kFirst, (Track{"Another Song", "Artist A", 200}));
}

TEST(PlaylistConstructorTests, CreatesEmptyPlaylistWithSpecifiedCapacity) {
    Playlist playlist(3);
    EXPECT_EQ(playlist.Capacity(), 3);
    EXPECT_EQ(playlist.Count(), 0);
}

TEST(PlaylistConstructorTests, RejectsNonPositiveCapacity) {
    EXPECT_THROW(Playlist(0), PlaylistException);
    EXPECT_THROW(Playlist(-2), PlaylistException);
}

TEST(PlaylistCopyTests, CopyConstructorCreatesIndependentCopy) {
    Playlist original(2);
    original.Add(kFirst);

    Playlist copy(original);
    copy[0].title = "Changed";

    EXPECT_EQ(original[0].title, "First Song");
    EXPECT_EQ(copy[0].title, "Changed");
}

TEST(PlaylistCopyTests, AssignmentCopiesDataAndHandlesSelfAssignment) {
    Playlist source(3);
    source.Add(kFirst);
    source.Add(kSecond);
    Playlist destination(1);

    destination = source;
    EXPECT_EQ(destination, source);
    EXPECT_EQ(destination.Capacity(), 3);

    destination = destination;
    EXPECT_EQ(destination, source);
}

TEST(PlaylistIndexerTests, ReadsAndReplacesExistingTrack) {
    Playlist playlist(2);
    playlist.Add(kFirst);
    playlist[0] = kSecond;
    const Playlist& readOnly = playlist;

    EXPECT_EQ(readOnly[0], kSecond);
}

TEST(PlaylistIndexerTests, RejectsIndexesOutsideOccupiedRange) {
    Playlist playlist(2);
    playlist.Add(kFirst);
    const Playlist& readOnly = playlist;

    EXPECT_THROW(playlist[-1], PlaylistException);
    EXPECT_THROW(readOnly[1], PlaylistException);
}

TEST(PlaylistEqualityTests, ComparesCountOrderAndTrackIdentity) {
    Playlist left(3);
    Playlist right(3);
    EXPECT_TRUE(left == right);

    left.Add(kFirst);
    EXPECT_FALSE(left == right);

    right.Add(kSecond);
    EXPECT_TRUE(left != right);

    right[0] = kFirst;
    EXPECT_TRUE(left == right);
}

TEST(PlaylistAddTests, AppendsUniqueTracks) {
    Playlist playlist(2);
    playlist.Add(kFirst);
    playlist.Add(kSecond);

    EXPECT_EQ(playlist.Count(), 2);
    EXPECT_EQ(playlist[1], kSecond);
}

TEST(PlaylistAddTests, RejectsDuplicateEvenWhenDurationDiffers) {
    Playlist playlist(2);
    playlist.Add(kFirst);

    EXPECT_THROW(
        playlist.Add({"First Song", "Artist A", 1}), PlaylistException);
    EXPECT_EQ(playlist.Count(), 1);
}

TEST(PlaylistAddTests, RejectsTrackWhenPlaylistIsFull) {
    Playlist playlist(1);
    playlist.Add(kFirst);

    EXPECT_THROW(playlist.Add(kSecond), PlaylistException);
    EXPECT_EQ(playlist.Count(), 1);
}

TEST(PlaylistMergeTests, AppendsOnlyTracksThatAreNotPresent) {
    Playlist playlist(4);
    playlist.Add(kFirst);
    Playlist other(3);
    other.Add({"First Song", "Artist A", 500});
    other.Add(kSecond);
    other.Add(kThird);

    playlist.Merge(other);

    ASSERT_EQ(playlist.Count(), 3);
    EXPECT_EQ(playlist[0], kFirst);
    EXPECT_EQ(playlist[1], kSecond);
    EXPECT_EQ(playlist[2], kThird);
}

TEST(PlaylistMergeTests, LeavesPlaylistUnchangedWhenCapacityIsInsufficient) {
    Playlist playlist(2);
    playlist.Add(kFirst);
    Playlist other(2);
    other.Add(kSecond);
    other.Add(kThird);

    EXPECT_THROW(playlist.Merge(other), PlaylistException);
    ASSERT_EQ(playlist.Count(), 1);
    EXPECT_EQ(playlist[0], kFirst);
}

TEST(PlaylistMergeTests, AcceptsEmptyOrFullyDuplicatePlaylist) {
    Playlist playlist(1);
    playlist.Add(kFirst);
    Playlist empty(1);
    Playlist duplicate(1);
    duplicate.Add(kFirst);

    EXPECT_NO_THROW(playlist.Merge(empty));
    EXPECT_NO_THROW(playlist.Merge(duplicate));
    EXPECT_EQ(playlist.Count(), 1);
}

TEST(PlaylistRemoveTests, RemovesAllMatchingTracksAndKeepsOrder) {
    Playlist playlist(4);
    playlist.Add(kFirst);
    playlist.Add(kSecond);
    playlist.Add(kThird);
    Playlist toRemove(2);
    toRemove.Add(kFirst);
    toRemove.Add(kThird);

    playlist.RemoveTracksOf(toRemove);

    ASSERT_EQ(playlist.Count(), 1);
    EXPECT_EQ(playlist[0], kSecond);
}

TEST(PlaylistRemoveTests, ThrowsWhenNoTrackMatches) {
    Playlist playlist(2);
    playlist.Add(kFirst);
    Playlist other(1);
    other.Add(kSecond);

    EXPECT_THROW(playlist.RemoveTracksOf(other), PlaylistException);
    EXPECT_EQ(playlist.Count(), 1);
}

TEST(PlaylistDurationTests, ReturnsZeroForEmptyAndSumForFilledPlaylist) {
    Playlist playlist(3);
    EXPECT_EQ(playlist.TotalDuration(), 0);

    playlist.Add(kFirst);
    playlist.Add(kSecond);
    playlist.Add(kThird);
    EXPECT_EQ(playlist.TotalDuration(), 384);
}

TEST(PlaylistFindTests, ReturnsFirstMatchingArtist) {
    Playlist playlist(3);
    playlist.Add(kFirst);
    playlist.Add(kSecond);
    playlist.Add(kThird);

    EXPECT_EQ(playlist.FindByArtist("Artist A"), 0);
    EXPECT_EQ(playlist.FindByArtist("Artist B"), 1);
}

TEST(PlaylistFindTests, ThrowsWhenArtistIsMissing) {
    Playlist playlist(1);
    playlist.Add(kFirst);

    EXPECT_THROW(playlist.FindByArtist("Unknown"), PlaylistException);
}

TEST(PlaylistStringTests, FormatsEachTrackAndPadsSeconds) {
    Playlist playlist(2);
    EXPECT_EQ(playlist.ToString(), "");

    playlist.Add(kFirst);
    playlist.Add(kSecond);
    EXPECT_EQ(
        playlist.ToString(),
        "Artist A - First Song (3:20)\nArtist B - Second Song (2:05)\n");
}

}  // namespace
