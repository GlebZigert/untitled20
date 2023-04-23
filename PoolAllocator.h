#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include <QObject>
#include <type_traits>
#include <vector>
#include <iostream>
#include <QDebug>

template <typename T>

class PoolAllocator
{

public:
using value_type = T;
PoolAllocator(): pool_(reinterpret_cast<uint8_t*>(::operator new(pool_size_))),pool_begin_(pool_){pool_begin_=pool_;qDebug()<<"PoolAllocator()\n";}
~PoolAllocator(){
    std::cout<<"~PoolAllocator()\n";
    ::operator delete(pool_begin_);
}

template <typename... Args>
void construct(T* ptr, const Args&... args) {
      qDebug()<<"construct()\n";
    new(ptr) T(args...);
}
void destroy(T* ptr) noexcept {
      qDebug()<<"destroy()\n";
    ptr->~T();
}

T* allocate(size_t n){
      qDebug()<<"allocate() "<<n<<" sizeof "<<sizeof(T)<<"\n";
    size_t bytes = n * sizeof(T);
    auto memory_to_return = pool_;
    pool_ += bytes;

    if(pool_begin_+pool_size_-pool_<bytes){

        qDebug()<<"места больше нет";
        return nullptr;
    }

    qDebug()<<"pool_ "<<pool_<<" from "<<(pool_begin_+pool_size_)<<" осталось "<<(pool_begin_+pool_size_-pool_);

    return reinterpret_cast<T*> (memory_to_return);
}

void deallocate(T* ptr, size_t n){
      qDebug()<<"deallocate()\n";
    return;
//    ::operator delete(ptr);
}
private:
const size_t pool_size_ = 100000;
uint8_t* pool_ = nullptr;
uint8_t* pool_begin_ = nullptr;
};

#endif // POOLALLOCATOR_H
