#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <new> // placement new
#include <utility> // std::swap

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
public:
    // 定义基本类型
    typedef T               value_type;
    typedef T*              pointer;
    typedef T&              reference;
    typedef const T*        const_pointer;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    // 为了达成 vector 内置的效果
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::true_type  propagate_on_container_move_assignment;
    typedef std::true_type  propagate_on_container_swap;

    // todo
    template <typename U>
    struct rebind {
        typedef MemoryPool<U> other;
    };

    MemoryPool() noexcept;
    // todo
    MemoryPool(const MemoryPool &rhs) noexcept;
    MemoryPool(MemoryPool &&rhs) noexcept;

    ~MemoryPool() noexcept;

    MemoryPool& operator=(const MemoryPool &rhs) = delete;
    MemoryPool& operator=(MemoryPool &&rhs) noexcept;

    pointer address(reference x) const noexcept;
    const_pointer address(const_reference x) const noexcept;
    size_type max_size() const noexcept;

    pointer allocate(size_type n = 1, const_pointer hint = 0);
    void deallocate(pointer p, size_type n = 1);
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);

    template <typename U>
    void destroy(U* p);

    template <typename... Args>
    pointer newElement(Args&&... args);
    void deleteElement(pointer p);

private:
    union Slot_ {
        value_type element;
        Slot_* next;
    };

    typedef char*               data_pointer_;
    typedef Slot_               slot_type_;
    typedef Slot_*              slot_pointer_;

    slot_pointer_ currentBlock_;
    slot_pointer_ currentSlot_;
    slot_pointer_ lastSlot_;
    slot_pointer_ freeSlots_;

    size_type padPointer(data_pointer_ p, size_type align) const noexcept;
    void allocateBlock();

    static_assert(BlockSize >= 2*sizeof(slot_type_), "BlockSize too small.");
};

#include "memory_pool.tcc"

#endif // !MEMORY_POOL_H_