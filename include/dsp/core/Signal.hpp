#ifndef DSP_CORE_SINGAL_HPP
#define DSP_CORE_SINGAL_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dsp {

    template <typename T>
    class Signal {

        private:
            std::vector<T> samples_;
            double sample_rate_;

            static double validate_sample_rate(double rate)
            {
                if(rate <= 0.0)
                    throw std::invalid_argument{
                        "Signal: sample_rate must be positive, got" + std::to_string(rate)
                    };
                return rate;
            }

        public:
            // TYPE aliases
            // STL Compatible naming for generic coding
            using value_type        =   T;
            using size_type         =   std::size_t;
            using iterator          =   typename std::vector<T>::iterator;
            using const_iterator    =   typename std::vector<T>::const_iterator;

            // Constructors

            // Default constructor: empty sinal, sample rate is still required
            explicit Signal(double sample_rate = 44100.0)
                : sample_rate_(validate_sample_rate(sample_rate))
            {}

            // Construct with size and initialized to default T constructor
            Signal(size_type count, double sample_rate)
                : samples_(count, T{})
                , sample_rate_(validate_sample_rate(sample_rate))
            {}
                        
            // Construct from initializer-list - Useful for tests
            Signal(std::initializer_list<T> init, double sample_rate = 44100.0)
                : samples_{init}
                , sample_rate_{validate_sample_rate(sample_rate)}
            {}

            // Cosntruct by taking ownership of existing data - to avoid a copy
            Signal(std::vector<T> data, double sample_rate)
                : samples_(std::move(data))
                , sample_rate_(validate_sample_rate(sample_rate))
            {}

            // Rule of 5
            //Destructor - std::vector handles clean-up
            ~Signal() = default;

            // Copy constructor - deep copy of all samples done by vector
            Signal(const Signal&) = default;

            // Copy Assignments - deep copy of all samples done by vector
            Signal& operator=(const Signal&) = default;
            
            // Move constructor: leave source empty
            // noexcept: enables std::vector<Signal<T>> to use move during reallocation
            // without noexcept, vector falls back to copy, samples may get deep copied but sample_rate_ might not updated in default
            Signal(Signal&& other) noexcept
                : samples_(std::move(other.samples_))
                , sample_rate_(other.sample_rate_)
            {
                other.sample_rate_ = 0;
            }

            // Move assignment operatos
            Signal& operator=(Signal&& other) noexcept 
            {
                if(this != &other)
                {
                    samples_ = std::move(other.samples_);
                    sample_rate_ = other.sample_rate_;
                    other.sample_rate_ = 0;
                }
                return *this;
            }

            // Capacity
            [[nodiscard]]size_type size() const noexcept
            {
                return samples_.size();
            }

            [[nodiscard]]bool empty() const noexcept
            {
                return samples_.empty();
            }
 
            // Element access - discarding sample value is alwas bug
            [[nodiscard]]T& operator[](size_type index) noexcept
            {
                return samples_[index];
            }

            // Const signal should send const reference
            [[nodiscard]]const T& operator[](size_type index) const noexcept
            {
                return samples_[index];
            }
            
            [[nodiscard]]T& at(size_type index)
            {
                if(index >= samples_.size())
                    throw std::out_of_range{"Signal::at - index" + std::to_string(index)
                        + "out of range (size=" + std::to_string(samples_.size()) + ")"};
                
                return samples_.at(index);
            }

            [[nodiscard]]const T& at(size_type index) const 
            {
                if(index >= samples_.size())
                    throw std::out_of_range{"Signal::at - index" + std::to_string(index)
                        + "out of range (size=" + std::to_string(samples_.size()) + ")"};
                
                return samples_.at(index);
            }
            
            // Raw data access - contigeous memory pointer
            [[nodiscard]]T* data() noexcept
            {
                return samples_.data();
            }

            [[nodiscard]]const T* data() const noexcept
            {
                return samples_.data();
            }
            
            // Iterators
            [[nodiscard]] iterator begin() noexcept {return samples_.begin();}
            [[nodiscard]] iterator end() noexcept {return samples_.end();}
            [[nodiscard]] const_iterator begin() const noexcept {return samples_.begin();}
            [[nodiscard]] const_iterator end() const noexcept {return samples_.end();}
            [[nodiscard]] const_iterator cbegin() const noexcept {return samples_.cbegin();}
            [[nodiscard]] const_iterator cend() const noexcept {return samples_.cend();}

            [[nodiscard]] double sample_rate()const noexcept {return sample_rate_;}

            // Nyquist - Max frequency that signal can represent
            [[nodiscard]] double nyquist() const noexcept {return sample_rate_ / 2.0;}
            [[nodiscard]] double duration() const noexcept 
            {
                if(samples_.empty())
                    return 0.0;
                return static_cast<double>(samples_.size()) / sample_rate_;
            }

            // Modifiers
            void push_back(const T& value)
            {
                samples_.push_back(value);
            }
            void push_back(T&& value)
            {
                samples_.push_back(std::move(value));
            }
            
            void reserve(size_type capacity)
            {
                samples_.reserve(capacity);
            }

            void resize(size_type count)
            {
                samples_.resize(count);
            }

            void clear() noexcept
            {
                samples_.clear();
            }
    };

} // namespace dsp

#endif // DSP_CORE_SINGAL_HPP
