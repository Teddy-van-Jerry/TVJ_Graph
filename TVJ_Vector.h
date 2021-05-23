/*
 * File: TVJ_Vector.h
 * --------------------------
 *
 * @author: Teddy van Jerry
 * @licence: The MIT Licence
 * @compiler: at least C++/11 (tested on MSVC and MinGW)
 *
 * @version 1.5 2021/05/21
 * - solve iterator bugs
 * 
 * @version 1.4 2021/05/14
 * - add function no_delete
 * 
 * @version 1.3 2021/04/25
 * - add function delete_vectors_with_same_content_pointers
 * 
 * @version 1.2 2021/04/05
 * - bug fix
 * - change capacity_ to protected
 * - add functions
 * 
 * @version 1.1 2021/03/25
 * - bug fix in compatibility with MinGW
 * - bug fix in inheritance
 *
 * @version 1.0 2021/03/24
 * - initial version
 *
 */

#pragma once

#include <iostream>
#include <stdexcept>
#include <exception>
#include <iterator>
#ifdef _MSC_VER
#include <xutility> // std::_Is_iterator
#endif // for MSVC

namespace tvj
{

#define ASCENDING  true
#define DESCENDING false

#ifndef error_info
#define error_info(_text__, _exception__) \
_error_info(_text__, _exception__, __FILE__, __LINE__);
#endif // !error_info


#pragma warning(disable: 4018) // signed/unsigned mismatch
#pragma warning(disable: 4172) // returning address of local variable or temporary
#pragma warning(disable: 4244) // convertion possible loss of data
#pragma warning(disable: 26812)// enum type unscoped

    // error throw code
    enum TVJ_VECTOR_EXCEPTION
    {
        TVJ_VECTOR_UNDERFLOW = 1000,
        TVJ_VECTOR_OVERFLOW,
        TVJ_VECTOR_TYPE_MISMATCH,
        TVJ_VECTOR_NULLPTR,
        TVJ_VECTOR_ITER_RANGE,
        TVJ_VECTOR_OTHER
    };

    /**
     * brief: throw error info
     * param: const char* and exception name (an enum type)
     * return: void
     * @warning disabled: 26812
     */
    inline void _error_info(const char* text, const TVJ_VECTOR_EXCEPTION& exception_code, const char* file_name = nullptr, int line = 0)
    {
        std::cerr << "\n==================================================================="
            << "\nEXCEPTION at file " << file_name << " in line " << line
            << "\n-> " << text
            << "\n===================================================================" << std::endl;
        switch (exception_code)
        {
        case TVJ_VECTOR_UNDERFLOW:
            throw std::underflow_error(text);
            break;
        case TVJ_VECTOR_OVERFLOW:
            throw std::overflow_error(text);
            break;
        case TVJ_VECTOR_TYPE_MISMATCH:
#ifdef _MSC_VER // MSVC compiler
            throw std::exception(text);
#else
            throw std::runtime_error(text);
#endif
            break;
        case TVJ_VECTOR_NULLPTR:
            throw std::runtime_error(text);
            break;
        case TVJ_VECTOR_ITER_RANGE:
            throw std::range_error(text);
            break;
        default:
#ifdef _MSC_VER // MSVC compiler
            throw std::exception(text);
#else
            throw std::runtime_error(text);
#endif
            break;
        }
    }

    /**
     * brief: swap two contents
     * param: two elements with the same type
     * return: void
     */
    template<typename T>
    inline void _swap(T& a, T& b)
    {
        auto tmp = a;
        a = b;
        b = tmp;
    }

    /**
     * brief: compare two contents (>= or <=)
     * param: two elements with the same type and the order
     * return: bool
     */
    template<typename T>
    inline bool _compare(const T& a, const T& b, bool is_ascending)
    {
        if (a == b) return true;
        return ((a > b) ^ is_ascending);
    }

    /**
     * brief: compare two contents strictly (> or <)
     * param: two elements with the same type and the order
     * return: bool
     */
    template<typename T>
    inline bool _strict_compare(const T& a, const T& b, bool is_ascending = ASCENDING)
    {
        if (a == b) return false;
        return ((a > b) ^ is_ascending);
    }

