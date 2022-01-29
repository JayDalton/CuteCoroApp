#include "common.h"

using namespace std::chrono_literals;

static std::atomic<unsigned> shared_value{ 0 };

//std::counting_semaphore<N> number_of_queuing_portions{ 0 };
//std::counting_semaphore<N> number_of_empty_positions{ N };
//std::mutex buffer_manipulation;
//
//void producer() {
//   for (;;) {
//      Portion portion = produce_next_portion();
//      number_of_empty_positions.acquire();
//      {
//         std::lock_guard<std::mutex> g(buffer_manipulation);
//         add_portion_to_buffer(portion);
//      }
//      number_of_queuing_portions.release();
//   }
//}
//
//void consumer() {
//   for (;;) {
//      number_of_queuing_portions.acquire();
//      Portion portion;
//      {
//         std::lock_guard<std::mutex> g(buffer_manipulation);
//         portion = take_portion_from_buffer();
//      }
//      number_of_empty_positions.release();
//      process_portion_taken(portion);
//   }
//}

auto main(int argc, char* argv[]) -> int
{
   std::mutex global_mutex;
   std::condition_variable_any cond_variable;

   std::cout << "main() starts: " << std::this_thread::get_id() << std::endl;

   //int result{ 0 };
   //bool notified{ false };

   //std::jthread reporter([&] 
   //{
   //   std::unique_lock lock(global_mutex);
   //   if (!notified)
   //   {
   //      cond_variable.wait(lock);
   //   }
   //   std::cout << std::format("\tReporter Result: {}\n", result);
   //});

   //std::jthread worker([&] 
   //{
   //   std::unique_lock lock(global_mutex);

   //   result = 42;
   //   notified = true;

   //   std::this_thread::sleep_for(2s);

   //   std::cout << std::format("\tworker complete\n");

   //   cond_variable.notify_one();
   //});

   //reporter.join();
   //worker.join();

   std::list<int> result{};
   bool notified{ false };

   std::jthread consumer([&](std::stop_token token)
   {
      while (!token.stop_requested())
      {
         std::unique_lock lock(global_mutex);
         while (result.empty())
         {
            std::cout << std::format("\tConsume waiting\n");
            cond_variable.wait(lock);
         }

         std::cout << std::format("\tConsume Input\n");
         while (!result.empty())
         {
            std::cout << std::format("\t\tInput: {}\n", result.front());
            result.pop_front();
         }      
         std::cout << std::format("\tConsume complete\n");
      }
   });

   std::jthread producer([&] 
   {
      std::vector<int> data = {1, 2, 3, 4};

      for (auto&& value : data)
      {
         std::cout << std::format("\tProducer sleeps\n");
         //std::this_thread::sleep_for(2s);

         std::unique_lock lock(global_mutex);
         result.push_back(value);
         lock.unlock();

         std::cout << std::format("\tproduce {}\n", value);

         cond_variable.notify_one();
      }

      std::this_thread::sleep_for(2s);

      std::cout << std::format("\tProducer stops\n");
      consumer.request_stop();
   });

   producer.join();
   //consumer.join();

   std::cout << "main() ends: " << std::endl;

   return 0;
}

