# Type-Registering-Based-Serializer

This is a project I wanted to do to see if some ideas I had would work in practice. For a while I was thinking of how I could bind C++ class into Roboto (My own scripting language) and the best way I thought of doing so was by "registering" the class and its members, I think most scripting library that offer this feature do it like so. So I built this protoptype to see if the implementation I was thinking about would work. This serializer has multiple flaws and is not completly finished, some part of the code is quite a mess and there's still multiple compile warning. Nontheless I was able to see how to ameliorate the design and I'm quite confident that when it will be time to add thoses feature to Roboto it should be much more simple. It was also an occasion get used to Unit Tests.

# How it work

Let's say you have some struct like this:

```cpp
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
```

Then you can register them very easily like so:

```cpp
Serializer s;
s.registerType<Test>(&Test::a, &Test::b, &Test::c, &Test::b, &Test::d, &Test::e, &Test::f);
s.registerType<Test2>(&Test2::e, &Test2::k, &Test2::l);
s.registerType<Test3>(&Test3::f, &Test3::v);
```

Once the type has been registered it is very easy to serialize the struct 

```cpp
//The struct to serialize
Test t;
t.a = 10;
t.b = 20;
t.c = 30;
t.d.e = "jeh";

//serialize our struct to string
std::string serializedString = s.save<std::string>(t);

...

//And to load it back
t = s.load<Test>(serializedString);
```

And of course you can provide your own conversion function

```cpp
std::string stringSaver(const std::string& serialized, const std::string& toSerialize)
{
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

    serialized.resize(x);

    return s;
}

...


s.addConversion<int, std::string>([](const auto& i){return std::to_string(i)});
s.addConversion<std::string, int>([](const auto& s){return std::stoi(s)});
s.addConversion<int, double>();//will make automaticly make a function that use the default cast
s.addSaver<std::string>(stringSaver);
s.addLoader<std::string>(stringLoader);
```

