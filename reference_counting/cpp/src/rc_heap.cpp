#include "rc_heap.h"

RCHeap::RCHeap(EventLogger &logger_)
    : rc(objects, logger_), logger(logger_) {}

void RCHeap::allocate(int obj_id)
{
    if (objects.count(obj_id))
        return;
    objects.emplace(obj_id, RCObject(obj_id));
    logger.log_allocate(obj_id);
}

void RCHeap::add_ref(int from, int to)
{
    rc.add_ref(from, to);
}

void RCHeap::remove_ref(int from, int to)
{
    rc.remove_ref(from, to);
}

void RCHeap::dump_state() const
{
    std::cout << "=== HEAP STATE ===\n";
    if (objects.empty())
        std::cout << "[empty]\n";
    for (const auto &[id, obj] : objects)
    {
        std::cout << "Object " << id
                  << " | ref_count=" << obj.ref_count
                  << " | refs: ";
        for (int r : obj.references)
            std::cout << r << " ";
        std::cout << "\n";
    }
    std::cout << "=================\n\n";
}