    // a linear container similar to std::vector that supports random access
    template<typename Elem>
    class vector
    {
        template<typename T>
        friend void delete_vectors_with_same_content_pointers(size_t size, T array_of_vectors);

    protected:
        class const_iterator
        {
            friend class vector<Elem>;

        protected:
            Elem* value;
            const vector<Elem>* parent;

        public:
            // @constructors
            const_iterator();
            const_iterator(Elem* elem_, const vector<Elem>* parent_);

            // @destructors
            ~const_iterator();

            // @limited access
            const Elem& operator*() const;
            const Elem* operator->() const;
            auto operator++();
            auto operator--();
            auto operator++(int);
            auto operator--(int);
            const_iterator operator+(const unsigned n) const; // move right
            const_iterator operator-(const unsigned n) const; // move left
            const_iterator operator+=(const unsigned n);
            const_iterator operator-=(const unsigned n);
            auto operator-(const const_iterator& iter); // distance
            bool operator==(const const_iterator& iter) const noexcept;
            bool operator!=(const const_iterator& iter) const noexcept;
            bool operator<(const const_iterator& iter) const noexcept;
            bool operator>(const const_iterator& iter) const noexcept;
            bool operator<=(const const_iterator& iter) const noexcept;
            bool operator>=(const const_iterator& iter) const noexcept;
        };

        class iterator : public const_iterator
        {
        public:

            // @constructos
            iterator();
            iterator(const const_iterator& const_iter);
            using const_iterator::const_iterator;

            // @destructors
            ~iterator();

            // @limited access
            Elem& operator*();
            Elem* operator->();
            iterator operator+(const unsigned n); // move right
            iterator operator-(const unsigned n); // move left
            auto operator-(const iterator& iter); // distance
            bool operator==(const iterator& iter) const noexcept;
            bool operator!=(const iterator& iter) const noexcept;
            bool operator<(const iterator& iter) const noexcept;
            bool operator>(const iterator& iter) const noexcept;
            bool operator<=(const iterator& iter) const noexcept;
            bool operator>=(const iterator& iter) const noexcept;
        };

    public:

        /**
         * brief: default constructor
         * param: size, default element, capacity
         * return: --
         */
        vector();

        /**
         * brief: constructor
         * param: size, default element, capacity
         * return: --
         */
        vector(size_t size, const Elem& elem, size_t capacity = 32);

        /**
         * brief: copy constructor
         * param: the vector of the same type
         * return: --
         */
        vector(const vector<Elem>& new_vec);

#ifdef _MSC_VER

        /**
         * brief: constructor fron iterators
         * param: size, default element, capacity
         * return: --
         * @compiler: MSVC
         */
        template<typename _Iter>
        vector(const _Iter& iter1, typename std::enable_if<std::_Is_iterator<_Iter>::value, const _Iter&>::type iter2);

#else

        /**
         * brief: constructor fron iterators
         * param: size, default element, capacity
         * return: --
         * @compiler: not MSVC
         */
        template<typename _Iter>
        vector(const _Iter& iter1, const _Iter& iter2);

#endif

        /**
         * brief: destructor
         * param: (void)
         * return: --
         */
        ~vector();

        /**
         * @ iterators and const iterators
         * @ begin is the iterator pointing to the first element
         * @ end is the iterator pointing to one past the last element
         * @ front is the alias for begin
         * @ back is the iterator pointing to the last element
         * @ begin, end, front, back supports const and non-const versions
         * @ cbegin, cend, cfront, cback is the const version
         */
        iterator begin() noexcept;
        iterator end() noexcept;
        iterator front() noexcept;
        iterator back() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator front() const noexcept;
        const_iterator back() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        const_iterator cfront() const noexcept;
        const_iterator cback() const noexcept;

        /**
         * brief: size, the size of the container (i.e. the number of contents)
         * param: void
         * return: size_t
         */
        size_t size() const noexcept;

