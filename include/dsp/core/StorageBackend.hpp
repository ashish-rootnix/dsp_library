#ifndef DSP_CORE_STORAGE_BACKEND_HPP
#define DSP_CORE_STORAGE_BACKEND_HPP

#include <cstddef>
#include <memory>

namespace dsp {
    template<typename T>
    class StorageBackend
    {
        protected:
            StorageBackend() = default;
            StorageBackend(const StorageBackend&) = default;
            StorageBackend(StorageBackend&&) = default;
            StorageBackend& operator=(StorageBackend&) = default;
            StorageBackend& operator=(StorageBackend&&) = default;

        public:
            virtual ~StorageBackend() = default;

            [[nodiscard]]virtual std::size_t size() const noexcept = 0;
            [[nodiscard]]virtual std::size_t capacity() const noexcept = 0;
            [[nodiscard]]virtual bool empty() const noexcept = 0;

            [[nodiscard]]virtual T& operator[](std::size_t index) = 0;
            [[nodiscard]]virtual const T& operator[](std::size_t index) const = 0;
            [[nodiscard]]virtual T& at(std::size_t index) = 0;
            [[nodiscard]]virtual const T& at(std::size_t index) const = 0;

            [[nodiscard]]virtual T* data() noexcept = 0;
            [[nodiscard]]virtual const T* data() const noexcept = 0;

            virtual void push_back(const T& value) = 0;
            virtual void push_back(T&& value) = 0;            
            virtual void resize(std::size_t new_size) = 0;
            virtual void resize(std::size_t new_size, const T& fill_value) = 0;
            virtual void reserve(std::size_t new_capacity) = 0;
            virtual void clear() noexcept = 0;

            // PRototype for deep copy
            [[nodiscard]]virtual std::unique_ptr<StorageBackend<T>> clone() = 0;            
    };

} // namespace dsp

#endif // DSP_CORE_STORAGE_BACKEND_HPP
