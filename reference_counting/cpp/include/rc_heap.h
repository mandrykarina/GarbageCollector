#pragma once
#include <unordered_map>
#include <iostream>
#include "rc_object.h"
#include "reference_counter.h"
#include "event_logger.h"

class RCHeap
{
public:
    explicit RCHeap(EventLogger &logger);

    void allocate(int obj_id);
    void add_ref(int from, int to);
    void remove_ref(int from, int to);

    void dump_state() const;

private:
    std::unordered_map<int, RCObject> objects;
    ReferenceCounter rc;
    EventLogger &logger;
};
