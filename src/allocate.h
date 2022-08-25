#ifndef ALLOCATE_H_
#define ALLOCATE_H_

#include "memory_pool.h"

template <typename T>
class Allocator
{
public:
    
    // Member types
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef char data_type;
    typedef char* data_pointer;

    // 为了达成 vector 内置的效果
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::true_type  propagate_on_container_move_assignment;
    typedef std::true_type  propagate_on_container_swap;

    // todo
    template <typename U>
    struct rebind {
        typedef MemoryPool<U> other;
    };

    Allocator() noexcept;

    // todo
    template <typename U>
    Allocator(const Allocator<U> &rhs) noexcept;
    Allocator(Allocator &&rhs) noexcept;

    Allocator& operator=(const Allocator &rhs) noexcept;
    Allocator& operator=(Allocator &&rhs) noexcept;

    // == and != operators
    template<typename U>
    bool operator==(const Allocator<U>&) const noexcept;

    template<typename U>
    bool operator!=(const Allocator<U>&) const noexcept;

    pointer allocate(size_type n = 1, const_pointer hint = 0);
    void deallocate(pointer p, size_type n = 1);

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);

    template <typename U>
    void destroy(U* p);

private:
    
};

#include "allocate.tcc"

#endif // !ALLOCATE_H_