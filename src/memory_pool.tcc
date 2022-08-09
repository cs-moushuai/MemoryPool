#ifndef MEMORY_POOL_TCC_
#define MEMORY_POOL_TCC_


template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool()
noexcept {
    currentBlock_ = nullptr;
    currentSlot_ = nullptr;
    lastSlot_ = nullptr;
    freeSlots_ = nullptr;
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool& rhs)
noexcept: MemoryPool() {}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(MemoryPool &&rhs)
noexcept {
    currentBlock_ = rhs.currentBlock_;
    // 移动过来，且收回原对象的控制权
    rhs.currentBlock_ = nullptr;
    currentSlot_ = rhs.currentSlot_;
    lastSlot_ = rhs.lastSlot_;
    freeSlots_ = rhs.freeSlots_;
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool()
noexcept {
    // 不要写 currentSlot_ 会将值当作地址
    slot_pointer_ curr = currentBlock_;
    while (curr) {
        slot_pointer_ t = curr->next;
        // 转化为 void* 避免调用 dtor
        operator delete(reinterpret_cast<void*>(curr));
        curr = t;
    }
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>&
MemoryPool<T, BlockSize>::operator=(MemoryPool &&rhs)
noexcept {
    if (this != &rhs) {
        std::swap(currentBlock_, rhs.currentBlock_);
        currentSlot_ = rhs.currentSlot_;
        lastSlot_ = rhs.lastSlot_;
        freeSlots_ = rhs.freeSlots_;
    }
    return *this;
}

template <typename T, size_t BlockSize>
template<typename U>
bool MemoryPool<T, BlockSize>::operator==(const MemoryPool<U>&)
const noexcept {
    return true;
}

template <typename T, size_t BlockSize>
template<typename U>
bool MemoryPool<T, BlockSize>::operator!=(const MemoryPool<U>&)
const noexcept {
    return false;
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::address(reference x)
const noexcept {
    return &x;
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::const_pointer
MemoryPool<T, BlockSize>::address(const_reference x)
const noexcept {
    return &x;
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::max_size()
const noexcept {
    size_type maxBlocks = -1 / sizeof(data_pointer_);
    // 每个块中 slots 的大小 * 最大块数
    return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::allocate(size_type n, const_pointer hint) {
    if (freeSlots_) {
        pointer res = reinterpret_cast<pointer>(freeSlots_);
        freeSlots_ = freeSlots_->next;
        return res;
    } else {
        // todo 为什么优先使用 freeSlot_ 中的块，那当前的块不就浪费了吗
        // answer：freeSlots 存放的是 slot 不是 block，不影响当前块
        if (currentSlot_ >= lastSlot_)
            allocateBlock();
        return reinterpret_cast<pointer>(currentSlot_++);
    }
}

template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deallocate(pointer p, size_type n) {
    if (p) {
        reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
        freeSlots_ = reinterpret_cast<slot_pointer_>(p);
    }
}

template <typename T, size_t BlockSize>
template <typename U, typename... Args>
inline void
MemoryPool<T, BlockSize>::construct(U* p, Args&&... args) {
    // todo 为什么类型是 U 不是 T
    // answer：U 是 value_type，本质也是 T
    new (p) U(std::forward<Args>(args)...);
}

template <typename T, size_t BlockSize>
template <typename U>
inline void
MemoryPool<T, BlockSize>::destroy(U* p) {
    if (p) {
        p->~U();
    }
}

template <typename T, size_t BlockSize>
template <typename... Args>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::newElement(Args&&... args) {
    pointer p = allocate();
    // todo 需要显式指定 construct 类型吗
    construct<value_type>(p, std::forward<Args>(args)...);
    return p;
}

template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deleteElement(pointer p) {
    if (p) {
        p->~value_type();
        deallocate(p);
    }
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::padPointer(data_pointer_ p, size_type align)
const noexcept {
    uintptr_t res = reinterpret_cast<uintptr_t>(p);
    // todo 为什么这样写
    return (align - res) % align;
}

template <typename T, size_t BlockSize>
void MemoryPool<T, BlockSize>::allocateBlock() {
    // 分配新 Block
    data_pointer_ newBlock = reinterpret_cast<data_pointer_>
                            (operator new(BlockSize));
    // 头插法
    reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
    currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
    // 保留一个大小指向下一个区块的地址，计算 body 位置
    // 用 currentBlock_ 要类型转换
    data_pointer_ body = newBlock + sizeof(slot_pointer_);
    // 计算 body 中填充位置
    size_type padding = padPointer(body, alignof(slot_type_));
    // currentSlot_ 指向（body+padding）头，lastSlot_ 指向最后一个位置地址 +1
    currentSlot_ = reinterpret_cast<slot_pointer_>(body + padding);
    lastSlot_ = reinterpret_cast<slot_pointer_>
                (newBlock + BlockSize - sizeof(slot_type_) + 1);
}

#endif // !MEMORY_POOL_TCC_