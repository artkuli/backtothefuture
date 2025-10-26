#pragma once


struct IExecutor {
    virtual ~IExecutor() = default;
    virtual void for_each(size_t count, std::function<void(std::size_t)> func) = 0;
};
