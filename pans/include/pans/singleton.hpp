#ifndef PANS_INCLUDE_PANS_SINGLETON_HPP
#define PANS_INCLUDE_PANS_SINGLETON_HPP

namespace pans{

// 单例模式：必须保证它们在系统中存在一个实例

template <typename T>
class Singleton {
public:
    static T& Instance()
    {
        static T instance; // 局部静态变量，C++11之后线程安全
        return instance;
    }
private:
    // 单例对象不需要拷贝或移动
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
};

}

#endif
