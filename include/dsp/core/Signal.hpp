#ifndef DSP_CORE_SINGAL_HPP
#define DSP_CORE_SINGAL_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <string>

#include "VectorBackend.hpp"
#include "DynamicArrayBackend.hpp"

namespace dsp {

    template <typename T>
    class Signal {

        private:
            std::unique_ptr<StorageBackend<T>> backend_;
            double sample_rate_;
            std::string label_;

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
            using iterator          =   T*;
            using const_iterator    =   const T*;

            // Constructors
            // Default constructor:
            Signal() 
            : backend_(std::make_unique<VectorBackend<T>>())
            , sample_rate_(0.0) {}

            // Construct with size and initialized to default value
            explicit Signal(std::size_t num_samples, double sample_rate = 44100.0)
                : backend_(std::make_unique<VectorBackend<T>>(num_samples))
                , sample_rate_(validate_sample_rate(sample_rate))
            {}

            // Construct with size and initialized to value
            Signal(size_type num_samples, const T& fill_value, double sample_rate)
                : backend_(std::make_unique<VectorBackend<T>>(num_samples, fill_value))
                , sample_rate_(validate_sample_rate(sample_rate))
            {}
                        
            // Inject custome Storage Backend
            Signal(std::unique_ptr<StorageBackend<T>> backend, double sample_rate = 44100.0)
                : backend_(std::move(backend))
                , sample_rate_{validate_sample_rate(sample_rate)}
            {}

            // Rule of 5
            //Destructor - std::vector handles clean-up
            ~Signal() = default;

            // Copy constructor - deep copy of all samples done by vector
            Signal(const Signal& other)
            : backend_(other.backend_->clone())
            , sample_rate_(other.sample_rate)
            , label_(other.label_) {}

            // Move constructor - deep move of all samples done by vector
            Signal(Signal&& other)
            : backend_(std::move(other.backend_))
            , sample_rate_(other.sample_rate_)
            , label_(std::move(other.label_)) {}

            // Copy assignment
            Signal& operator=(const Signal& other)
            {
                if(this != other)
                {
                    backend_ = other.backend_->clone();
                    sample_rate_ = other.sample_rate_;
                    label_ = other.label_;
                }
                return *this;
            }

            // Move assignment
            Signal& operator=(const Signal&& other)
            {
                if(this != other)
                {
                    backend_ = std::move(other.backend_);
                    sample_rate_ = other.sample_rate_;
                    label_ = std::move(other.label_);
                }
                return *this;
            }

            [[nodiscard]] double sample_rate()const noexcept {return sample_rate_;}

            // Nyquist - Max frequency that signal can represent
            [[nodiscard]] double nyquist() const noexcept {return sample_rate_ / 2.0;}
            [[nodiscard]] double duration() const noexcept 
            {
                if(backend_->empty())
                    return 0.0;
                return static_cast<double>(backend_->size()) / sample_rate_;
            }

            // Size and capacity
            [[nodiscard]]size_type size() const noexcept
            {
                return backend_->size();
            }

            [[nodiscard]]size_type capacity() const noexcept
            {
                return backend_->capacity();
            }            

            [[nodiscard]]bool empty() const noexcept
            {
                return backend_->empty();
            }
 
            // Element access - discarding sample value is alwas bug
            [[nodiscard]]T& operator[](size_type index) noexcept
            {
                return (*backend_)[index];
            }

            // Const signal should send const reference
            [[nodiscard]]const T& operator[](size_type index) const noexcept
            {
                return (*backend_)[index];
            }
            
            [[nodiscard]]T& at(size_type index)
            {
                if(index >= backend_->size())
                    throw std::out_of_range{"Signal::at - index" + std::to_string(index)
                        + "out of range (size=" + std::to_string(backend_->size()) + ")"};
                
                return backend_->at(index);
            }

            [[nodiscard]]const T& at(size_type index) const 
            {
                if(index >= backend_->size())
                    throw std::out_of_range{"Signal::at - index" + std::to_string(index)
                        + "out of range (size=" + std::to_string(backend_->size()) + ")"};
                
                return backend_->at(index);
            }
            
            // Raw data access - contigeous memory pointer
            [[nodiscard]]T* data() noexcept
            {
                return backend_->data();
            }

            [[nodiscard]]const T* data() const noexcept
            {
                return backend_->data();
            }
            
            // Iterators
            [[nodiscard]] iterator begin() noexcept {return backend_->begin();}
            [[nodiscard]] iterator end() noexcept {return backend_->end();}
            [[nodiscard]] const_iterator begin() const noexcept {return backend_->begin();}
            [[nodiscard]] const_iterator end() const noexcept {return backend_->end();}
            [[nodiscard]] const_iterator cbegin() const noexcept {return backend_->cbegin();}
            [[nodiscard]] const_iterator cend() const noexcept {return backend_->cend();}



            // Modifiers
            void push_back(const T& value)
            {
                backend_->push_back(value);
            }
            void push_back(T&& value)
            {
                backend_->push_back(std::move(value));
            }
            
            void reserve(size_type capacity)
            {
                backend_->reserve(capacity);
            }

            void resize(size_type count)
            {
                backend_->resize(count);
            }

            void clear() noexcept
            {
                backend_->clear();
            }

            void set_backend(std::unique_ptr<StorageBackend<T>> new_backend)
            {
                if(!new_backend)
                    throw std::invalid_argument{"Signal::set_backend backend must be non null"};

                backend_ = std::move(new_backend);
            }

            [[nodiscard]] const StorageBackend<T>& backend()const noexcept
            {
                return *backend_;
            }

            [[nodiscard]] const std::string& label()const noexcept
            {
                return label_;
            }            

            void set_label(std::string label)
            {
                label_ = label;
            }

    };

} // namespace dsp

#endif // DSP_CORE_SINGAL_HPP
