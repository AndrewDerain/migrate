#include <iostream>

#include "mango_headers.h"

#include "foundation/fixed_string_test.h"
#include "foundation/property_test.h"
 


using namespace unit_test::property_test;
 
int main()
{
	
	property_test test;
	test.run();


	/*IAnimal* animal = new Dog();
	animal->Name = "Cendy";*/


	std::cin.get();
	return 0;
}
