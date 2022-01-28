#include "common.h"

void test(int x)
{
   std::cout << std::format("Hello from thread!\n");
   std::cout << std::format("Param is x = {}\n", x);
}

auto main(int argc, char* argv[]) -> int
{
   std::cout << std::format("Hello from main thread! 1\n");

   std::jthread t(&test, 123);
   t.join();

   std::cout << std::format("Hello from main thread! 2\n");
   return 0;
}

