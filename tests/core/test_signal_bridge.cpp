#include <gtest/gtest.h>
#include "../../include/dsp/core/Signal.hpp"
#include "../../include/dsp/core/VectorBackend.hpp"
#include "../../include/dsp/core/DynamicArrayBackend.hpp"
#include <utility>
#include <numeric>

using namespace dsp;

TEST(SignalBridge, DefaultConstructorUsesVectorBackend) {
    Signal<double> sig;

    // Default state
    EXPECT_EQ(sig.size(), 0u);
    EXPECT_TRUE(sig.empty());
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 0.0);

    // Verify backend is actually a VectorBackend by checking dynamic type
    const auto& backend = sig.backend();
    EXPECT_NE(dynamic_cast<const VectorBackend<double>*>(&backend), nullptr);
}

TEST(SignalBridge, ConstructWithSizeAndRate) {
    Signal<double> sig(1024, 44100.0);

    EXPECT_EQ(sig.size(), 1024u);
    EXPECT_FALSE(sig.empty());
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 44100.0);
    EXPECT_DOUBLE_EQ(sig.nyquist(), 22050.0);
    // Duration = 1024 / 44100 ≈ 0.02322 seconds
    EXPECT_NEAR(sig.duration(), 1024.0 / 44100.0, 1e-9);
}

TEST(SignalBridge, InjectCustomBackend) {
    // Create a pre-filled VectorBackend externally
    auto backend = std::make_unique<VectorBackend<float>>(
        std::initializer_list<float>{1.0f, 2.0f, 3.0f, 4.0f}
    );

    Signal<float> sig(std::move(backend), 8000.0);

    EXPECT_EQ(sig.size(), 4u);
    EXPECT_FLOAT_EQ(sig[0], 1.0f);
    EXPECT_FLOAT_EQ(sig[3], 4.0f);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
}

TEST(SignalBridge, NullBackendThrows) {
    EXPECT_THROW(
        Signal<double>(nullptr, 44100.0),
        std::invalid_argument
    );
}

TEST(SignalBridge, NegativeSampleRateThrows) {
    EXPECT_THROW(
        Signal<double>(100, -1.0),
        std::invalid_argument
    );
}

TEST(SignalBridge, CopyConstructorDeepCopies) {
    Signal<double> original(4, 0.0, 44100.0);
    original[0] = 1.0;
    original[1] = 2.0;
    original[2] = 3.0;
    original[3] = 4.0;
    original.set_label("test_signal");

    // Copy
    Signal<double> copy(original);

    // Verify values match
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_DOUBLE_EQ(copy.sample_rate(), original.sample_rate());
    EXPECT_EQ(copy.label(), "test_signal");
    for (std::size_t i = 0; i < copy.size(); ++i) {
        EXPECT_DOUBLE_EQ(copy[i], original[i]);
    }

    // Verify independence — mutating copy doesn't affect original
    copy[0] = 999.0;
    EXPECT_DOUBLE_EQ(original[0], 1.0);  // Original unchanged
    EXPECT_DOUBLE_EQ(copy[0], 999.0);
}

TEST(SignalBridge, CopyAssignmentDeepCopies) {
    Signal<double> a(3, 1.0, 8000.0);
    Signal<double> b;

    b = a;

    EXPECT_EQ(b.size(), 3u);
    EXPECT_DOUBLE_EQ(b[0], 1.0);

    // Mutate b, verify a untouched
    b[0] = -1.0;
    EXPECT_DOUBLE_EQ(a[0], 1.0);
}

TEST(SignalBridge, SelfAssignmentIsSafe) {
    Signal<double> sig(3, 5.0, 44100.0);
    sig = sig;  // Should not crash or corrupt
    EXPECT_EQ(sig.size(), 3u);
    EXPECT_DOUBLE_EQ(sig[0], 5.0);
}

TEST(SignalBridge, MoveConstructorTransfersOwnership) {
    Signal<double> original(1024, 44100.0);
    original[0] = 42.0;

    Signal<double> moved(std::move(original));

    EXPECT_EQ(moved.size(), 1024u);
    EXPECT_DOUBLE_EQ(moved[0], 42.0);
    EXPECT_DOUBLE_EQ(moved.sample_rate(), 44100.0);

    // Original is in moved-from state
    EXPECT_DOUBLE_EQ(original.sample_rate(), 0.0);
}

TEST(SignalBridge, MoveAssignmentTransfersOwnership) {
    Signal<double> a(512, 22050.0);
    a[0] = 7.0;

    Signal<double> b;
    b = std::move(a);

    EXPECT_EQ(b.size(), 512u);
    EXPECT_DOUBLE_EQ(b[0], 7.0);
}

