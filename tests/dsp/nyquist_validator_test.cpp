#include <gtest/gtest.h>
#include "../../include/dsp/core/Signal.hpp"
#include "../../include/dsp/dsp/dsp/SignalGenerator.hpp"
#include"../../include/dsp/dsp/dsp/NyquistValidator.hpp"

#include <numbers>

using dsp::Signal;
using dsp::SignalGenerator;
using dsp::NyquistValidator;

TEST(NyquistValidatorTest, FrequencyBelowNyquistIsValid) {
    // 100 Hz signal, 8000 Hz sample rate → Nyquist = 4000 Hz → valid
    EXPECT_TRUE(NyquistValidator::can_represent(100.0, 8000.0));
}

TEST(NyquistValidatorTest, FrequencyAboveNyquistIsInvalid) {
    // 5000 Hz signal, 8000 Hz sample rate → Nyquist = 4000 Hz → invalid
    EXPECT_FALSE(NyquistValidator::can_represent(5000.0, 8000.0));
}

TEST(NyquistValidatorTest, FrequencyExactlyAtNyquistIsInvalid) {
    // 4000 Hz at 8000 Hz sample rate → exactly at Nyquist → invalid
    // This is the critical edge case students get wrong
    EXPECT_FALSE(NyquistValidator::can_represent(4000.0, 8000.0));
}

TEST(NyquistValidatorTest, ZeroFrequencyIsValid) {
    // DC component (0 Hz) is always representable
    EXPECT_TRUE(NyquistValidator::can_represent(0.0, 8000.0));
}

TEST(NyquistValidatorTest, InvalidSampleRateThrows) {
    EXPECT_THROW((void)NyquistValidator::can_represent(100.0, 0.0), std::invalid_argument);
    EXPECT_THROW((void)NyquistValidator::can_represent(100.0, -1.0), std::invalid_argument);
}

TEST(NyquistValidatorTest, ValidateMultipleFrequencies) {
    Signal<double> sig(1000, 8000.0);  // Nyquist = 4000 Hz

    auto result = NyquistValidator::validate(sig, {100.0, 500.0, 2000.0, 3999.0});

    EXPECT_TRUE(result.valid);
    EXPECT_DOUBLE_EQ(result.nyquist_frequency, 4000.0);
    EXPECT_TRUE(result.violating_freqs.empty());
}

TEST(NyquistValidatorTest, ValidateDetectsViolations) {
    Signal<double> sig(1000, 8000.0);  // Nyquist = 4000 Hz

    auto result = NyquistValidator::validate(sig, {100.0, 4000.0, 5000.0, 10000.0});

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.violating_freqs.size(), 3U);  // 4000, 5000, 10000 all violate
}

TEST(NyquistValidatorTest, ClassicAliasingScenario) {
    // 200 Hz signal needs > 400 Hz sample rate
    EXPECT_TRUE(NyquistValidator::can_represent(200.0, 401.0));   // just enough
    EXPECT_FALSE(NyquistValidator::can_represent(200.0, 400.0));  // exactly 2x = not enough
    EXPECT_FALSE(NyquistValidator::can_represent(200.0, 399.0));  // below = alias
}
