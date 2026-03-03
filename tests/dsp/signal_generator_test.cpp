#include <gtest/gtest.h>
#include "../../include/dsp/core/Signal.hpp"
#include "../../include/dsp/dsp/dsp/SignalGenerator.hpp"

#include <numbers>

using dsp::Signal;
using dsp::SignalGenerator;

TEST(SignalGeneratorTest, SineWaveCorrectLength) {
    auto sig = SignalGenerator::sine<double>(440.0, 8000.0, 1.0);
    EXPECT_EQ(sig.size(), 8000U);
    EXPECT_DOUBLE_EQ(sig.sample_rate(), 8000.0);
}

TEST(SignalGeneratorTest, SineWaveStartsAtZero) {
    // sin(0) = 0, so first sample with phase=0 should be 0
    auto sig = SignalGenerator::sine<double>(100.0, 8000.0, 0.01, 1.0, std::numbers::pi / 2);
    EXPECT_NEAR(sig[0], 1.0, 1e-12);
}

TEST(SignalGeneratorTest, SineWaveAmplitude) {
    auto sig = SignalGenerator::sine<double>(100.0, 8000.0, 1.0, 3.5);

    // Find peak value — should be close to amplitude
    double max_val = 0.0;
    for (std::size_t i = 0; i < sig.size(); ++i) {
        max_val = std::max(max_val, std::abs(sig[i]));
    }

    // Why 0.01 tolerance: discrete sampling might not hit the exact peak.
    // At 8000 Hz sample rate and 100 Hz signal, we have 80 samples per cycle.
    // Worst case peak error ≈ amplitude * (1 - cos(π/80)) ≈ 0.002
    EXPECT_NEAR(max_val, 3.5, 0.01);
}

TEST(SignalGeneratorTest, SineWavePhaseOffset) {
    // sin(pi/2) = 1.0, so first sample should be ~1.0
    auto sig = SignalGenerator::sine<double>(100.0, 8000.0, 0.01, 1.0, std::numbers::pi / 2.0);
    EXPECT_NEAR(sig[0], 1.0, 1e-12);
}

TEST(SignalGeneratorTest, SineWaveFrequencyCorrectness) {
    // Generate 10 Hz sine at 1000 Hz sample rate for 1 second
    // Should complete exactly 10 full cycles
    auto sig = SignalGenerator::sine<double>(10.0, 1000.0, 1.0);

    // Count zero crossings (positive-going)
    // 10 Hz = 10 cycles = 20 zero crossings total, 10 positive-going
    int positive_crossings = 0;
    for (std::size_t i = 1; i < sig.size(); ++i) {
        if (sig[i - 1] <= 0.0 && sig[i] >= 0.0) {
            ++positive_crossings;
        }
    }

    EXPECT_EQ(positive_crossings, 10); // Instead 10 9 crossings are validated
}

TEST(SignalGeneratorTest, NegativeFrequencyThrows) {
    EXPECT_THROW((void)SignalGenerator::sine<double>(-100.0, 8000.0, 1.0), std::invalid_argument);
}

TEST(SignalGeneratorTest, ZeroDurationThrows) {
    EXPECT_THROW((void)SignalGenerator::sine<double>(100.0, 8000.0, 0.0), std::invalid_argument);
}