TEST(SignalBridge, RangeBasedFor) {
    Signal<int> sig(5, 0, 1000.0);
    int val = 0;
    for (auto& s : sig) {
        s = val++;
    }

    EXPECT_EQ(sig[0], 0);
    EXPECT_EQ(sig[4], 4);
}

TEST(SignalBridge, STLAlgorithmCompatibility) {
    Signal<double> sig(10, 0.0, 44100.0);
    // Fill with 0..9
    std::iota(sig.begin(), sig.end(), 0.0);

    // std::accumulate
    double sum = std::accumulate(sig.begin(), sig.end(), 0.0);
    EXPECT_DOUBLE_EQ(sum, 45.0);

    // std::sort (descending)
    std::sort(sig.begin(), sig.end(), std::greater<double>());
    EXPECT_DOUBLE_EQ(sig[0], 9.0);
    EXPECT_DOUBLE_EQ(sig[9], 0.0);

    // std::find
    auto it = std::find(sig.begin(), sig.end(), 5.0);
    EXPECT_NE(it, sig.end());
}

TEST(SignalBridge, SwapBackendAtRuntime) {
    Signal<double> sig(4, 1.0, 8000.0);
    EXPECT_EQ(sig.size(), 4u);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);

    // Swap to a new backend with different data
    auto new_backend = std::make_unique<VectorBackend<double>>(
        std::initializer_list<double>{10.0, 20.0}
    );
    sig.set_backend(std::move(new_backend));

    // Signal now reflects new backend's data
    EXPECT_EQ(sig.size(), 2u);
    EXPECT_DOUBLE_EQ(sig[0], 10.0);
    EXPECT_DOUBLE_EQ(sig[1], 20.0);

    // Sample rate is preserved — it's the Abstraction's state, not the backend's
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
}

TEST(SignalBridge, SwapBackendAtRuntime2) {
    Signal<double> sig(4, 1.0, 8000.0);
    EXPECT_EQ(sig.size(), 4u);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);

    // Swap to a new backend with different data
    auto new_backend = std::make_unique<DynamicArrayBackend<double>>(
        std::initializer_list<double>{10.0, 20.0}
    );
    sig.set_backend(std::move(new_backend));

    // Signal now reflects new backend's data
    EXPECT_EQ(sig.size(), 2u);
    EXPECT_DOUBLE_EQ(sig[0], 10.0);
    EXPECT_DOUBLE_EQ(sig[1], 20.0);

    // Sample rate is preserved — it's the Abstraction's state, not the backend's
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
}

TEST(SignalBridge, SwapToNullBackendThrows) {
    Signal<double> sig(4, 1.0, 8000.0);
    EXPECT_THROW(
        sig.set_backend(nullptr),
        std::invalid_argument
    );
    // Signal should be unchanged after failed swap
    EXPECT_EQ(sig.size(), 4u);
}

TEST(SignalBridge, EmptySignalDurationIsZero) {
    Signal<double> sig;
    EXPECT_DOUBLE_EQ(sig.duration(), 0.0);
}

TEST(SignalBridge, ZeroSampleRateIsAllowed) {
    // Useful for "raw samples with no rate metadata" scenarios
    Signal<double> sig(10, 0.0);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 0.0);
    EXPECT_DOUBLE_EQ(sig.duration(), 0.0);  // 0 rate → 0 duration, not infinity
    EXPECT_DOUBLE_EQ(sig.nyquist(), 0.0);
}

TEST(SignalBridge, PushBackThroughBridge) {
    Signal<double> sig;
    sig = Signal<double>(0, 44100.0);  // Empty signal with rate

    sig.push_back(1.0);
    sig.push_back(2.0);
    sig.push_back(3.0);

    EXPECT_EQ(sig.size(), 3u);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);
    EXPECT_DOUBLE_EQ(sig[2], 3.0);
}

TEST(SignalBridge, AtThrowsOnOutOfBounds) {
    Signal<double> sig(3, 0.0, 1000.0);
    EXPECT_THROW((void)sig.at(3), std::out_of_range);
    EXPECT_THROW((void)sig.at(100), std::out_of_range);
}

TEST(SignalBridge, DynamicArrayBackendWorks) {
    auto backend = std::make_unique<DynamicArrayBackend<double>>(4);
    Signal<double> sig(std::move(backend), 16000.0);
    sig[0] = 1.0;
    sig[1] = 2.0;
    EXPECT_EQ(sig.size(), 4u);
    EXPECT_DOUBLE_EQ(sig[0], 1.0);

    // Verify it's the right backend type
    const auto& b = sig.backend();
    EXPECT_NE(dynamic_cast<const DynamicArrayBackend<double>*>(&b), nullptr);
}