        /**
         * brief: push element at the back
         * param: the element
         * return: void
         */
        void push_back(const Elem& value) noexcept;

        /**
         * brief: push element at the front
         * param: the element
         * return: void
         */
        void push_front(const Elem& value) noexcept;

        /**
         * brief: pop element at the back
         * param: (void)
         * return: void
         */
        void pop_back();

        /**
         * brief: pop element at the front
         * param: (void)
         * return: void
         */
        void pop_front();

        /**
         * brief: insert element at a certain place
         * param: index (< 0 means the front) and the value
         * return: void
         */
        void insert_after(long long index, const Elem& value) noexcept;

        /**
         * brief: push element at the back (same as push_back)
         * param: the element
         * return: void
         */
        void append(const Elem& value) noexcept;

        /**
         * brief: remove element at a certain place
         * param: the index
         * return: void
         */
        Elem remove_at(size_t index);

        /**
         * brief: remove all elements
         * param: (void)
         * return: void
         */
        void clear() noexcept;

        /**
         * brief: if the vector is empty
         * param: (void)
         * return: bool
         */
        bool empty() noexcept;

        /**
         * brief: shrink, i.e. reduce the capacity
         * param: (void)
         * return: void
         */
        void shrink_to_fit() noexcept;

        /**
         * brief: if it is sorted in the order
         * param: bool (ASCENDING or DESCENDING, default as ASCENDING)
         * return: bool
         */
        bool sorted(bool is_ascending = ASCENDING) noexcept;

        /**
         * brief: sort (quick sort)
         * param: bool (ASCENDING or DESCENDING, default as ASCENDING)
         * return: void
         */
        void sort(bool is_ascending = ASCENDING);

        /**
         * brief: insertion sort
         * param: bool (ASCENDING or DESCENDING, default as ASCENDING)
         * return: void
         */
        void insertion_sort(bool is_ascending = ASCENDING);

        /**
         * brief: find
         * param: the element
         * return: the index of its first occurence
         */
        size_t find(const Elem& elem) const noexcept;

        /**
         * brief: find in a sorted vector
         * param: the element
         * return: the index of its first occurence
         */
        size_t search(const Elem& elem, bool is_ascending = ASCENDING) const noexcept;

        /**
         * brief: the read-only version of the element
         * param: size_t index
         * return: void
         */
        const Elem& at(size_t index) const;

        /**
         * brief: the read-and-write version of the element
         * param: size_t index
         * return: void
         */
        Elem& at(size_t index);

        /**
         * brief: the read-only version of the element
         * param: size_t index
         * return: void
         */
        const Elem& operator[](size_t index) const;

        /**
         * brief: the read-and-write version of the element
         * param: size_t index
         * return: void
         */
        Elem& operator[](size_t index);

        /**
         * brief: check if two vectors are the same
         * param: another vector the same element type
         * return: bool
         */
        bool operator==(const vector<Elem>& vec) const noexcept;

        /**
         * brief: check if two vectors are not the same
         * param: another vector the same element type
         * return: bool
         */
        bool operator!=(const vector<Elem>& vec) const noexcept;

        /**
         * ================== CAUTION ==================
         * This function should be used very carefully!
         * =============================================
         * brief: no delete,
         *        used especially when delete meets access violation
         * param: void
         * return: void
         */
        void no_delete();

    private:
        // the dynamic array that stores elements
        Elem* vec;

    protected:
        // the size of vec and should be always larger than size_ by at least 1
        size_t capacity_;

        // the number of valid elements
        size_t size_;

        /**
         * brief: expand the vector by double the capacity
         * param: (void)
         * return: void
         */
        void _expand();

        /**
         * brief: move elements from a certain index by a distance
         * param: the start index and the distance (right as positive)
         * return: void
         */
        void _move(size_t n, long long distance = 1);

        /**
         * brief: quick sort
         * param: the begin iterator and the end iterator and the order
         * return: void
         */
        void _quick_sort(iterator i_beg, iterator i_end, bool is_ascending = ASCENDING);

