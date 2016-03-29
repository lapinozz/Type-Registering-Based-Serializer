#pragma once

#include <functional>
#include <memory>


//this implementation is NOT safe
class FunctionWrapper
{
    public:
    template <typename ReturnType, typename...Args>
    FunctionWrapper(const std::function<ReturnType(Args...)>& func)
    {
        mFunction = std::make_shared<std::function<ReturnType(Args...)>>(func);
    }

    std::shared_ptr<void> get()
    {
        return mFunction;
    }

    template <typename ReturnType, typename...Args>
    std::shared_ptr<std::function<ReturnType(Args...)>> get()
    {
        return std::static_pointer_cast<std::function<ReturnType(Args...)>>(mFunction);
    }

    template <typename ReturnType, typename...Args>
    ReturnType invoke(Args...args)
    {
        return (*get<ReturnType, Args...>)(args...);
    }

    std::shared_ptr<void> mFunction;
};

//this implementation is NOT safe
class OneArgFunctionWrapper
{
    public:

    template <typename ReturnType, typename Arg, typename ArgType = typename std::remove_reference<Arg>::type>
    OneArgFunctionWrapper(const std::function<ReturnType(Arg)>& func) : mFunction((std::function<ReturnType(const void*)>)[func](const void* arg){return func(*((ArgType*)arg));})
    {
    }

    std::shared_ptr<void> get()
    {
        return mFunction.get();
    }

    template <typename ReturnType>
    std::shared_ptr<std::function<ReturnType(const void*)>> get()
    {
        return mFunction.get<ReturnType, const void*>();
    }

    template <typename ReturnType>
    ReturnType invoke(const void* ptr)
    {
        return (*get<ReturnType>())(ptr);
    }

    FunctionWrapper mFunction;
};

//this implementation is NOT safe
class VoidPtrReturnFunctionWrapper
{
    public:

    template <typename ReturnType, typename Arg, typename ArgType = typename std::remove_reference<Arg>::type>
    VoidPtrReturnFunctionWrapper(const std::function<ReturnType(Arg)>& func) : mFunction((std::function<std::shared_ptr<void>(const void*)>)[func](const void* arg){return std::shared_ptr<void>(new ReturnType(func(*((ArgType*)arg))));})
    {
    }

    std::shared_ptr<std::function<std::shared_ptr<void>(const void*)>> get()
    {
        return mFunction.get<std::shared_ptr<void>, const void*>();
    }

    std::shared_ptr<void> invoke(const void* ptr)
    {
        return (*get())(ptr);
    }

    std::shared_ptr<void> invoke(std::shared_ptr<const void> ptr)
    {
        return invoke(ptr.get());
    }

    template <typename ReturnType>
    ReturnType invoke(const void* ptr)
    {
        return *std::static_pointer_cast<ReturnType>(invoke(ptr));
    }

    template <typename ReturnType>
    ReturnType invoke(std::shared_ptr<const void> ptr)
    {
        return *std::static_pointer_cast<ReturnType>(invoke(ptr));
    }

    FunctionWrapper mFunction;
};
