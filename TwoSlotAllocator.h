#ifndef TWOSLOTALLOCATOR_H
#define TWOSLOTALLOCATOR_H



#include <QObject>
#include <type_traits>
#include <vector>
#include <iostream>
#include <QDebug>

template <typename T>

class TwoSlotAllocator
{

public:
using value_type = T;
TwoSlotAllocator(size_t size): pool_(reinterpret_cast<uint8_t*>(::operator new(size*2))),pool_begin_(pool_){


    pool_begin_=pool_;

    qDebug()<<"TwoSlotAllocator() на два слота размера "<<size;
    size_ = size;

    pool_first_=pool_begin_;
    pool_second_=pool_begin_+size_;
    first_free=true;
    second_free=true;


}
~TwoSlotAllocator(){
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

      if(n!=size_){

          qDebug()<<"не тот размер!";
          return nullptr;
      }

      if(first_free){
           qDebug()<<"выделен первый слот";
          first_free=false;
           return reinterpret_cast<T*> (pool_first_);
      }

      if(second_free){
           qDebug()<<"выделен второй слот";
          second_free=false;
          return reinterpret_cast<T*> (pool_second_);
      }
       qDebug()<<"оба слота заняты!";
       return nullptr;
      /*
    size_t bytes = n * sizeof(T);
    auto memory_to_return = pool_;
    pool_ += bytes;

    if(pool_begin_+pool_size_-pool_<bytes){

        qDebug()<<"места больше нет";
        return nullptr;
    }

    qDebug()<<"pool_ "<<pool_<<" from "<<(pool_begin_+pool_size_)<<" осталось "<<(pool_begin_+pool_size_-pool_);

    return reinterpret_cast<T*> (memory_to_return);
    */
}

void deallocate(T* ptr, size_t n){
      qDebug()<<"deallocate()\n";

      if(ptr = reinterpret_cast<T*> (pool_first_)){
          qDebug()<<"освобожден первый слот";
          first_free=true;
          return;
      }
      if(ptr = reinterpret_cast<T*> (pool_second_)){

          qDebug()<<"освобожден второй слот";
          second_free=true;
          return;
      }

    return;
//    ::operator delete(ptr);
}
private:
const size_t pool_size_ = 100000;
uint8_t* pool_ = nullptr;
uint8_t* pool_begin_ = nullptr;
uint8_t* pool_first_ = nullptr;
uint8_t* pool_second_ = nullptr;
uint8_t size_ = 0;
bool first_free;
bool second_free;
};

#endif // TWOSLOTALLOCATOR_H
