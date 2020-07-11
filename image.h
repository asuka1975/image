#ifndef INCLUDE_IMAGE_H
#define INCLUDE_IMAGE_H

#include <cstdint>
#include <iterator>
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

    template <class TMemory, class TImage>
    class image {
    public:
        using iterator = TImage*;
        using const_iterator = const TImage*;
    public:
        image(std::vector<TMemory>& mem, std::uint32_t start, std::uint32_t length);
        bool empty() const noexcept;
        std::size_t size() const noexcept;
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
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

        image<TMemory, TImage>* operator&() = delete;
        static void* operator new(size_t size) = delete;
        static void operator delete(void *ptr) = delete;
        static void* operator new[](size_t size) = delete;
        static void operator delete[](void *ptr) = delete;
        static void* operator new(std::size_t size, const std::nothrow_t&) noexcept = delete;
        static void* operator new[](std::size_t size, const std::nothrow_t&) noexcept = delete;
        static void* operator new(std::size_t, void*) = delete;
        static void* operator new[](std::size_t size, void* ptr) noexcept = delete;
    private:
        std::vector<TMemory>& memory;
        std::uint32_t start;
        std::uint32_t count;
    };

    template <class TMemory, class TImage>
    inline image<TMemory, TImage>::image(std::vector<TMemory>& mem, std::uint32_t start, std::uint32_t count) :
        memory(mem),
        start(start),
        count(count)
    {
        static_assert(std::is_unsigned<TMemory>::value && std::is_integral<TMemory>::value == true, "TMemory doesn't express memory array");
        static_assert(std::is_trivially_copyable<TImage>::value == true, "TImage is not trivially copyable");
        static_assert(sizeof(TMemory) <= sizeof(TImage), "TMemories can't express TImage memory layout by size error");
        static_assert(sizeof(TImage) % sizeof(TMemory) == 0, "TMemories set can't match TImage memory layout");

        if(start + count * sizeof(TImage) / sizeof(TMemory) > memory.size()) memory.resize(start + count * sizeof(TImage) / sizeof(TMemory));
    }

    template <class TMemory, class TImage>
    inline bool image<TMemory, TImage>::empty() const noexcept {
        return !count;
    }

    template <class TMemory, class TImage>
    inline std::size_t image<TMemory, TImage>::size() const noexcept {
        return count;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::begin() noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start);
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::const_iterator image<TMemory, TImage>::begin() const noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start);
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::end() noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start + count * sizeof(TImage) / sizeof(TMemory));
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::const_iterator image<TMemory, TImage>::end() const noexcept {
        return reinterpret_cast<TImage*>(memory.data() + start + count * sizeof(TImage) / sizeof(TMemory));
    }

    template <class TMemory, class TImage>
    inline void image<TMemory, TImage>::push_back(const TImage& obj) {
        const TMemory* mem = reinterpret_cast<const TMemory*>(&obj);
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.insert(memory.begin() + start + count * c, mem, mem + c);
        count++;
    }

    template <class TMemory, class TImage>
    inline void image<TMemory, TImage>::push_back(TImage&& obj) {
        const TMemory* mem = reinterpret_cast<const TMemory*>(&obj);
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.insert(memory.begin() + start + count * c, mem, mem + c);
        count++;
    }

    template <class TMemory, class TImage>
    inline void image<TMemory, TImage>::pop_back() {
        if(count == 0) return;
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.erase(memory.begin() + start + count * c - c, memory.begin() + start + count * c);
        count--;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::insert(typename image<TMemory, TImage>::const_iterator position, const TImage& obj) {
        const TMemory* mem = reinterpret_cast<const TMemory*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.insert(memory.begin() + offset, mem, mem + c);
        count++;
        return memory.begin() + offset;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::insert(typename image<TMemory, TImage>::const_iterator position, TImage&& obj) {
        const TMemory* mem = reinterpret_cast<const TMemory*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.insert(memory.begin() + offset, mem, mem + c);
        count++;
        return begin() + d;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::insert(typename image<TMemory, TImage>::const_iterator position, std::size_t n, const TImage& obj) {
        const TMemory* mem = reinterpret_cast<const TMemory*>(&obj);
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        for(int i = 0; i < n; i++) {
            memory.insert(memory.begin() + offset, mem, mem + c);
        }
        count += n;
        return begin() + d;
    }

    template <class TMemory, class TImage>
    template <class InputIterator>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::insert(typename image<TMemory, TImage>::const_iterator position, InputIterator first, InputIterator last) {
        static_assert(is_iterator<InputIterator>::value == true, "InputIterator is not iteratable");
        static_assert(std::is_same<TImage&, decltype(*first)>::value == true 
            || std::is_same<TImage, decltype(*first)>::value == true
            || std::is_same<const TImage&, decltype(*first)>::value == true, "Iterator content is not TImage");
        auto d = position - begin();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        auto i = 0;
        for(auto iter = first; iter != last; iter++) {
            const TMemory* mem = reinterpret_cast<const TMemory*>(&(*iter));
            memory.insert(memory.begin() + offset + i * c, mem, mem + c);
            i++;
        }
        count += last - first;
        return begin() + d;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::insert(typename image<TMemory, TImage>::const_iterator position, std::initializer_list<TImage> il) {
        return insert(position, il.begin(), il.end());
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::erase(typename image<TMemory, TImage>::const_iterator position) {
        auto d = position - begin();
        if(d >= count) return end();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        memory.erase(memory.begin() + offset, memory.begin() + offset + c);
        count--;
        return begin() + d;
    }

    template <class TMemory, class TImage>
    inline typename image<TMemory, TImage>::iterator image<TMemory, TImage>::erase(typename image<TMemory, TImage>::const_iterator first, typename image<TMemory, TImage>::const_iterator last) {
        auto d = first - begin();
        if(d >= count || last - begin() < first - begin()) return end();
        if(last - begin() > count) last = end();
        auto offset = start + d * sizeof(TImage) / sizeof(TMemory);
        auto c = sizeof(TImage) / sizeof(TMemory);
        auto n = last - first;
        memory.erase(memory.begin() + offset, memory.begin() + offset + c * n);
        count -= n;
        return begin() + d;
    }
}


#endif