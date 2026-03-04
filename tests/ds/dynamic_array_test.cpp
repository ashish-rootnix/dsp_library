#include <gtest/gtest.h>
#include <algorithm>
#include <utility>
#include <string>
#include <numeric>
#include "../../include/dsp/ds/DynamicArray.hpp"

using dsp::ds::DynamicArray;

TEST(DynamicArrayTest, DefaultConstructionIsEmpty) 
{
    DynamicArray<int> arr;
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0U);
    EXPECT_EQ(arr.capacity(), 0U);
}

TEST(DynamicArrayTest, ConstructWithCapacity) 
{
    DynamicArray<int> arr(16);
    EXPECT_TRUE(arr.empty());       // no elements
    EXPECT_EQ(arr.capacity(), 16U); // but memory reserved
}

TEST(DynamicArrayTest, ConstructFromInitializerList) 
{
    DynamicArray<double> arr{1.0, 2.0, 3.0};
    EXPECT_EQ(arr.size(), 3U);
    EXPECT_DOUBLE_EQ(arr[0], 1.0);
    EXPECT_DOUBLE_EQ(arr[1], 2.0);
    EXPECT_DOUBLE_EQ(arr[2], 3.0);
}

TEST(DynamicArrayTest, CopyConstructorDeepCopies) 
{
    DynamicArray<int> original{10, 20, 30};
    DynamicArray<int> copy{original};

    EXPECT_EQ(copy.size(), 3U);
    EXPECT_EQ(copy[0], 10);

    // Modify copy — original must not change
    copy[0] = 999;
    EXPECT_EQ(original[0], 10);
}

TEST(DynamicArrayTest, CopyAssignmentDeepCopies) 
{
    DynamicArray<int> original{1, 2, 3};
    DynamicArray<int> other{9, 8};

    other = original;
    EXPECT_EQ(other.size(), 3U);
    EXPECT_EQ(other[0], 1);

    other[0] = 999;
    EXPECT_EQ(original[0], 1);
}

TEST(DynamicArrayTest, MoveConstructorStealsResources) 
{
    DynamicArray<int> original{1, 2, 3};
    const int* original_data = original.data();

    DynamicArray<int> moved{std::move(original)};

    EXPECT_EQ(moved.size(), 3U);
    EXPECT_EQ(moved.data(), original_data);  // same memory — no allocation
    EXPECT_EQ(moved[0], 1);

    EXPECT_TRUE(original.empty());
    EXPECT_EQ(original.data(), nullptr);
}

TEST(DynamicArrayTest, MoveAssignmentStealsResources) 
{
    DynamicArray<int> original{1, 2, 3};
    DynamicArray<int> other{9};

    other = std::move(original);
    EXPECT_EQ(other.size(), 3U);
    EXPECT_EQ(other[0], 1);
    EXPECT_TRUE(original.empty());
}

TEST(DynamicArrayTest, MoveConstructorIsNoexcept) 
{
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<DynamicArray<int>>);
}

TEST(DynamicArrayTest, AtThrowsOutOfRange) 
{
    DynamicArray<int> arr{1, 2, 3};
    EXPECT_THROW((void)arr.at(3), std::out_of_range);
    EXPECT_THROW((void)arr.at(100), std::out_of_range);
}

TEST(DynamicArrayTest, FrontAndBack) 
{
    DynamicArray<int> arr{10, 20, 30};
    EXPECT_EQ(arr.front(), 10);
    EXPECT_EQ(arr.back(), 30);
    
    int& fr = arr.front();
    fr = fr + 1;
    int& bk = arr.back();
    bk = bk + 1;

    EXPECT_EQ(arr.front(), 11);
    EXPECT_EQ(arr.back(), 31);   
}

TEST(DynamicArrayTest, PushBackGrows) 
{
    DynamicArray<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);

    EXPECT_EQ(arr.size(), 3U);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayTest, PushBackTriggersReallocation) 
{
    DynamicArray<int> arr(2);  // capacity 2
    arr.push_back(1);
    arr.push_back(2);
    EXPECT_EQ(arr.capacity(), 2U);

    arr.push_back(3);  // triggers growth
    EXPECT_GE(arr.capacity(), 3U);
    EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayTest, EmplaceBackConstructsInPlace) 
{
    DynamicArray<std::string> arr;
    // Constructs string directly from (count, char) — no temporary
    arr.emplace_back(5, 'x');

    EXPECT_EQ(arr.size(), 1U);
    EXPECT_EQ(arr[0], "xxxxx");
}

TEST(DynamicArrayTest, EmplaceBackReturnsReference) {
    DynamicArray<int> arr;
    int& ref = arr.emplace_back(42);
    EXPECT_EQ(ref, 42);
    EXPECT_EQ(&ref, &arr[0]);
}

TEST(DynamicArrayTest, PopBackRemovesLast) {
    DynamicArray<int> arr{1, 2, 3};
    arr.pop_back();

    EXPECT_EQ(arr.size(), 2U);
    EXPECT_EQ(arr.back(), 2);
}

