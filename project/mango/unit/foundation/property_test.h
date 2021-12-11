#pragma once

#include "../mango_headers.h"
#include <iostream>


using namespace mango::foundation;


namespace unit_test
{
    namespace property_test
    {
        struct TestStruct
        {
            int name;
            int age;

            TestStruct()
            {
                // std::cout << __FUNCTION__ << std::endl;
            }
        };


        class property_test
        {
        public:
            double counter;

            property<double> Counter
            {
                getter(double) { return counter; },
                setter(double) { counter = value; }
            };

           /* property<TestStruct> MyTestStruct
            {
                getter(TestStruct) { return test_struct; },
                setter(TestStruct) { test_struct = value; }
            };*/


            // 两种声明方式都可以
            /*
        private:
            double counter2;

            TestStruct test_struct;
            TestStruct test_struct2;
        public:
            readonly_property<double> Counter2{ counter2 };
            readonly_property<double> Counter2_
            {
                getter(double) { return counter2; },
            };

            readonly_property<TestStruct> MyTestStruct2
            {
                getter(TestStruct) { return test_struct2; }
            };*/

        public:
            void run() {
                std::cout << "test run start..." << std::endl;

                counter = 1;
                std::cout << "set counter = 1;            counter is " << counter << std::endl;
                
                Counter = 5;
                std::cout << "set property Counter = 5;   Counter is " << Counter << ";    " << "counter is " << counter << std::endl;

                counter = 90;
                std::cout << "set counter = 1;            Counter is " << Counter << ";    " << "counter is " << counter << std::endl;

            }
        private:
            double counter2;

            TestStruct test_struct;
            TestStruct test_struct2;

            void set_counter(const double& value) { counter = value; }
            const double& get_counter() { return counter; }
        };


        class IAnimal {
        public:
            // 此处只有对 Name 属性的声明，没有实现
            property<std::string> Name;
        };

        class Dog : public IAnimal {
        public:
            Dog() {
                // Name 属性的读写实现
                Name = std::move(property<std::string> {
                    getter(std::string) { return name; },
                    setter(std::string) { set_name(value); }
                });
            }

        private:
            std::string name;

            void set_name(std::string _name) {
                // Notify
                name = _name;
                std::cout << "set dog name: "  << _name << std::endl;
            }
        };
    }
}
