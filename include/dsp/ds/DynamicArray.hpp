#ifndef DSP_DS_DYNAMIC_ARRAY_HPP
#define DSP_DS_DYNAMIC_ARRAY_HPP

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace dsp::ds
{
    template <typename T>
    class DynamicArray 
    {
            [[no_unique_address]] std::allocator<T> alloc_;
            T* data_;
            std::size_t size_;
            std::size_t capacity_;

            void bounds_check(std::size_t index)
            {
                if(index >= size_)
                    throw std::out_of_range{"dynamic_array::at - index " + std::to_string(index)
                            + " out of range (size " + std::to_string(size_) + ")"};
            }

            void ensure_capacity()
            {   
                if(size_ < capacity_) return;

                size_type new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
                reallocate(new_cap);
            }

            void reallocate (std::size_t new_capacity)
            {
                T* new_data = alloc_.allocate(new_capacity);

                for(std::size_t i = 0; i < size_ ; ++i)
                {
                    if constexpr (std::is_nothrow_move_constructible_v<T>)
                    {
                        std::construct_at(new_data + i, std::move(data_[i]));   
                    }
                    else
                    {
                        std::construct_at(new_data + i, data_[i]);
                    }
                }
                destroy_all();
                deallocate();

                data_ = new_data;
                capacity_ = new_capacity;
            }

            void destroy_all() noexcept
            {
                for(std::size_t i = 0; i < size_; ++i)
                {
                    std::destroy_at(data_ + i);
                }
            }

            void deallocate()
            {
                if(data_)
                    alloc_.deallocate(data_, capacity_);
            }

        public:
            using value_type        =   T;
            using size_type         =   std::size_t;
            using iterator          =   T*;
            using const_iterator    =   const T*;

        // Constructors            
        /**
         * @brief Default constructor
         */
        DynamicArray() noexcept : data_{nullptr}, size_{0}, capacity_{0}
        {}
        
        /**
         * @brief construct with initial capacity - reserve the memory
         * @param initial_capacity Allocate memory to be reserved
         */
        DynamicArray(size_type initial_capacity)
        :data_{initial_capacity > 0 ? alloc_.allocate(initial_capacity) : nullptr}
        , size_{0}
        , capacity_{initial_capacity}
        {}
        
        /**
         * @brief construct with initializer list
         * @param initializrt_list
         */
        DynamicArray(std::initializer_list<T> init)
        : data_{init.size() > 0 ? alloc_.allocate(init.size()) : nullptr}
        , size_{0}
        , capacity_{init.size()}
        { 
            for(const auto& elem : init)
            {
                std::construct_at(data_+ size_, elem);
                ++size_;
            }
        }

        // Rule of 5
        /**
         * @brief Destructor
         */
        ~DynamicArray()
        {
            destroy_all();
            deallocate();            
        }

        /**
         * @brief Copy constructor
         * @param other dynamic array
         */
        DynamicArray(const DynamicArray& other)
        : data_{other.size_ > 0 ? alloc_.allocate(other.size_) : nullptr}
        , size_{0}
        , capacity_{other.capacity_}
        {
            for(size_type i = 0; i < other.size_; ++i)
            {
                std::construct_at(data_ + i, other.data_[i]);
                ++size_;
            }
        }

        /**
         * @brief Unified assignement operator
         * @param other dynamic array
         * If lvalue copy constructor is called
         * if rvalue then data is swapped with existing object
         */
        DynamicArray& operator=(DynamicArray other) noexcept
        {
            swap(other);
            return *this;
        }
        
       /**
         * @brief Copy constructor
         * @param other dynamic array
         */
        DynamicArray(DynamicArray&& other) noexcept
        : data_{other.data_}
        , size_{other.size_}
        , capacity_{other.capacity_}
        {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
       
        // Data member access 
        [[nodiscard]] size_type size() const noexcept { return size_; }
        [[nodiscard]] size_type capacity() const noexcept { return capacity_; }
        [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

        /**
         * @brief reserve new memory
         */
        void reserve(size_type new_capacity) 
        {
            if (new_capacity <= capacity_) return;
            reallocate(new_capacity);
        }

        /**
         * @brief Shrinks memory to number of elements
         */
        void shrink_to_fit()
        {
            if(capacity_ == size_) return;
            if(size_ == 0)
            {
                deallocate();
                data_ = nullptr;
                capacity_ = 0;
                return;
            }
            reallocate(size_);
        }

        // Element access
        [[nodiscard]] T& operator[](size_type index) noexcept {
            return data_[index];
        }

        [[nodiscard]] const T& operator[](size_type index) const noexcept {
            return data_[index];
        }

        [[nodiscard]] T& at(size_type index) {
            bounds_check(index);
            return data_[index];
        }

        [[nodiscard]] const T& at(size_type index) const {
            bounds_check(index);
            return data_[index];
        }
        
        [[nodiscard]] T& front() noexcept { return data_[0]; }
        [[nodiscard]] const T& front() const noexcept { return data_[0]; }

        [[nodiscard]] T& back() noexcept { return data_[size_ - 1]; }
        [[nodiscard]] const T& back() const noexcept { return data_[size_ - 1]; }

        [[nodiscard]] T* data() noexcept { return data_; }
        [[nodiscard]] const T* data() const noexcept { return data_; } 
        
        void push_back(const T& value) 
        {
            ensure_capacity();
            std::construct_at(data_+size_, value);
            ++size_;
        }

        void push_back(T&& value) 
        {
            ensure_capacity();
            std::construct_at(data_+size_, std::move(value));
            ++size_;
        }

        /**
         * @brief varadic inputs, inserts element into array without copying / moving
         * Creates objects exactly at memoty location
         */
        template <typename... Args>
        T& emplace_back(Args&&... args)
        {
            ensure_capacity();
            T* new_element = data_ + size_;
            std::construct_at(new_element, std::forward<Args>(args)...);
            ++size_;
            return *new_element;
        }
        
        /**
         * @brief pops the last element in the array
         */
        void pop_back() {
            if (size_ == 0) {
                throw std::out_of_range{"DynamicArray::pop_back — array is empty"};
            }
            --size_;
            std::destroy_at(data_ + size_);
        }

        /**
         * @brief destructs all elements in array
         */
        void clear() noexcept {
            destroy_all();
            size_ = 0;
        }
        
        /**
         * @brief earase at index
         * @param index 
         */
        void erase(size_type index)
        {
            bounds_check(index);

            for(size_type i = index; i < size_-1; ++i)
            {
                data_[i] = std::move(data_[index+i]);
            }

            --size_;
            std::destroy_at(data_ + size_);
        }

        /**
         * @brief insert a element at index 
         * @param index
         * @param value
         */
        void insert(size_type index, const T& value)
        {
            if(index > size_)
                throw std::out_of_range{"DynamicArray::insert - index " + std::to_string(index)
                    + "out of range (size " + std::to_string(size_) + ")"};
                
            ensure_capacity();

            for(size_type i = size_; i > index; --i)
            {
                std::construct_at(data_ + i, std::move(data_[i-1]));
                std::destroy_at(data_ + i -1);
            }
            std::construct_at(data_ + index, value);
            ++size_;
        }

        // Iterators
        [[nodiscard]] iterator begin() noexcept { return data_; }
        [[nodiscard]] iterator end() noexcept { return data_ + size_; }
        [[nodiscard]] const_iterator begin() const noexcept { return data_; }
        [[nodiscard]] const_iterator end() const noexcept { return data_ + size_; }
        [[nodiscard]] const_iterator cbegin() const noexcept { return data_; }
        [[nodiscard]] const_iterator cend() const noexcept { return data_ + size_; }  
        
        /**
         * @brief swap
         */
        void swap(DynamicArray& other) noexcept
        {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);                        
        }

        friend void swap(DynamicArray& a, DynamicArray& b) noexcept
        {
            a.swap(b);
        }
    };
    
}//namespace dsp::ds}

#endif
