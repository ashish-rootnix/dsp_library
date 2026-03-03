#include <gtest/gtest.h>
#include "../../include/dsp/core/Signal.hpp"

#include <utility>
#include <type_traits>
#include <vector>

using dsp:: Signal;

TEST(SignalTest, DefaultConstructionIsEmpty) {
    Signal<double> sig;
    EXPECT_TRUE(sig.empty());
    EXPECT_EQ(sig.size(), 0U);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 44100.0);
}

TEST(SignalTest, ConstructWithSizeZeroInitailized) {
    Signal<double> sig(256, 8000.0);
    EXPECT_FALSE(sig.empty());
    EXPECT_EQ(sig.size(), 256U);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
    for(std::size_t i = 0; i < sig.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(sig[i], 0.0);
    }
}

TEST(SignalTest, ConstructWithInitializerList) {
    Signal<double> sig({1.0, 2.0, 3.0, 4.0, 5.0}, 1000.0);
    EXPECT_FALSE(sig.empty());
    EXPECT_EQ(sig.size(), 5U);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 1000.0);
    
    EXPECT_DOUBLE_EQ(sig[0], 1.0);
    EXPECT_DOUBLE_EQ(sig[1], 2.0);
    EXPECT_DOUBLE_EQ(sig[2], 3.0);    
    EXPECT_DOUBLE_EQ(sig[3], 4.0);
    EXPECT_DOUBLE_EQ(sig[4], 5.0);    
}

TEST(SignalTest, ConstructFromVector) {
    Signal<double> sig(std::vector<double>{1.0, 2.0, 3.0}, 8000.0);
    EXPECT_FALSE(sig.empty());
    EXPECT_EQ(sig.size(), 3U);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);
    EXPECT_DOUBLE_EQ(sig[1], 2.0);
    EXPECT_DOUBLE_EQ(sig[2], 3.0);    
}

TEST(SignalTest, IinvalidSampleRateThrows)
{
    EXPECT_THROW(Signal<double>(0.0), std::invalid_argument);
    EXPECT_THROW(Signal<double>(-100.0), std::invalid_argument);
}

// Rule of 5
TEST(SignalTest, CopyConstructorDeepCopies)
{
    Signal<double> original({1.0, 2.0, 3.0}, 8000.0);
    Signal<double> copy{original};

    EXPECT_EQ(copy.size(), 3U);
    EXPECT_DOUBLE_EQ(copy.sample_rate(), 8000.0);

    // Modifying copy wont change original
    copy[0] = 99.0;
    EXPECT_DOUBLE_EQ(original[0], 1.0); 

    // Once copied original modification should not change copied
    original[0] = 100.0;
    EXPECT_DOUBLE_EQ(copy[0], 99.0);     
}

TEST(SignalTest, CopyAssignmentDeepCopies)
{
    Signal<double> original({1.0, 2.0, 3.0}, 8000.0);
    Signal<double> other({9.0}, 16000.0);

    other = original;
    EXPECT_EQ(original.size(), 3U);
    EXPECT_DOUBLE_EQ(original.sample_rate(), 8000.0);

    // Modifying copy wont change original
    other[0] = 99.0;
    EXPECT_DOUBLE_EQ(original[0], 1.0); 

    // Once copied original modification should not change copied
    original[0] = 100.0;
    EXPECT_DOUBLE_EQ(other[0], 99.0);     
}

TEST(SignalTest, MoveConstructorStealResource)
{
    Signal<double> original({1.0, 2.0, 3.0}, 8000.0);
    Signal<double> moved{std::move(original)};

    EXPECT_EQ(moved.size(), 3U);
    EXPECT_DOUBLE_EQ(moved.sample_rate(), 8000.0);
    EXPECT_DOUBLE_EQ(moved[0], 1.0); 

    // Original is empty 
    EXPECT_TRUE(original.empty());
    EXPECT_DOUBLE_EQ(original.sample_rate(), 0.0);  
}

TEST(SignalTest, MoveCAssignmentStealResource)
{
    Signal<double> original({1.0, 2.0, 3.0}, 8000.0);
    Signal<double> moved({9.0}, 16000.0);
    
    moved = std::move(original);

    EXPECT_EQ(moved.size(), 3U);
    EXPECT_DOUBLE_EQ(moved.sample_rate(), 8000.0);
    EXPECT_DOUBLE_EQ(moved[0], 1.0); 

    // Original is empty 
    EXPECT_TRUE(original.empty());
    EXPECT_DOUBLE_EQ(original.sample_rate(), 0.0);  
}

TEST(SignalTest, MoveConstructorIsNoexcept)
{
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<Signal<double>>);
}

TEST(SignalTest, MoveAssignmentIsNoexcept)
{
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<Signal<double>>);
}

TEST(SignalTest, AtThrowsOutOfRange)
{
    Signal<double> sig({1.0, 2.0}, 1000.0);
    EXPECT_THROW((void)sig.at(2), std::out_of_range);
    EXPECT_THROW((void)sig.at(100), std::out_of_range);    
}

TEST(SignalTest, AtReturnsCorrectValue) 
{
    Signal<double> sig({10.0, 20.0, 30.0}, 1000.0);
    EXPECT_DOUBLE_EQ(sig.at(0), 10.0);
    EXPECT_DOUBLE_EQ(sig.at(2), 30.0);
}

TEST(SignalTest, DataReturnsContiguousPointer) 
{
    Signal<double> sig({1.0, 2.0, 3.0}, 1000.0);
    const double* ptr = sig.data();
    EXPECT_DOUBLE_EQ(*ptr, 1.0);
    EXPECT_DOUBLE_EQ(*(ptr+1), 2.0);
    EXPECT_DOUBLE_EQ(ptr[2], 3.0);
}

TEST(SignalTest, RangeBasedForWorks) {
    Signal<double> sig({1.0, 2.0, 3.0}, 1000.0);
    double sum = 0.0;
    for (const auto& sample : sig) 
    {
        sum += sample;
    }
    EXPECT_DOUBLE_EQ(sum, 6.0);
}

TEST(SignalTest, NyquistIsHalfSampleRate) 
{
    Signal<double> sig(256, 8000.0);
    EXPECT_DOUBLE_EQ(sig.nyquist(), 4000.0);
}

TEST(SignalTest, DurationIsCorrect) 
{
    Signal<double> sig(8000, 8000.0);  // 1 second of audio
    EXPECT_DOUBLE_EQ(sig.duration(), 1.0);
}

TEST(SignalTest, EmptySignalDurationIsZero) 
{
    Signal<double> sig(8000.0);
    EXPECT_DOUBLE_EQ(sig.duration(), 0.0);
}

TEST(SignalTest, PushBackGrowsSignal) {
    Signal<double> sig(1000.0);
    sig.push_back(1.0);
    sig.push_back(2.0);
    EXPECT_EQ(sig.size(), 2U);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);
}

TEST(SignalTest, SingleSampleSignal) {
    Signal<double> sig({42.0}, 1000.0);
    EXPECT_EQ(sig.size(), 1U);
    EXPECT_DOUBLE_EQ(sig.duration(), 0.001);  // 1/1000
}

TEST(SignalTest, SelfAssignmentIsSafe) {
    Signal<double> sig({1.0, 2.0}, 8000.0);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
    sig = std::move(sig);
    #pragma GCC diagnostic pop
    // Must not crash — state may be valid or empty, but not UB
}