        /**
         * brief: insertion sort
         * param: the begin iterator and the end iterator and order
         * return: void
         */
        void _insertion_sort(iterator i_beg, iterator i_end, bool is_ascending = ASCENDING);

        /**
         * brief: medium
         * param: the medium one of the three
         * return: iterator
         */
        auto _medium(const iterator& a, const iterator& b, const iterator& c);

    };

    template<typename Elem>
    inline vector<Elem>::const_iterator::const_iterator() { }

    template<typename Elem>
    vector<Elem>::const_iterator::const_iterator(Elem* elem_, const vector<Elem>* parent_)
        : value(elem_), parent(parent_) { }

    template<typename Elem>
    inline vector<Elem>::const_iterator::~const_iterator() { }

    template<typename Elem>
    inline const Elem& vector<Elem>::const_iterator::operator*() const
    {
#ifndef NDEBUG
        if (!this->value)
            error_info("Null pointer in operator '*' of const_iterator of tvj::vector.", TVJ_VECTOR_NULLPTR);
        if (this->value - parent->vec < 0)
            error_info("Underflow in operator '*' of const_iterator of tvj::vector.", TVJ_VECTOR_UNDERFLOW);
        if (this->value - parent->vec >= parent->size_)
            error_info("Overflow in operator '*' of const_iterator of tvj::vector.", TVJ_VECTOR_OVERFLOW);
#endif
        return *value;
    }

    template<typename Elem>
    inline const Elem* vector<Elem>::const_iterator::operator->() const
    {
#ifndef NDEBUG
        if (!this->value)
            error_info("Null pointer in operator '->' of const_iterator of tvj::vector.", TVJ_VECTOR_NULLPTR);
        if (this->value - parent->vec < 0)
            error_info("Underflow in operator '->' of const_iterator of tvj::vector.", TVJ_VECTOR_UNDERFLOW);
        if (this->value - parent->vec >= parent->size_)
            error_info("Overflow in operator '->' of const_iterator of tvj::vector.", TVJ_VECTOR_OVERFLOW);
#endif
        return value;
    }

    template<typename Elem>
    inline auto vector<Elem>::const_iterator::operator++()
    {
        return const_iterator(++value, parent);
    }

    template<typename Elem>
    inline auto vector<Elem>::const_iterator::operator--()
    {
        return const_iterator(--value, parent);
    }

    template<typename Elem>
    inline auto vector<Elem>::const_iterator::operator++(int)
    {
        return const_iterator(value++, parent);
    }