TEST(DynamicArrayTest, PopBackOnEmptyThrows) {
    DynamicArray<int> arr;
    EXPECT_THROW(arr.pop_back(), std::out_of_range);
}

// ── Insert & Erase ──

TEST(DynamicArrayTest, InsertAtBeginning) {
    DynamicArray<int> arr{2, 3, 4};
    arr.insert(0, 1);

    EXPECT_EQ(arr.size(), 4U);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[3], 4);
}

TEST(DynamicArrayTest, InsertAtEnd) {
    DynamicArray<int> arr{1, 2};
    arr.insert(2, 3);  // insert at size() = append

    EXPECT_EQ(arr.size(), 3U);
    EXPECT_EQ(arr[2], 3);
}

TEST(DynamicArrayTest, InsertOutOfRangeThrows) {
    DynamicArray<int> arr{1, 2};
    EXPECT_THROW(arr.insert(5, 99), std::out_of_range);
}

TEST(DynamicArrayTest, EraseShiftsLeft) {
    DynamicArray<int> arr{10, 20, 30, 40};
    arr.erase(1);  // remove 20

    EXPECT_EQ(arr.size(), 3U);
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 30);
    EXPECT_EQ(arr[2], 40);
}

TEST(DynamicArrayTest, EraseOutOfRangeThrows) {
    DynamicArray<int> arr{1};
    EXPECT_THROW(arr.erase(1), std::out_of_range);
}

// ── Iterators ──

TEST(DynamicArrayTest, RangeBasedForWorks) {
    DynamicArray<int> arr{1, 2, 3, 4, 5};
    int sum = 0;
    for (const auto& val : arr) {
        sum += val;
    }
    EXPECT_EQ(sum, 15);
}

TEST(DynamicArrayTest, StdSortWorks) {
    DynamicArray<int> arr{5, 3, 1, 4, 2};
    std::sort(arr.begin(), arr.end());

    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 5);
}

TEST(DynamicArrayTest, StdAccumulateWorks) {
    DynamicArray<int> arr{1, 2, 3, 4};
    int total = std::accumulate(arr.begin(), arr.end(), 0);
    EXPECT_EQ(total, 10);
}

TEST(DynamicArrayTest, StdFindWorks) {
    DynamicArray<int> arr{10, 20, 30};
    auto it = std::find(arr.begin(), arr.end(), 20);

    EXPECT_NE(it, arr.end());
    EXPECT_EQ(*it, 20);
}

TEST(DynamicArrayTest, CapacityDoublesOnGrowth) {
    DynamicArray<int> arr;

    arr.push_back(1);
    EXPECT_EQ(arr.capacity(), 1U);  // 0 → 1

    arr.push_back(2);
    EXPECT_EQ(arr.capacity(), 2U);  // 1 → 2

    arr.push_back(3);
    EXPECT_EQ(arr.capacity(), 4U);  // 2 → 4

    arr.push_back(4);
    arr.push_back(5);
    EXPECT_EQ(arr.capacity(), 8U);  // 4 → 8
}

TEST(DynamicArrayTest, ReserveIncreasesCapacity) {
    DynamicArray<int> arr;
    arr.reserve(100);

    EXPECT_EQ(arr.capacity(), 100U);
    EXPECT_TRUE(arr.empty());  // no elements created
}

TEST(DynamicArrayTest, ReserveDoesNotShrink) {
    DynamicArray<int> arr;
    arr.reserve(100);
    arr.reserve(10);  // should be ignored

    EXPECT_EQ(arr.capacity(), 100U);
}

TEST(DynamicArrayTest, ShrinkToFitReducesCapacity) {
    DynamicArray<int> arr;
    arr.reserve(100);
    arr.push_back(1);
    arr.push_back(2);
    arr.shrink_to_fit();

    EXPECT_EQ(arr.size(), 2U);
    EXPECT_EQ(arr.capacity(), 2U);
}

// ── Clear ──

TEST(DynamicArrayTest, ClearRemovesAllElements) {
    DynamicArray<int> arr{1, 2, 3};
    arr.clear();

    EXPECT_TRUE(arr.empty());
    EXPECT_GE(arr.capacity(), 3U);  // capacity preserved — same as std::vector
}

TEST(DynamicArrayTest, WorksWithNonTrivialTypes) {
    DynamicArray<std::string> arr;
    arr.push_back("hello");
    arr.emplace_back("world");

    EXPECT_EQ(arr.size(), 2U);
    EXPECT_EQ(arr[0], "hello");
    EXPECT_EQ(arr[1], "world");
    // This tests that destructors run correctly — ASan will catch leaks
}

TEST(DynamicArrayTest, ManyInsertions) {
    DynamicArray<int> arr;
    for (int i = 0; i < 10000; ++i) {
        arr.push_back(i);
    }

    EXPECT_EQ(arr.size(), 10000U);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[9999], 9999);
    // ASan will catch any memory errors from repeated reallocations
}

TEST(DynamicArrayTest, SwapWorks) {
    DynamicArray<int> a{1, 2, 3};
    DynamicArray<int> b{4, 5};

    a.swap(b);

    EXPECT_EQ(a.size(), 2U);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(b[0], 1);
}
