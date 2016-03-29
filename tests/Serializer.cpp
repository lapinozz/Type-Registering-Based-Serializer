#include "../Serializer.h"
#include "catch.hpp"

#include <vector>

struct Test3
{
    std::string v = "ba";
    std::string f = "ab";
};

struct Test2
{
  std::string e = "hej";
  int k = 9;
  Test3 l;
};

struct Test
{
  int a = 1;
  int b = 2;
  int c = 3;

  Test2 d;
  Test2 e;
  Test3 f;
};

template<typename T>
std::string intToString(const T& i)
{
    return std::to_string(i);
}

int stringToInt(const std::string& s)
{
    if(s.empty())
        return 0;

    return std::stoi(s);
}

int intSaver(const int& serialized, const int& toSerialize)
{
    return serialized + toSerialize;
}

std::string stringSaver(const std::string& serialized, const std::string& toSerialize)
{
//    std::cout << "Saving: " << (serialized + toSerialize).insert(serialized.size()-1, 1, char(0)) << std::endl;
//    std::cout << "Szie: " << serialized.size() + toSerialize.size() << ", " << (serialized + toSerialize).insert(serialized.size()-1, 1, char(0)).size() << std::endl;
    if(serialized.size())
        return (serialized + toSerialize).insert(serialized.size(), 1, char(0));

    return toSerialize;
}

std::string stringLoader(std::string& serialized)
{
    int x = serialized.size() - 1;
    for(;x > 0; x--)
    {
        if(serialized[x] == char(0))
            break;
    }

    std::cout << serialized.size() << " " << serialized << std::endl;
    std::string s = x ? serialized.substr(x+1) : serialized;
    std::cout << "x: " << x << std::endl;
    if(x)
        serialized.resize(x);
    std::cout << "loading: " << s << std::endl;

    return s;
}

SCENARIO("Serializer register types", "[Serializer]")
{
    Serializer s;

    WHEN("The type is not registered yet")
    {
        THEN("The serializer should not have it registered")
        {
            REQUIRE(!s.hasType<Test>());
        }
    }

    WHEN("A type is registered")
    {
        s.registerType<Test>(&Test::a, &Test::b, &Test::c, &Test::b, &Test::d);

        THEN("The serializer should have it registered")
        {
            REQUIRE(s.hasType<Test>());
        }
    }
}

SCENARIO("Serializer register types, nooooob", "[Serializer]")
{

    Serializer s;
    s.registerType<Test>(&Test::a, &Test::b, &Test::c, &Test::b, &Test::d, &Test::e, &Test::f);
//    s.registerType<Test>(&Test::a, &Test::b, &Test::c, &Test::b, &Test::d, &Test::e);
    s.registerType<Test2>(&Test2::e, &Test2::k, &Test2::l);
//    s.registerType<Test2>(&Test2::e, &Test2::k);
    s.registerType<Test3>(&Test3::f, &Test3::v);

//    s.registerType<std::vector>(&std::vector::size(), &std::vector::resize());

    s.addConversion<int, std::string>(intToString<int>);
    s.addConversion<std::string, int>(stringToInt);
    s.addConversion<double, int>();
    s.addConversion<int, double>();
    s.addSaver<int>(intSaver);
    s.addSaver<std::string>(stringSaver);
    s.addLoader<std::string>(stringLoader);

    Test t;
    t.a = 10;
    t.b = 20;
    t.c = 30;
    t.d.e = "jeh";

    auto i = s.save<int>(std::string("9"));
    auto string = s.save<std::string>(9);
    auto string2 = s.save<std::string>(t);


    for(auto& c : string2)
    {
        if(c)
            std::cout << c;
        else
            std::cout << "\n";
    }


    auto test = s.load<Test>(string2);
    std::cout << i << " " << string << std::endl;
//    std::cout << string2 << std::endl;
    std::cout << test.a << std::endl;
    std::cout << test.b << std::endl;
    std::cout << test.c << std::endl;
    std::cout << test.d.e << std::endl;
//
//    std::vector<DataPtrWrapper::unique_ptr> wrappers;
//    wrappers.emplace_back(DataPtrWrapper::create(&Test::a));
//    wrappers.emplace_back(DataPtrWrapper::create(&Test::b));
//    wrappers.emplace_back(DataPtrWrapper::create(&Test::c));
//
//    for(auto& w : wrappers)
//        std::cout << *(int*)w->get(&test) << std::endl;
}
