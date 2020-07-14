#ifndef INCLUDE_IMAGE_H
#define INCLUDE_IMAGE_H

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

namespace aslib {
    template < class, class=void >
    struct is_iterator : std::false_type {};

    template < typename T >
    struct is_iterator<T,
        std::enable_if_t<
        std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value
        || std::is_base_of<std::output_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value
        >
    > : std::true_type
    {};

    template <class TImage>
    class image {
    public:
        using iterator = TImage*;
        using const_iterator = const TImage*;
        using reference = TImage&;
        using const_reference = const TImage&;
    public:
        image(std::vector<std::byte>& mem, std::uint32_t start, std::uint32_t length);
        bool empty() const noexcept;
        std::size_t size() const noexcept;
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        reference operator[](std::size_t i);
        const_reference operator[](std::size_t i) const;
        reference at(std::size_t i);
        const_reference at(std::size_t i) const;
        void push_back(const TImage& obj);
        void push_back(TImage&& obj);
        void pop_back();
        iterator insert(const_iterator position, const TImage& obj);
        iterator insert(const_iterator position, TImage&& obj);
        iterator insert(const_iterator position, std::size_t n, const TImage& obj);
        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last);
        iterator insert(const_iterator position, std::initializer_list<TImage> il);
        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);

        image(const image<TImage>& obj) = delete;
        image(image<TImage>&& obj) = delete;
        image<TImage>& operator=(const image<TImage>& obj) = delete;        
        image<TImage>* operator&() = delete;
        static void* operator new(size_t size) = delete;
        static void operator delete(void *ptr) = delete;
        static void* operator new[](size_t size) = delete;
        static void operator delete[](void *ptr) = delete;
        static void* operator new(std::size_t size, const std::nothrow_t&) noexcept = delete;
        static void* operator new[](std::size_t size, const std::nothrow_t&) noexcept = delete;
        static void* operator new(std::size_t, void*) = delete;
        static void* operator new[](std::size_t size, void* ptr) noexcept = delete;
    private:
        std::vector<std::byte>& memory;
        std::uint32_t start;
        std::uint32_t count;
    private:
        bool _M_range_check(std::size_t i) const;
    };

    template <class TImage>
    inline image<TImage>::image(std::vector<std::byte>& mem, std::uint32_t start, std::uint32_t count) :
        memory(mem),
        start(start),
        count(count)
    {
        static_assert(std::is_trivially_copyable<TImage>::value == true, "TImage is not trivially copyable");

        if(start + count * sizeof(TImage) > memory.size()) memory.resize(start + count * sizeof(TImage));
    }

    template <class TImage>
    inline bool image<TImage>::empty() const noexcept {
        return !count;
    }

    template <class TImage>
    inline std::size_t image<TImage>::size() const noexcept {
        return count;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::begin() noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start);
    }

    template <class TImage>
    inline typename image<TImage>::const_iterator image<TImage>::begin() const noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start);
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::end() noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start + count * sizeof(TImage));
    }

    template <class TImage>
    inline typename image<TImage>::const_iterator image<TImage>::end() const noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start + count * sizeof(TImage));
    }

    template <class TImage>
    inline typename image<TImage>::reference image<TImage>::operator[](std::size_t i) {
        return *reinterpret_cast<TImage*>(memory.data() + start + i * sizeof(TImage));
    }

    template <class TImage>
    inline typename image<TImage>::const_reference image<TImage>::operator[](std::size_t i) const {
        return *reinterpret_cast<TImage*>(memory.data() + start + i * sizeof(TImage));
    }

    template <class TImage>
    inline typename image<TImage>::reference image<TImage>::at(std::size_t i) {
        if(i >= count) {
            std::ostringstream oss;
            oss << "image::_M_range_check: i (which is " << i << ") >= this->size() (which is " << size() << ")";
            throw std::out_of_range(oss.str());
        }
        return operator[](i);
    }

    template <class TImage>
    inline typename image<TImage>::const_reference image<TImage>::at(std::size_t i) const {
        if(i >= count) {
            std::ostringstream oss;
            oss << "image::_M_range_check: i (which is " << i << ") >= this->size() (which is " << size() << ")";
            throw std::out_of_range(oss.str());
        }
        return operator[](i);
    }

    template <class TImage>
    inline void image<TImage>::push_back(const TImage& obj) {
        const std::byte* mem = reinterpret_cast<const std::byte*>(&obj);
        auto c = sizeof(TImage);
        memory.insert(memory.begin() + start + count * c, mem, mem + c);
        count++;
    }

    template <class TImage>
    inline void image<TImage>::push_back(TImage&& obj) {
        const std::byte* mem = reinterpret_cast<const std::byte*>(&obj);
        auto c = sizeof(TImage);
        memory.insert(memory.begin() + start + count * c, mem, mem + c);
        count++;
    }

    template <class TImage>
    inline void image<TImage>::pop_back() {
        if(count == 0) return;
        auto c = sizeof(TImage);
        memory.erase(memory.begin() + start + count * c - c, memory.begin() + start + count * c);
        count--;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::insert(typename image<TImage>::const_iterator position, const TImage& obj) {
        const std::byte* mem = reinterpret_cast<const std::byte*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage);
        memory.insert(memory.begin() + offset, mem, mem + c);
        count++;
        return memory.begin() + offset;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::insert(typename image<TImage>::const_iterator position, TImage&& obj) {
        const std::byte* mem = reinterpret_cast<const std::byte*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage);
        memory.insert(memory.begin() + offset, mem, mem + c);
        count++;
        return begin() + d;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::insert(typename image<TImage>::const_iterator position, std::size_t n, const TImage& obj) {
        const std::byte* mem = reinterpret_cast<const std::byte*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage);
        for(int i = 0; i < n; i++) {
            memory.insert(memory.begin() + offset, mem, mem + c);
        }
        count += n;
        return begin() + d;
    }

    template <class TImage>
    template <class InputIterator>
    inline typename image<TImage>::iterator image<TImage>::insert(typename image<TImage>::const_iterator position, InputIterator first, InputIterator last) {
        static_assert(is_iterator<InputIterator>::value == true, "InputIterator is not iteratable");
        static_assert(std::is_same<TImage&, decltype(*first)>::value == true 
            || std::is_same<TImage, decltype(*first)>::value == true
            || std::is_same<const TImage&, decltype(*first)>::value == true, "Iterator content is not TImage");
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage) ;
        auto i = 0;
        for(auto iter = first; iter != last; iter++) {
            const std::byte* mem = reinterpret_cast<const std::byte*>(&(*iter));
            memory.insert(memory.begin() + offset + i * c, mem, mem + c);
            i++;
        }
        count += last - first;
        return begin() + d;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::insert(typename image<TImage>::const_iterator position, std::initializer_list<TImage> il) {
        return insert(position, il.begin(), il.end());
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::erase(typename image<TImage>::const_iterator position) {
        auto d = position - begin();
        if(d >= count) return end();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage);
        memory.erase(memory.begin() + offset, memory.begin() + offset + c);
        count--;
        return begin() + d;
    }

    template <class TImage>
    inline typename image<TImage>::iterator image<TImage>::erase(typename image<TImage>::const_iterator first, typename image<TImage>::const_iterator last) {
        auto d = first - begin();
        if(d >= count || last - begin() < first - begin()) return end();
        if(last - begin() > count) last = end();
        auto offset = start + d * sizeof(TImage);
        auto c = sizeof(TImage);
        auto n = last - first;
        memory.erase(memory.begin() + offset, memory.begin() + offset + c * n);
        count -= n;
        return begin() + d;
    }

    template <class TImage>
    inline bool image<TImage>::_M_range_check(std::size_t i) const {
        return i < size();
    }
}


#endif