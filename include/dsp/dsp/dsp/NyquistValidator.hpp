#ifndef DSP_DSP_NYQUIST_VALIDATOR_HPP
#define DSP_DSP_NYQUIST_VALIDATOR_HPP

#include "../../core/Signal.hpp"
#include <vector>
#include <string>

namespace dsp {

    // Result of nyquist validation test
    struct NyquistResult
    {   
        bool valid;
        double nyquist_frequency;
        std::vector<double> violating_freqs;
    };

    class NyquistValidator
    {
        public:
            /**
             * @brief Nyquist criteria: Sampling frequency > 2 * Maximum input frequency
             * @param frequency
             * @param sample_rate
             */
            [[nodiscard]] static bool can_represent(double frequency, double sample_rate)
            {
                if(sample_rate <= 0.0)
                        throw std::invalid_argument{"NyquistValidator::can_represent - sample rate must be non-negative"
                                                + std::to_string(sample_rate)}; 
                return frequency < (sample_rate / 2.0);                                                           
            }
            
            /**
             * @brief Check multiple frequencies against a signal's sample rate
             * @param signal 
             * @param frequencies vector of frequencies
             */ 
            template <typename T>
            [[nodiscard]] static NyquistResult validate(const Signal<T>& signal, 
                        const std::vector<double>& frequencies)
            {
                NyquistResult result;
                result.nyquist_frequency = signal.nyquist();
                result.valid = true;

                for(const double freq : frequencies)
                {
                    if(freq >= result.nyquist_frequency)
                    {
                        result.valid = false;
                        result.violating_freqs.push_back(freq);
                    }
                }
                return result;
            }
    };

} // namespace dsp 

#endif // DSP_DSP_NYQUIST_VALIDATOR_HPP
