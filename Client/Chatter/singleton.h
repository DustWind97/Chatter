#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <iostream>
#include <mutex> //互斥量

/******************************************************************************
 *
 * @file       singleton.h
 * @brief      Function: 写一个单例模板，方便下次批量使用
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
template <typename T>
class Singleton{
protected:
    Singleton() = default;                                    // 构造函数
    Singleton(const Singleton<T>&) = delete;                  // 拷贝构造（删除）
    Singleton& operator = (const Singleton<T>& st) = delete;  // 拷贝运算符（删除）

    //智能指针自动回收
    static std::shared_ptr<T> _instance;//使用static的原因，所有单例都返回一个实例

public:
    //获取单例实例
    static std::shared_ptr<T> GetInstance(){
        //单次调用 once_flag 和添加 运行锁是一样的效果
        static std::once_flag s_flag; 
        std::call_once(s_flag,[&](){/*回调函数*/
            _instance = std::shared_ptr<T>(new T); //初始化instance
            /*
             * ?为啥是 new T 而不是 make_shared
             * make_shared 需要调用构造函数，但是单例类中构造函数私有
             * 如果坚持使用 make_shared 可将 Singleton 类声明为友元：
             *      friend std::shared_ptr<T> std::make_shared<T>();
            */
        });
        return _instance;
    };

    //打印地址
    void PrintAddress(){
        std::cout << _instance.get() << std::endl;//原始的裸指针
    }

    //析构函数
    ~Singleton(){
        std::cout << "This Singleton was destructed" << std::endl;
    }
};

//类内声明，类外实例化
//实例化
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
