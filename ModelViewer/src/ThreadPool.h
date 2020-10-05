#pragma once
#include "pch.h"

class ThreadPool
{
public:
    ThreadPool(int threadsCount, int tasksCount = 0)
    {
        m_threads.reserve(threadsCount);
        m_tasks.reserve(tasksCount ? tasksCount : threadsCount);

        for (int i = 0; i < threadsCount; i++)
        {
            m_threads.emplace_back(&task, this);
            m_threads.back().detach();
        }
    }

    ~ThreadPool()
    {
        m_shouldWork = false;
        m_cv.notify_all();

        wait();
    }

    template<typename TCallable, typename ...TArgs>
    void enque(TCallable task, TArgs ...args)
    {
        auto&& binded = std::bind(task, std::forward<TArgs>(args)...);

        std::unique_lock l(m_mutex);
        m_tasks.emplace_back(std::move(binded));
        m_cv.notify_one();
    }

    void wait()
    {
        std::unique_lock l(m_mutex);

        while (!m_tasks.empty())
            m_otherCv.wait(l);
    }

private:
    static void task(ThreadPool* t)
    {
        while (t->m_shouldWork)
        {
            std::function<void()> tsk;
            {
                std::unique_lock l(t->m_mutex);
                while (t->m_tasks.empty() && t->m_shouldWork)
                {
                    t->m_otherCv.notify_all();
                    t->m_cv.wait(l);
                }

                if (!t->m_shouldWork)
                    return;

                tsk = std::move(t->m_tasks.back());
                t->m_tasks.pop_back();
            }


            tsk();
        }
    }

private:
    std::vector<std::function<void(void)>> m_tasks;
    std::vector<std::thread> m_threads;
    std::condition_variable m_cv;
    std::condition_variable m_otherCv;
    std::mutex m_mutex;
    volatile bool m_shouldWork = true;
};