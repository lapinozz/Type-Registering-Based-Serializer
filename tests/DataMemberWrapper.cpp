#include "../DataMemberWrapper.hpp"
#include "catch.hpp"

SCENARIO("DataPtrWrapper can get and set member of class", "[DataPtrWrapper]")
{
    struct Test2
    {
        int i = 0;
    };

    struct Test
    {
        int i = 0;
        std::string s = "";
        Test2 t;
    };

    GIVEN("A DataPtrWrapper")
    {
        DataPtrWrapper::unique_ptr wrapper = DataPtrWrapper::create(&Test::i);

        WHEN("Using set function")
        {
            Test t;
            int i = 10;
            wrapper->set(&t, &i);

            THEN("The class member is set to the value")
            {
                REQUIRE(t.i == 10);
            }

            auto t_shared = std::make_shared<Test>();
            auto i_shared = std::make_shared<int>(10);
            wrapper->set(t_shared, i_shared);

            THEN("The class member is set to the value")
            {
                REQUIRE(t_shared->i == 10);
            }
        }

        WHEN("Using get function")
        {
            Test t;
            t.i = 10;
            auto value = wrapper->get(&t);

            THEN("The returned pointer point to the member")
            {
                REQUIRE(*(int*)value == 10);
                REQUIRE((int*)value == &t.i);
            }

            auto t_shared = std::make_shared<Test>();
            t_shared->i = 10;
            std::shared_ptr<const void> value_shared = wrapper->get(t_shared);

            THEN("The returned pointer point to the member")
            {
                REQUIRE(*(int*)value_shared.get() == 10);
                REQUIRE((int*)value_shared.get() == &(*t_shared).i);
            }
        }
    }
}
