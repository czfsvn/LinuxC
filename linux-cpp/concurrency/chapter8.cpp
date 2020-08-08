#include <iostream>
#include <list>
#include <vector>
#include <thread>
#include <future>
#include <set>
#include <algorithm>
#include <numeric>

using namespace std;

namespace ns_parallel_quick_sort
{
	template<typename T>
		class Sorter
		{
			struct chunck_to_sort
			{
				std::list<T>                    data;
				std::promise<std::list<T>>      promise;
			};
		};
}

using namespace std;

#define LOOP_ELEMENTS		10000000
#define FUNC_COST			TimeCost __time_cost__(__FUNCTION__)
#define TIME_COST(x)		TimeCost __time_cost__(x)

class ThreadJoin
{
public:
	ThreadJoin(std::vector<std::thread> thrds) :threads(std::move(thrds))	{}
	~ThreadJoin()
	{
		for (uint32_t index = 0; index < threads.size(); ++index)
		{
			threads[index].join();
		}
	}
private:
	std::vector<std::thread> threads;
};

namespace ns_thread
{
	void func()
	{
		std::cout << std::this_thread::get_id() << std::endl;
	}

	void create_thread()
	{
		std::thread thrd(func);
		thrd.join();
	}

	void main()
	{
		create_thread();
	}
}

class TimeCost
{
	public:
		TimeCost(const std::string& func): func_name(func) {}
		~TimeCost()
		{
			std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
			auto dura = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << func_name.c_str() << " 耗时 cost: " << dura << " ms\n";
		}
	private:
		std::chrono::system_clock::time_point start = chrono::system_clock::now();
		std::string func_name;
};


namespace ns_foreach
{
	std::set<std::thread::id> thread_set = {};
	void func_print(uint64_t)
	{
		std::this_thread::sleep_for(1ms);
		//std::cout << std::this_thread::get_id() << std::endl;
		thread_set.emplace(std::move(std::this_thread::get_id()));
	}

	template <typename Iterator, typename Func>
		void parallel_for_each(Iterator first, Iterator last, Func func)
		{
			uint64_t const length = std::distance(first, last);
			if (!length)
				return;

			uint64_t const min_per_thread = 25;
			uint64_t const max_threads = (length + min_per_thread - 1) / min_per_thread;

			uint64_t const hardware_thread = std::thread::hardware_concurrency();
			uint64_t const num_threads = std::min(hardware_thread != 0 ? hardware_thread : 2, max_threads);

			uint64_t const block_size = length / num_threads;

			std::vector<std::future<void>> futures(num_threads - 1);
			std::vector<std::thread> threads(num_threads - 1);

			Iterator block_start = first;
			for (uint64_t i = 0; i < (num_threads - 1); ++i)
			{
				Iterator block_end = block_start;
				std::advance(block_end, block_size);
				std::packaged_task<void(void)> task(
						[=]()
						{
							std::for_each(block_start, block_end, func);
						});
				futures[i] = task.get_future();
				threads[i] = std::thread(std::move(task));
				block_start = block_end;
			}

			std::for_each(block_start, last, func);
			for (uint64_t i = 0; i < (num_threads - 1); ++i)
			{
				futures[i].get();
			}
			for (uint64_t idx = 0; idx < num_threads - 1; ++idx)
			{
				threads[idx].join();
			}
		}

	template<typename Iterator, typename Func>
		void parallel_async(Iterator first, Iterator last, Func func)
		{
			uint64_t const length = std::distance(first, last);
			if(!length)
				return;

			uint64_t const min_per_thread = 25;
			if (length < (2 * min_per_thread))
			{
				std::for_each(first, last, func);
			}
			else
			{
				Iterator const mid_point = first + length / 2;
				std::future<void> first_half = std::async(std::launch::async, 
					&parallel_async<Iterator, Func>, first, mid_point, func);
				parallel_async(mid_point, last, func);
				first_half.get();
			}
		}

	void main()
	{
		std::vector<uint64_t> u_vec;
		for(uint64_t idx = 0; idx < LOOP_ELEMENTS; idx++)
		{
			u_vec.emplace_back(idx);
		}
		{
			TIME_COST("for_each");
			std::for_each(u_vec.begin(), u_vec.end(), func_print);
		}
		thread_set.clear();
		{			
			TIME_COST("parallel_for_each");
			parallel_for_each(u_vec.begin(), u_vec.end(), func_print);
		}	
		thread_set.clear();			// created 4 thread when LOOP_ELEMENTS=10000
		{
			TIME_COST("parallel_async");
			parallel_async(u_vec.begin(), u_vec.end(), func_print);
		}
		thread_set.clear();			// created 189 thread when LOOP_ELEMENTS=10000
	}
}

