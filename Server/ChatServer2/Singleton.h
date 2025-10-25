#pragma once
#include <memory>
#include <mutex>
#include <iostream>

//单例
template <typename T>
class Singleton {
protected:
    Singleton() = default;//构造函数
    Singleton(const Singleton<T>&) = delete;//拷贝构造
    Singleton& operator = (const Singleton<T>& st) = delete;//删除拷贝赋值

    //智能指针自动回收
    static std::shared_ptr<T> _instance;//使用static的原因，所有单例都返回一个实例

public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {/*回调函数*/
            _instance = std::shared_ptr<T>(new T); //初始化instance
            /*
             * ?为啥是new T 而不是 make_shared
             * make_shared需要调用构造函数，但是单例类中构造函数私有
            */
            });
        return _instance;
    };

    //打印地址
    void PrintAddress() {
        std::cout << _instance.get() << std::endl;//原始的裸指针
    }

    //析构函数
    ~Singleton() {
        std::cout << "this Singleton is destructed" << std::endl;
    }

};

//类内声明，类外实例化
//实例化
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;