#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include <QObject>
#include <type_traits>
#include <vector>
#include <iostream>

template <typename T>

class PoolAllocator : public QObject
{
    Q_OBJECT
public:
using value_type = T;
PoolAllocator(): pool_(reinterpret_cast<uint8_t*>(::operator new(pool_size_))),pool_begin_(pool_){std::cout<<"PoolAllocator()\n";pool_begin_=pool_;}
~PoolAllocator(){
    std::cout<<"~PoolAllocator()\n";
    ::operator delete(pool_begin_);
}

template <typename... Args>
void construct(T* ptr, const Args&... args) {
      std::cout<<"construct()\n";
    new(ptr) T(args...);
}
void destroy(T* ptr) noexcept {
           std::cout<<"destroy()\n";
    ptr->~T();
}

T* allocate(size_t n){
                  std::cout<<"allocate() "<<n<<" sizeof "<<sizeof(T)<<"\n";
    size_t bytes = n * sizeof(T);
    auto memory_to_return = pool_;
    pool_ += bytes;

    return reinterpret_cast<T*> (memory_to_return);
}

void deallocate(T* ptr, size_t n){
                          std::cout<<"deallocate()\n";
    return;
//    ::operator delete(ptr);
}
private:
const size_t pool_size_ = 100000;
uint8_t* pool_ = nullptr;
uint8_t* pool_begin_ = nullptr;
};

#endif // POOLALLOCATOR_H