namespace ns_find
{
	template <typename Iterator, typename MatchType>
	Iterator parallel_find(Iterator first, Iterator last, MatchType match)
	{
		struct find_element
		{
			void operator()(Iterator begin, Iterator end, MatchType match,
				std::promise<Iterator>* result, std::atomic<bool>* done_flag)
			{
				try
				{
					for(; (begin != end) && !done_flag->load(); ++begin)
					{
						if (*begin == match)
						{
							result->set_value(begin);
							done_flag->store(true);
							return;
						}						
					}
				}
				catch(...)
				{
					try
					{
						result->set_exception(std::current_exception());
						done_flag->store(true);
					}
					catch(...)
					{
						
					}
				}
			}
		};

		uint64_t const length = std::distance(first, last);
		if (!length)
			return last;

		uint64_t const min_per_thread = 25;
		uint64_t const max_threads = (length + min_per_thread - 1) / min_per_thread;
		uint64_t const hardware_thread = 6;//std::max(std::thread::hardware_concurrency(), 4L);
		uint64_t const num_threads = std::min(hardware_thread != 0 ? hardware_thread : 2, max_threads);
		uint64_t const block_size = length / num_threads;

		std::promise<Iterator> result;
		std::atomic<bool> done_flag(false);
		std::vector<std::thread> threads(num_threads - 1);
		{
			Iterator block_start = first;
			for (uint64_t i = 0; i < (num_threads - 1); ++i)
			{
				Iterator block_end = block_start;
				std::advance(block_end, block_size);
				threads[i] = std::thread(find_element(), block_start, block_end, match, &result, &done_flag);
				block_start = block_end;
			}
			ThreadJoin t_join(std::move(threads));
			find_element()(block_start, last, match, &result, &done_flag);
			if (!done_flag.load())
			{
				return last;
			}
			return result.get_future().get();
		}
	}

	template<typename Iterator, typename MatchType>
	Iterator parallel_find_async_impl(Iterator first, Iterator last, MatchType match, std::atomic<bool>& done)
	{
		try
		{
			uint64_t const length = std::distance(first, last);
			uint64_t const min_per_thread = 25;
			if (length < (2 * min_per_thread))
			{
				for (; (first != last) && !done.load(); ++first)
				{
					if (*first == match)
					{
						done = true;
						return first;
					}
				}
				return last;
			}
			else
			{
				Iterator const mid_point = first + length / 2;
				std::future<Iterator> async_result = std::async(std::launch::deferred | std::launch::async,
					&parallel_find_async_impl<Iterator, MatchType>,	
					mid_point, last, match, std::ref(done));

				Iterator const direct_result = parallel_find_async_impl(first, mid_point, match, done);
				return (direct_result == mid_point) ? async_result.get() : direct_result;
			}
		}
		catch(...)
		{
			done = true;
			throw;
		}
	}

	template<typename Iterator, typename MatchType>
	Iterator parallel_find_async(Iterator first, Iterator last, MatchType match)
	{
		std::atomic<bool> done(false);
		return parallel_find_async_impl(first, last, match, done);
	}

	void main()
	{
		std::vector<uint64_t> u_vec;
		for(uint64_t idx = 0; idx < LOOP_ELEMENTS; idx++)
		{
			u_vec.emplace_back(idx);
		}
		if (u_vec.empty())	return;
		{
			TIME_COST("for_each");
			for(uint64_t value : u_vec)
			{
				if (value == LOOP_ELEMENTS/2)
					break;
			}
		}
		{
			TIME_COST("parallel_find");
			parallel_find(u_vec.begin(), u_vec.end(), LOOP_ELEMENTS/2);
		}
		{
			TIME_COST("parallel_find_async");
			parallel_find_async(u_vec.begin(), u_vec.end(), LOOP_ELEMENTS/2);
		}
	}
}

namespace ns_sum
{
	template<typename Iterator>
	void parallel_partial_sum(Iterator first, Iterator last)
	{
		typedef typename Iterator::value_type value_type;
		struct process_chunk
		{
			void operator() (Iterator begin, Iterator last,	std::future<value_type>* previous_end_value,
				std::promise<value_type>* end_value)
			{
				try
				{
					Iterator end = last;
					++end;
					std::partial_sum(begin, end, begin);
					if (previous_end_value)
					{
						value_type& addend = previous_end_value->get();
						*last += addend;
						if (end_value)
						{
							end_value->set_value(*last);
						}
						std::for_each(begin, last, [&addend](value_type& item)	
							{	
								item += addend;		
							});
					}
				}
				catch(...)
				{
					if (end_value)
					{
						end_value->set_exception(std::current_exception);
					}
					else
					{
						throw;
					}
				}
			}	
		};

		uint64_t const length = std::distance(first, last);
		if (!length)
			return;

		uint64_t const min_per_thread = 25;
		uint64_t const max_threads = (length + min_per_thread - 1) / min_per_thread;
		uint64_t const hardware_threads = std::thread::hardware_concurrency();
		uint64_t const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		uint64_t const block_size = length / num_threads;

		typedef typename Iterator::value_type value_type;

		std::vector<std::thread> threads(num_threads - 1);
		std::vector<std::promise<value_type>> end_value(num_threads - 1);
		std::vector<std::future<value_type>> previous_end_values;
		previous_end_values.reserve(num_threads - 1);

		Iterator block_start = first;
		for (uint64_t i = 0; i < (num_threads - 1); ++i)
		{
			Iterator block_last = block_start;
			std::advance(block_last, block_size - 1);
			threads[i] = std::thread(process_chunk(), block_start, block_last,
				(i != 0) ? &previous_end_values[i - 1] : 0, &end_value[i]);
			block_start = block_last;
			++block_start;
			previous_end_values.push_back(end_value[i].get_future());
		}
		ThreadJoin joinner(threads);
		Iterator final_element = block_start;
		std::advance(final_element, std::distance(block_start, last) - 1);
		process_chunk()(block_start, final_element,	(num_threads > 1) ? &previous_end_values.back() :0, 0);
	}
}

int main()
{
	ns_thread::main();
	//ns_foreach::main();
	//ns_find::main();
	std::cout << "Hello, VS2017" << std::endl;
	return 0;
}
