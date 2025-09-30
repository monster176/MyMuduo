#pragma once

#include <functional>
#include <memory>

class Buffer;
class TcpConnection;
class TimeStamp;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteComplateCallback = std::function<void(TcpConnectionPtr&)>;

using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer *,TimeStamp)>;

using HightWateMarkCallback = std::function<void(const TcpConnectionPtr&,size_t)>;