#ifndef ALLOCATE_TCC_
#define ALLOCATE_TCC_

template <typename T>
Allocator<T>::Allocator()
noexcept {}

template <typename T>
template <typename U>
Allocator<T>::Allocator(const Allocator<U> &rhs)
noexcept: Allocator() {}

template <typename T>
Allocator<T>::Allocator(Allocator &&rhs)
noexcept {}

template <typename T>
Allocator<T>& Allocator<T>::operator=(const Allocator<T> &rhs) noexcept {}

template <typename T>
Allocator<T>& Allocator<T>::operator=(Allocator<T> &&rhs) noexcept {}

template <typename T>
template<typename U>
bool Allocator<T>::operator==(const Allocator<U>&)
const noexcept {
    return true;
}

template <typename T>
template<typename U>
bool Allocator<T>::operator!=(const Allocator<U>&)
const noexcept {
    return false;
}

template <typename T>
inline typename Allocator<T>::pointer
Allocator<T>::allocate(size_type n, const_pointer hint) {
    return reinterpret_cast<pointer>(MemoryPool<T>::getInstance().allocate(n, hint));
}

template <typename T>
inline void
Allocator<T>::deallocate(pointer p, size_type n) {
    MemoryPool<T>::getInstance().deallocate(p, n);
}

#endif // !ALLOCATE_TCC_