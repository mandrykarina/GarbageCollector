#pragma once
#include <unordered_map>
#include <algorithm>
#include "rc_object.h"
#include "event_logger.h"

class ReferenceCounter
{
public:
    ReferenceCounter(std::unordered_map<int, RCObject> &heap,
                     EventLogger &logger);

    void add_ref(int from, int to);
    void remove_ref(int from, int to);

private:
    std::unordered_map<int, RCObject> &heap;
    EventLogger &logger;

    void decrement_and_collect(int obj_id);
};
