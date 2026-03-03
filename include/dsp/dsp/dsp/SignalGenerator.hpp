#ifndef DSP_DSP_SIGNAL_GENERATOR_HPP
#define DSP_DSP_SIGNAL_GENERATOR_HPP

#include "../../core/Signal.hpp"
#include <cmath>
#include <numbers>

namespace dsp {
    class SignalGenerator
    {
        public:
            /**
             * @brief GenerateSine wave
             * @param frequency in Hz
             * @param sample_rate Samples per seconds
             * @param duration in seconds
             * @param amplitude default 1.0
             * @param phase Phase offset in radian default 0.0
             */
            template <typename T>
            [[nodiscard]] static Signal<T> sine(
                    double frequency,
                    double sample_rate,
                    double duration,
                    double amplitude = 1.0,
                    double phase = 0.0)
            {
                if(frequency < 0.0)
                    throw std::invalid_argument{"SignalGenerator::sine - frequency must be non-negative"
                                            + std::to_string(frequency)};
                
                if(duration <= 0.0)
                    throw std::invalid_argument{"SignalGenerator::sine - duration must be positve"
                                            + std::to_string(duration)};
                
                // Number of samples = sample_rate * duration
                const auto num_samples = static_cast<std::size_t>(sample_rate * duration);
                Signal<T> signal(num_samples, sample_rate);

                const double two_pi_f = 2.0 * std::numbers::pi * frequency;

                for(std::size_t i = 0; i < num_samples; ++i)
                {
                    const double t = static_cast<double>(i) / sample_rate;
                    signal[i] = amplitude * std::sin((two_pi_f * t) + phase);
                }
                return signal;
            }   
            
    };

} // namespace dsp

#endif //DSP_DSP_SIGNAL_GENERATOR_HPP
