#ifndef DSP_CORE_VECTOR_BACKEND_HPP
#define DSP_CORE_VECTOR_BACKEND_HPP

#include "StorageBackend.hpp"
#include <vector>
#include <memory>

namespace dsp {
    template <typename T>
    class VectorBackend final : public StorageBackend<T>
    {
            std::vector<T> data_;
        public:
            VectorBackend() = default;
            explicit VectorBackend(std::size_t count) : data_(count) {}
            VectorBackend(std::size_t count, const T& value) : data_(count, value) {}
            explicit VectorBackend(std::vector<T>&& arr) noexcept 
            :data_(std::move(arr)) {}

            [[nodiscard]]std::size_t size() const noexcept
            {
                return data_.size();
            }

            [[nodiscard]]std::size_t capacity() const noexcept
            {
                return data_.capacity();
            }

            [[nodiscard]]bool empty() const noexcept
            {
                return data_.empty();
            }

            [[nodiscard]]T& operator[](std::size_t index)
            {
                return data_[index];
            }

            [[nodiscard]]const T& operator[](std::size_t index) const
            {
                return data_[index];                
            }

            [[nodiscard]]T& at(std::size_t index)
            {
                return data_.at(index);
            }

            [[nodiscard]]const T& at(std::size_t index) const
            {
                return data_.at(index);
            }

            [[nodiscard]]T* data() noexcept
            {
                return data_.data();
            }

            [[nodiscard]]const T* data() const noexcept
            {
                return data_.data();                
            }

            void push_back(const T& value)
            {
                data_.push_back(value);
            }

            void push_back(T&& value)
            {
                data_.push_back(std::move(value));
            }   

            void resize(std::size_t new_size)
            {
                data_.resize(new_size);
            }

            void resize(std::size_t new_size, const T& fill_value)
            {
                data_.resize(new_size, fill_value);
            }

            void reserve(std::size_t new_capacity)
            {
                data_.reserve(new_capacity);
            }

            void clear() noexcept
            {
                data_.clear();
            }

            // PRototype for deep copy
            [[nodiscard]]std::unique_ptr<StorageBackend<T>> clone()
            {
                auto copy = std::make_unique<VectorBackend>();
                copy->data_ = data_;
                return copy;
            }

            [[nodiscard]] const std::vector<T>& underlying() const noexcept
            {
                return data_;
            }
    };

} // namespace dsp

#endif // DSP_CORE_VECTOR_BACKEND_HPP
