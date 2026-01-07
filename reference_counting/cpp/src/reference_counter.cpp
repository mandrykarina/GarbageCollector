#include "reference_counter.h"

ReferenceCounter::ReferenceCounter(std::unordered_map<int, RCObject> &heap_,
                                   EventLogger &logger_)
    : heap(heap_), logger(logger_) {}

void ReferenceCounter::add_ref(int from, int to)
{
    if (!heap.count(from) || !heap.count(to))
        return;

    heap[from].references.push_back(to);
    heap[to].ref_count++;
    logger.log_add_ref(from, to, heap[to].ref_count);
}

void ReferenceCounter::remove_ref(int from, int to)
{
    if (!heap.count(from) || !heap.count(to))
        return;

    auto &refs = heap[from].references;
    auto it = std::remove(refs.begin(), refs.end(), to);
    if (it != refs.end())
        refs.erase(it, refs.end());

    heap[to].ref_count--;
    logger.log_remove_ref(from, to, heap[to].ref_count);

    if (heap[to].ref_count == 0)
    {
        decrement_and_collect(to);
    }
}

void ReferenceCounter::decrement_and_collect(int obj_id)
{
    auto it = heap.find(obj_id);
    if (it == heap.end())
        return;

    RCObject obj = std::move(it->second); // ссылка на объект не нужна
    heap.erase(it);
    logger.log_delete(obj_id);

    for (int child : obj.references)
    {
        if (heap.count(child))
        {
            heap[child].ref_count--;
            if (heap[child].ref_count == 0)
            {
                decrement_and_collect(child);
            }
        }
    }
}
