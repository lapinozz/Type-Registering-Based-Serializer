#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <algorithm>

#include "DataMemberWrapper.hpp"
#include "FunctionWrapper.hpp"

#include <iostream>

struct RegisteredType
{
    std::type_index type;
};

typedef std::shared_ptr<void> SharedVoidPtr;
typedef std::shared_ptr<const void> SharedConstVoidPtr;

//class RegisteredClass
//{
//
//};

class RegisteredClass
{
    public:
    template <typename...Args>
    RegisteredClass(Args... args)
    {
        registerType(args...);
    }

    const std::vector<std::pair<std::type_index, DataPtrWrapper::unique_ptr>>& registereds()
    {
        return mRegisteredPtr;
    }

    SharedVoidPtr construct()
    {
        return mConstruct();
    }

    private:
    template <typename T, typename ClassType, typename...Args>
    void registerType(T ClassType::*ptr, Args... args)
    {
        std::cout << typeid(T).name() << std::endl;
        mRegisteredPtr.push_back({typeid(T), DataPtrWrapper::create(ptr)});
        registerType(args...);
        mSize = sizeof(ClassType);
        mConstruct = [](){return std::shared_ptr<ClassType>(new ClassType());};
    }

    template <typename T, typename ClassType, typename...Args>
    void registerType(T (ClassType::* saveFunc)(), void (ClassType::* loadFunc)(T), Args... args)
    {
//        std::cout << typeid(T).name() << std::endl;
//        mRegisteredPtr.push_back({typeid(T), DataPtrWrapper::create(ptr)});
        registerType(args...);
//        mSize = sizeof(ClassType);
//        mConstruct = [](){return std::shared_ptr<ClassType>(new ClassType());};
    }

    void registerType(){/*should check if mSize != -1*/};

    size_t mSize = -1;
    std::function<SharedVoidPtr()> mConstruct;
    std::vector<std::pair<std::type_index, DataPtrWrapper::unique_ptr>> mRegisteredPtr;
};

class Registered
{
    public:
    int i;
};

class Serializer
{
    public:
        template <typename T, typename...Args>
        void registerType(Args...args)
        {
            mRegisteredClass.emplace(typeid(T), RegisteredClass{args...});
        }

        template <typename T>
        bool hasType()
        {
            return hasType(typeid(T));
        }

        template <typename From, typename To>
        void addConversion(std::function<To(const From&)> converter)
        {
            mConverter[typeid(From)].insert({typeid(To), {converter}});
        }

        template <typename From, typename To>
        void addConversion()
        {
            addConversion<From, To>([](const From& from) -> To {return from;});
        }

        template <typename From, typename To>
        bool hasConversion()
        {
            return hasConversion(typeid(From), typeid(From));
        }

        template <typename To>
        void addLoader(std::function<To(To&)> loader)
        {
            mLoaders.insert({typeid(To), {loader}});
        }

        template <typename To>
        void addSaver(std::function<To(const To&, const To&)> saver)
        {
            mSavers.insert({typeid(To), {saver}});
        }

        template <typename To, typename From>
        To save(const From& from)
        {
            return save<To>(std::shared_ptr<const From>(&from, [](auto*){}), typeid(From));
        }

        template <typename To, typename From>
        To load(const From& from)
        {
            return load<To>(std::shared_ptr<const From>(&from, [](auto*){}), typeid(From));
        }

    private:
        SharedConstVoidPtr convert(SharedConstVoidPtr from, std::type_index fromType, std::type_index toType)
        {
            auto fromIter = mConverter.find(fromType);
            if(fromIter != mConverter.end())
            {
                auto toIter = fromIter->second.find(toType);
                if(toIter != fromIter->second.end())
                {
                    return toIter->second.invoke(from);
                }
            }

            if(fromType == toType)
                return from;

            return {};
        }

        template <typename To>
        To convert(SharedConstVoidPtr from, std::type_index fromType, std::type_index toType)
        {
            auto fromIter = mConverter.find(fromType);
            if(fromIter != mConverter.end())
            {
                auto toIter = fromIter->second.find(toType);
                if(toIter != fromIter->second.end())
                {
                    return toIter->second.invoke<To>(from);
                }
            }

            if(fromType == toType)
                return *std::static_pointer_cast<const To>(from);

            return {};
        }


        bool hasConversion(std::type_index fromType, std::type_index toType)
        {
            auto fromIter = mConverter.find(fromType);
            if(fromIter != mConverter.end())
            {
                auto toIter = fromIter->second.find(toType);
                if(toIter != fromIter->second.end())
                {
                    return true;
                }
            }

            return fromType == toType;
        }

        bool hasType(std::type_index classType)
        {
            auto it = std::find_if(mRegisteredClass.begin(), mRegisteredClass.end(), [&](const auto& pair){return pair.first == classType;});

            return it != mRegisteredClass.end();
        }

        template <typename To>
        To save(SharedConstVoidPtr from, std::type_index fromType, To to = To())
        {
            if(hasConversion(fromType, typeid(To)))
            {
                auto saverIt = mSavers.find(typeid(To));
                if(saverIt == mSavers.end()) std::cout << "oj!" << std::endl;//ERROR
                auto saver = *saverIt->second.get<To, const To&, const To&>();

                return saver(to, convert<To>(from, fromType, typeid(To)));
            }

            auto it = std::find_if(mRegisteredClass.begin(), mRegisteredClass.end(), [&](const auto& pair){return pair.first == fromType;});
            if(it != mRegisteredClass.end())
            {
                for(auto& registered : it->second.registereds())
                {
                    to = save<To>(registered.second->get(from), registered.first, to);
                }

                return to;
            }

           //ERROR
        }
//
        template <typename To>
        To load(SharedConstVoidPtr from, std::type_index fromType)
        {
            return *std::static_pointer_cast<const To>(load(from, fromType, typeid(To)));
        }

        SharedConstVoidPtr load(SharedConstVoidPtr from, std::type_index fromType, std::type_index toType)
        {
            if(hasConversion(fromType, toType))
            {
                auto loaderIt = mLoaders.find(fromType);
                if(loaderIt == mLoaders.end()) std::cout << "oj!" << std::endl;//ERROR
                auto loader = *loaderIt->second.get();

                return convert(loader(from.get()), fromType, toType);
            }

            for(auto& pair : mRegisteredClass)
            {
                if(pair.first == toType)
                {
                    auto to = pair.second.construct();

//                    load<To>(*loader(from), registered.first);
                    auto& registereds = pair.second.registereds();
                    for(auto registered = registereds.rbegin(); registered != registereds.rend(); registered++)
                    {
                        registered->second->set(to.get(), load(from, fromType, registered->first).get());
                    }

                    return to;
                }
            }

           //ERROR
        }

        std::unordered_map<std::type_index, RegisteredClass> mRegisteredClass;
        std::unordered_map<std::type_index, std::unordered_map<std::type_index, VoidPtrReturnFunctionWrapper>> mConverter;
        //                  From Type                           //Tp Type       //converter

        std::unordered_map<std::type_index, FunctionWrapper> mSavers;
        std::unordered_map<std::type_index, VoidPtrReturnFunctionWrapper> mLoaders;

        template <typename T>
        void do_nothing_deleter(T*){};
};
