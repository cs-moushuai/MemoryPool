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
    // 单例模式调用getinstance存在线程安全问题，因为局部静态变量在第一次调用getinstance时才实例化
    // c++11 保证不会出现这个问题，如果是之前的就要加锁或者...
    // https://ost.51cto.com/posts/670
    return reinterpret_cast<pointer>(MemoryPool<T>::getInstance().allocate(n, hint));
}

template <typename T>
inline void
Allocator<T>::deallocate(pointer p, size_type n) {
    MemoryPool<T>::getInstance().deallocate(p, n);
}

#endif // !ALLOCATE_TCC_