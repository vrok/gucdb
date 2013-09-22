#include <vector>
#include <iostream>
#include <atomic>
#include <thread>

namespace Db {

// MOVE THIS TO C++11 thread_local AFTER GCC & CLANG FIX THEIR BUGS
// This has to be manually initialized at the beginning of every thread using the queue.
// With thread_local, seeds will be initialized automatically.
__thread unsigned int seed;

template<typename T>
class ConcurrentQueue
{
public:
    static std::atomic<unsigned int> topId;
    //static thread_local unsigned int seed; // We will use C++11 thread_local after GCC and Clang fix their bugs

    struct Item
    {
        Item *next;
        T value;
    };


    struct FirstItem {
        FirstItem() noexcept : item(nullptr), id(rand_r(&seed)) {}
        FirstItem(Item *item) : item(item), id(rand_r(&seed)) {}
        Item *item;
        uint64_t id;
    };

    std::atomic<FirstItem> first;

    void push(T t) {
        static_assert(sizeof(Item) == 16, "Argument of ConcurrentQueue template has to be 8 bits wide");

        Item *newItem = new Item;
        newItem->value = t;

        FirstItem newFirst(newItem);
        FirstItem copyFirst;

        do {
            copyFirst = first.load(std::memory_order_acquire);
            newItem->next = copyFirst.item;
        } while (!first.compare_exchange_weak(copyFirst, newFirst));
    }

    T pop() {
        FirstItem copyFirst, newFirst;
        do {
            copyFirst = first.load(std::memory_order_acquire);
            if (copyFirst.item == nullptr) {
                return T();
            }

            newFirst.item = copyFirst.item->next;

        } while (!first.compare_exchange_weak(copyFirst, newFirst));
        //} while (false);

        T && result = std::move(copyFirst.item->value);
        delete copyFirst.item;
        return result;
    }
};

template<typename T>
std::atomic<unsigned int> ConcurrentQueue<T>::topId { 0 };

//__thread unsigned int seed = topId++;

//template<typename T>
//thread_local unsigned int ConcurrentQueue<T>::seed = topId++;

}

#if 0
int main()
{
    std::vector<std::thread> threads;

    Db::ConcurrentQueue<int> queue;


    for (int i = 0; i < 10; i++) {
        threads.push_back(std::thread([&]() {
            Db::seed = ++queue.topId;

            for (int j = 0; j < 3; j++) {
                //int pushed = (Db::ConcurrentQueue<int>::seed + 1) * j;
                unsigned int pushed = ((Db::seed + 1) * (j + 1)) % 100;
                queue.push(pushed);

                std::this_thread::sleep_for(std::chrono::milliseconds(rand_r(&Db::seed) % 100));

                unsigned int popped = queue.pop();

                std::cout << "pushed: " << pushed << ", popped: " << popped << std::endl;
                //std::cout << "seed = " << Db::ConcurrentQueue<int>::seed << ", j = " << j << std::endl;
            }
        }));
        //t.join();
        //t.detach();
    }

    for (std::thread &t : threads) {
        std::cout << "aaaa" << std::endl;
        t.join();
    }
}

#endif
