#pragma once

#include <memory>

//keep in mind that void* will most of the time blow up
class DataPtrWrapper
{
    public:
    virtual const void* get(const void* classPtr) const = 0;

    virtual std::shared_ptr<const void> get(std::shared_ptr<const void> classPtr) const = 0;

    virtual void set(const void* classPtr, const void* data) = 0;
    virtual void set(std::shared_ptr<const void> classPtr, std::shared_ptr<const void> data) = 0;


    template <typename Class, typename Type>
    static std::unique_ptr<DataPtrWrapper> create(Type Class::* ptr)
    {
        return std::make_unique<DataPtrWrapper_impl<Class, Type>>(ptr);
    }

    typedef std::unique_ptr<DataPtrWrapper> unique_ptr;

    private:
    template <typename Class, typename Type>
    class DataPtrWrapper_impl;
};

template <typename Class, typename Type>
class DataPtrWrapper::DataPtrWrapper_impl : public DataPtrWrapper
{
    public:
    DataPtrWrapper_impl(Type Class::* ptr) : mPtr(ptr){}

    const void* get(const void* classPtr) const override
    {
        Type& t = ((Class*)classPtr)->*mPtr;
        return &t;
    }

    std::shared_ptr<const void> get(std::shared_ptr<const void> classPtr) const override
    {
        return std::shared_ptr<const void>(get(classPtr.get()), [](auto*){});
    }

    void set(const void* classPtr, const void* data) override
    {
        Type& t = ((Class*)classPtr)->*mPtr;
        t = *(Type*)data;
    }

    void set(std::shared_ptr<const void> classPtr, std::shared_ptr<const void> data) override
    {
        set(classPtr.get(), data.get());
    }

    private:
    Type Class::* mPtr;
};