    template<typename Elem>
    inline auto vector<Elem>::const_iterator::operator--(int)
    {
        return const_iterator(value--, parent);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::const_iterator::operator+(const unsigned n) const
    {
        return const_iterator(value + n, parent);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::const_iterator::operator-(const unsigned n) const
    {
        return const_iterator(value - n, parent);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::const_iterator::operator+=(const unsigned n)
    {
        value += n;
        return *this;
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::const_iterator::operator-=(const unsigned n)
    {
        value -= n;
        return *this;
    }

    template<typename Elem>
    inline auto vector<Elem>::const_iterator::operator-(const const_iterator& iter)
    {
        return this->value - iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator==(const const_iterator& iter) const noexcept
    {
        return this->value == iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator!=(const const_iterator& iter) const noexcept
    {
        return this->value != iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator<(const const_iterator& iter) const noexcept
    {
        return *this - iter < 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator>(const const_iterator& iter) const noexcept
    {
        return *this - iter > 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator<=(const const_iterator& iter) const noexcept
    {
        return *this - iter <= 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::const_iterator::operator>=(const const_iterator& iter) const noexcept
    {
        return *this - iter >= 0;
    }

    template<typename Elem>
    vector<Elem>::iterator::iterator() { }

    template<typename Elem>
    vector<Elem>::iterator::~iterator() { }

    template<typename Elem>
    vector<Elem>::iterator::iterator(const const_iterator& const_iter)
    {
        this->value = const_iter.value;
        this->parent = const_iter.parent;
    }

    template<typename Elem>
    inline Elem& vector<Elem>::iterator::operator*()
    {
#ifndef NDEBUG
        if (!const_iterator::value)
            error_info("Null pointer in operator '*' of iterator of tvj::vector.", TVJ_VECTOR_NULLPTR);
        if (const_iterator::value - const_iterator::parent->vec < 0)
            error_info("Underflow in operator '*' of iterator of tvj::vector.", TVJ_VECTOR_UNDERFLOW);
        if (const_iterator::value - const_iterator::parent->vec >= const_iterator::parent->size_)
            error_info("Overflow in operator '*' of iterator of tvj::vector.", TVJ_VECTOR_OVERFLOW);
#endif
        return *const_iterator::value;
    }

    template<typename Elem>
    inline Elem* vector<Elem>::iterator::operator->()
    {
#ifndef NDEBUG
        if (!const_iterator::value)
            error_info("Null pointer in operator '->' of iterator of tvj::vector.", TVJ_VECTOR_NULLPTR);
        if (const_iterator::value - const_iterator::parent->vec < 0)
            error_info("Underflow in operator '->' of iterator of tvj::vector.", TVJ_VECTOR_UNDERFLOW);
        if (const_iterator::value - const_iterator::parent->vec >= const_iterator::parent->size_)
            error_info("Overflow in operator '->' of iterator of tvj::vector.", TVJ_VECTOR_OVERFLOW);
#endif
        return const_iterator::value;
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::iterator::operator+(const unsigned n)
    {
        return iterator(const_iterator::value + n, const_iterator::parent);
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::iterator::operator-(const unsigned n)
    {
        return iterator(const_iterator::value - n, const_iterator::parent);
    }

    template<typename Elem>
    inline auto vector<Elem>::iterator::operator-(const iterator& iter)
    {
        return this->value - iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator==(const iterator& iter) const noexcept
    {
        return const_iterator::value == iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator!=(const iterator& iter) const noexcept
    {
        return const_iterator::value != iter.value;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator<(const iterator& iter) const noexcept
    {
        return const_iterator::value - iter.value < 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator>(const iterator& iter) const noexcept
    {
        return const_iterator::value - iter.value > 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator<=(const iterator& iter) const noexcept
    {
        return const_iterator::value - iter.value <= 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::iterator::operator>=(const iterator& iter) const noexcept
    {
        return const_iterator::value - iter.value >= 0;
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::begin() noexcept
    {
        return iterator(&vec[0], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::end() noexcept
    {
        return iterator(&vec[size_], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::front() noexcept
    {
        return iterator(&vec[0], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::iterator vector<Elem>::back() noexcept
    {
        return iterator(&vec[size_ - 1], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::begin() const noexcept
    {
        return const_iterator(&vec[0], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::end() const noexcept
    {
        return const_iterator(&vec[size_], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::front() const noexcept
    {
        return const_iterator(&vec[0], this);
    }

    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::back() const noexcept
    {
        return const_iterator(&vec[size_ - 1], this);
    }
    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::cbegin() const noexcept
    {
        return const_iterator(&vec[0], this);
    }
    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::cend() const noexcept
    {
        return const_iterator(&vec[size_], this);
    }
    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::cfront() const noexcept
    {
        return const_iterator(&vec[0], this);
    }
    template<typename Elem>
    inline typename vector<Elem>::const_iterator vector<Elem>::cback() const noexcept
    {
        return const_iterator(&vec[size_ - 1], this);
    }

    template<typename Elem>
    vector<Elem>::vector() : capacity_(32), size_(0), vec(new Elem[32]) { }

    template<typename Elem>
    vector<Elem>::vector(const vector<Elem>& new_vec)
        : capacity_(new_vec.capacity_), size_(new_vec.size_), vec(new Elem[new_vec.capacity_])
    {
        for (int i = 0; i != size_; i++)
        {
            vec[i] = new_vec[i];
        }
    }

#ifdef _MSC_VER

    template<typename Elem> template<typename _Iter>
    vector<Elem>::vector(const _Iter& iter1, typename std::enable_if<std::_Is_iterator<_Iter>::value, const _Iter&>::type iter2)
        : capacity_((iter2 - iter1) << 1), size_(iter2 - iter1), vec(new Elem[(iter2 - iter1) << 1])
    {
#ifndef NDEBUG
        if (iter2 - iter1 < 0) error_info("The order of iterators is illegal of tvj::vector constructor.", TVJ_VECTOR_ITER_RANGE);
#endif
        for (auto i = iter1; i != iter2; i++)
        {
            vec[i - iter1] = *i;
        }
    }

#else

    template<typename Elem> template<typename _Iter>
    inline vector<Elem>::vector(const _Iter& iter1, const _Iter& iter2)
    {
#ifndef NDEBUG
        if (iter2 - iter1 < 0) error_info("The order of iterators is illegal of tvj::vector constructor.", TVJ_VECTOR_ITER_RANGE);
#endif
        capacity_ = ((iter2 - iter1) << 1) + 1;
        size_     =   iter2 - iter1;
        vec       = new Elem[capacity_];
        for (auto i = iter1; i != iter2; i++)
        {
            vec[i - iter1] = *i;
        }
    }

#endif

    template<typename Elem>
    vector<Elem>::vector(size_t size, const Elem& elem, size_t capacity) : capacity_(capacity), size_(size)
    {
        if (capacity < 2) capacity = 2;
        while (capacity <= size) capacity <<= 1; // twice its size
        vec = new Elem[capacity];
        for (int i = 0; i != capacity; i++) vec[i] = elem;
    }


    template<typename Elem>
    vector<Elem>::~vector() // destructor
    {
        if(vec) delete[] vec; // free the dynamic array
        vec = nullptr;
    }

    template<typename Elem>
    inline size_t vector<Elem>::size() const noexcept
    {
        return size_;
    }

    template<typename Elem>
    inline void vector<Elem>::push_back(const Elem& value) noexcept
    {
        if (size_ + 1 == capacity_) _expand();
        vec[size_++] = value;
    }

    template<typename Elem>
    inline void vector<Elem>::push_front(const Elem& value) noexcept
    {
        if (size_ + 1 == capacity_) _expand();
        _move(0);
        vec[0] = value;
        size_++;
    }

    template<typename Elem>
    inline void vector<Elem>::pop_back()
    {
#ifndef NDEBUG
        if (size_ == 0) error_info("No element to pop in pop_back of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        size_--;
    }

    template<typename Elem>
    inline void vector<Elem>::pop_front()
    {
#ifndef NDEBUG
        if (size_ == 0) error_info("No element to pop in pop_front of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        _move(0, -1);
        size_--;
    }

    template<typename Elem>
    inline void vector<Elem>::insert_after(long long index, const Elem& value) noexcept
    {
        if (index < 0) push_front(value);
        else
        {
#ifndef NDEBUG
            if (index >= size_) error_info("Overflow in insert_after of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
            if (size_ + 1 == capacity_) _expand();
            _move(index);
            vec[index] = value;
        }
    }

    template<typename Elem>
    inline void vector<Elem>::append(const Elem& value) noexcept
    {
        this->push_back(value);
    }

    template<typename Elem>
    inline Elem vector<Elem>::remove_at(size_t index)
    {
#ifndef NDEBUG
        if (index >= size_) error_info("Overflow in remove_at of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        auto ret = vec[index];
        _move(index + 1, -1);
        size_--;
        return ret;
    }

    template<typename Elem>
    inline void vector<Elem>::clear() noexcept
    {
        size_ = 0;
    }

    template<typename Elem>
    inline bool vector<Elem>::empty() noexcept
    {
        return !this->size_;
    }

    template<typename Elem>
    inline void vector<Elem>::shrink_to_fit() noexcept
    {
        capacity_ = size_ + 1;
        Elem* new_vec = new Elem[capacity_];
        for (size_t i = 0; i != size_; i++)
            new_vec[i] = vec[i];
        delete[] vec;
        vec = new_vec;
    }

    template<typename Elem>
    inline bool vector<Elem>::sorted(bool is_ascending) noexcept
    {
        for (size_t i = 0; i < size_ - 1; i++)
        {
            if (!_compare(vec[i], vec[i + 1], is_ascending)) return false;
        }
        return true;
    }

    template<typename Elem>
    inline void vector<Elem>::sort(bool is_ascending)
    {
        if (size_ < 2) return;
        _quick_sort(begin(), end(), is_ascending);
    }

    template<typename Elem>
    inline void vector<Elem>::insertion_sort(bool is_ascending)
    {
        if (size_ < 2) return;
        _insertion_sort(begin(), end(), is_ascending);
    }

    template<typename Elem>
    inline size_t vector<Elem>::find(const Elem& elem) const noexcept
    {
        for (size_t i = 0; i != this->size_; i++)
        {
            if (vec[i] == elem) return i;
        }
        return this->size_;
    }

    template<typename Elem>
    inline size_t vector<Elem>::search(const Elem& elem, bool is_ascending) const noexcept
    {
        if (size_ == 0) return 0;

#ifndef NDEBUG
        // If the vector is not sorted
        if (!sorted(is_ascending))
        {
            error_info("Use search function in an unsorted tvj::vector. Try using function find.", TVJ_VECTOR_OTHER);
        }
#endif

        size_t L = 0; size_t R = size_;
        while (1)
        {
            if (R >= L) return size_;
            if (vec[(R + L) / 2] == elem) return (R + L) / 2;
            if ((vec[(R + L) / 2] > elem) ^ is_ascending)
            {
                // left part
                R = (R + L) / 2;
            }
            else
            {
                // right part
                L = (R + L) / 2 + 1;
            }
        }
    }

    template<typename Elem>
    inline const Elem& vector<Elem>::at(size_t index) const
    {
#ifndef NDEBUG
        if (index > size_) error_info("Overflow in at of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        return vec[index];
    }

    template<typename Elem>
    inline Elem& vector<Elem>::at(size_t index)
    {
#ifndef NDEBUG
        if (index > size_) error_info("Overflow in at of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        return vec[index];
    }

    template<typename Elem>
    inline const Elem& vector<Elem>::operator[](size_t index) const
    {
#ifndef NDEBUG
        if (index > size_) error_info("Overflow in operator[] of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        return vec[index];
    }

    template<typename Elem>
    inline Elem& vector<Elem>::operator[](size_t index)
    {
#ifndef NDEBUG
        if (index > size_) error_info("Overflow in operator[] of tvj::vector", TVJ_VECTOR_OVERFLOW);
#endif
        return vec[index];
    }

    template<typename Elem>
    inline bool vector<Elem>::operator==(const vector<Elem>& another_vec) const noexcept
    {
        if (this->size_ != another_vec.size_) return false;
        for (size_t i = 0; i != this->size_; i++)
        {
            if ((*this)[i] != another_vec[i]) return false;
        }
        return true;
    }

    template<typename Elem>
    inline bool vector<Elem>::operator!=(const vector<Elem>& another_vec) const noexcept
    {
        return !(*this == another_vec);
    }

    template<typename Elem>
    inline void vector<Elem>::no_delete()
    {
        vec = nullptr;
    }

    template<typename Elem>
    inline void vector<Elem>::_expand()
    {
        // 1. ask for new space for the array
        Elem* new_vec = new Elem[capacity_ << 1];
        // 2. copy the values over
        for (size_t i = 0; i != size_; i++)
            new_vec[i] = vec[i];
        // 3. delete the old array
        delete[] vec;
        // 4. point vec to new array
        vec = new_vec;
        // 5. update capacity (twice the capacity)
        capacity_ <<= 1;
    }

    template<typename Elem>
    inline void vector<Elem>::_move(size_t start_index, long long distance)
    {
        if (distance == 0) return;
        if (distance > 0) // move right
            for (size_t i = size_ + distance - 1; i != start_index; i--) vec[i] = vec[i - distance];
        else // move left
            for (size_t i = start_index; i != size_; i++) vec[i + distance] = vec[i];
    }

    template<typename Elem>
    inline void vector<Elem>::_quick_sort(iterator i_beg_, iterator i_end_, bool is_ascending)
    {
        iterator i_beg = i_beg_;
        iterator i_end = i_end_;
        if (i_end - i_beg <= 1) return;
        else if (i_end - i_beg == 2)
        {
            if (!_compare(*i_beg, *(i_end - 1), is_ascending))
            {
                _swap(*i_beg, *(i_end - 1));
            }
        }
        else if (i_end - i_beg <= 7)
        {
            // If the number is not large,
            // insertion sort can be more efficient.
            _insertion_sort(i_beg, i_end, is_ascending);
            return;
        }
        else
        {
            // Choose the medium one of the three numbers,
            // in order to avoid the circumstance that
            // the standard number is too large or too small
            // when quicksort can be reduced from o(nlog(n)) to o(N^2).
            iterator standard_number = _medium(i_beg, i_end - 1, i_beg + ((i_end - i_beg) - 1) / 2);
            auto init_begin = i_beg;
            auto init_end = i_end;

            auto Standard_defined = *standard_number;
            _swap(*standard_number, *init_begin);
            while (i_beg != i_end)
            {
                do { --i_end; } while (!_compare(*i_end, Standard_defined, is_ascending) && i_beg < i_end);
                if (i_beg == i_end) break;
                else
                {
                    do { ++i_beg; } while (_compare(*i_beg, Standard_defined, is_ascending) && i_beg < i_end);
                    _swap(*i_beg, *i_end);
                }
            }
            _swap(*init_begin, *i_beg);
            _quick_sort(init_begin, i_beg);
            _quick_sort(i_beg + 1, init_end);
        }
    }

    template<typename Elem>
    inline void vector<Elem>::_insertion_sort(iterator i_beg, iterator i_end, bool is_ascending)
    {
        for (iterator i = i_beg + 1; i != i_end; i++)
        {
            auto temp = *i;
            // slide elements right to make room for v[i]
            auto j = i;
            while (j >= i_beg + 1 && !_compare(*(j - 1), temp, is_ascending))
            {
                *(j--) = *(j - 1);
            }
            *j = temp;
        }
    }

    template<typename Elem>
    inline auto vector<Elem>::_medium(const iterator& a, const iterator& b, const iterator& c)
    {
        // if ((*a <= *b && *a >= *c) || (*a >= *b && *a <= *c)) return a;
        // if ((*b <= *a && *b >= *c) || (*b >= *a && *b <= *c)) return b;
        return c;
    }

    // Using this function will sort this vector
    // and the parameter vector at the same time.
    //auto merge(vector<T>& vec)
    //{
    //	this->sort();
    //	vec.sort();
    //	vector<T> merged(this->size() + vec.size());
    //	auto i = this->cbegin(), j = vec.cbegin();
    //	while (1)
    //	{
    //		T to_push_value = 0;
    //		if (i == this->cend())
    //		{
    //			if (j == vec.cend()) break;
    //			else to_push_value = *j++;
    //		}
    //		else
    //		{
    //			if (j == vec.cend()) to_push_value = *i++;
    //			else
    //			{
    //				to_push_value = *i < *j ? *i++ : *j++;
    //			}
    //		}
    //		if (merged.empty() || to_push_value != *(merged.cend() - 1)) merged.push_back(to_push_value);
    //	}
    //	return merged;
    //}

    /**
     * brief: delete vectors with same content pointers,
     *        otherwise it can delete on the same pointer and leads to errors
     * param: array or container of [pointers to vectors]
     * return: void
     */
    template<typename T>
    void delete_vectors_with_same_content_pointers(size_t size, T array_of_vectors)
    {
        if (size < 1) return;
        auto delete_later__ = *array_of_vectors[0];
        for (size_t i = 0; i != size; i++)
        {
            array_of_vectors[i]->vec = nullptr; // avoid being deleted
        }
        // delete_later__ will be deleted implicitly later
    }
}

// ALL RIGHTS RESERVED (C) 2021 Teddy van Jerry