#pragma once

/**
 * 所有类均继承自noncopyable，为了防止派生类出现拷贝构造或赋值的行为
 * 但是可以正常构造或析构
 */
class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
