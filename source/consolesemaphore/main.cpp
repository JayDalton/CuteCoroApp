#include <iostream>
#include <list>
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <shared_mutex>
#include <thread>
#include <random>
#include <format>

std::optional<int> readNumberInput()
{
   while (true)
   {
      try
      {
         std::string input;
         std::cout << "input: ";
         getline(std::cin, input);
         return std::stoi(input);
      }
      catch (...) { }
   }
}

struct QueueData final
{
   signed m_value{ 0 };
   std::chrono::milliseconds m_delay;

   std::string toString() const
   {
      return std::format("ID {:5} in {:>8}", m_value, m_delay);
   }
};

template<typename Data>
class Queue
{
   std::list<Data> m_content;
   mutable std::mutex m_mutex;
   const std::size_t m_size{ 4 };
   std::condition_variable_any m_signal;

public:
   Queue() = default;
   Queue(const Queue<Data>&) = delete;
   Queue& operator=(const Queue<Data>&) = delete;

   void push(Data&& item)
   {
      std::lock_guard lock(m_mutex);
      while (m_size <= m_content.size())
      {
         m_content.pop_front();
      }
      m_content.push_back(item);
      m_signal.notify_one();
   }

   std::vector<Data> values() const
   {
      std::scoped_lock lock(m_mutex);
      return { m_content.cbegin(), m_content.cend() };
   }

   std::optional<Data> take(std::stop_token stop)
   {
      std::unique_lock lock(m_mutex);
      if (!m_signal.wait(lock, stop, [&] { return !m_content.empty(); }))
      {
         std::cout << "queue waiting cancled\n";
         return {};
      }

      const Data data{ m_content.front() };
      m_content.pop_front();
      return data;
   }
}; 

auto main(int argc, char* argv[]) -> int
{
   std::cout << "main() starts: " << std::this_thread::get_id() << std::endl;

   Queue<QueueData> m_export;
   Queue<QueueData> m_import;

   std::jthread consumer([&](std::stop_token stop) {

      std::random_device rd;
      std::mt19937 eng(rd());
      std::uniform_int_distribution<> dist(200, 500);

      std::cout << "consumer starts: " << std::this_thread::get_id() << std::endl;

      while (auto data = m_export.take(stop))
      {
         std::cout << std::format("consumer input: {}\n", data->m_value);

         // between min - max ms per our distribution
         std::chrono::milliseconds duration(dist(eng));
         std::this_thread::sleep_for(duration);

         auto value{ data.value().m_value };
         m_import.push({ value * 2, duration });

         std::cout << std::format("consumer result: {} took {}\n", value * 2, duration);
      }

      std::cout << std::format("consumer stops.\n");
   });

   while (true)
   {
      if (auto number = readNumberInput())
      {
         if (number.value() == 7)
         {
            consumer.request_stop();
            std::cout << "program exits.\n";
            return 0;
         }

         std::cout << std::format("main intput: {}\n", number.value());
         m_export.push(QueueData{ number.value() });
      }
   }

   std::cout << "main() ends: " << std::endl;

   return {};
}
