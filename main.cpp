#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <future>

std::mutex cout_guard, mtx;
std::condition_variable condition;
bool thread1_finished = false;
bool thread2_finished = false;
bool thread3_finished = false;

void Thread_1_2_Working(const std::string& name, int& n, int* m1, int* m2)
{
	for (int i = 0; i < n; i++)
	{
		cout_guard.lock();
		if (name == "thread1")
		{
			std::cout << m1[i] << " * " << m2[i] << " = " << m1[i] * m2[i] << "\t thread1 " << std::endl;
		}
		else if (name == "thread2")
		{
			std::cout << m1[i] << " + " << m2[i] << " = " << m1[i] + m2[i] << "\t thread2 " << std::endl;

		}
		cout_guard.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}

	if (name == "thread1")
	{
		thread1_finished = true;
		condition.notify_one();
	}
	else if (name == "thread2")
	{
		thread2_finished = true;
		condition.notify_one();
	}
}


void Thread_3_Working(int& n)
{
	srand(time(NULL));
	for (size_t i = 0; i < n; i++)
	{
		cout_guard.lock();
		std::cout << 1 + rand() % 1000 << "\t thread3" << std::endl;
		cout_guard.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
	thread3_finished = true;
	condition.notify_one();
}

int main()
{

	srand(time(NULL));

	int First_Array[20], Second_Array[20];

	for (int i = 0; i < 20; i++)
	{
		First_Array[i] = 1 + rand() % 100;
		Second_Array[i] = 1 + rand() % 100;
	}

	int n = 20;

	std::thread thread1(Thread_1_2_Working, "thread1", std::ref(n), First_Array, Second_Array);

	std::thread thread2(Thread_1_2_Working, "thread2", std::ref(n), First_Array, Second_Array);

	std::thread thread3(Thread_3_Working, std::ref(n));

	{
		std::unique_lock<std::mutex> mainlock(mtx);
		condition.wait(mainlock, []
			{
				return thread1_finished && thread2_finished && thread3_finished;
			});
		std::cout << "All threads finished working ====================" << '\n';
	}

	thread3.join();
	thread1.join();
	thread2.join();

	return 0;
}