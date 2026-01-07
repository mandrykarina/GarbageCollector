#include "rc_heap.h"
#include "event_logger.h"
#include <iostream>

void run_basic_test(RCHeap &heap)
{
    std::cout << ">>> Basic test: allocate and simple add/remove refs\n";
    heap.allocate(1);
    heap.allocate(2);
    heap.add_ref(1, 2);
    heap.remove_ref(1, 2);
    heap.dump_state();
}

void run_cascade_test(RCHeap &heap)
{
    std::cout << ">>> Cascade deletion test\n";
    heap.allocate(1);
    heap.allocate(2);
    heap.allocate(3);

    heap.add_ref(1, 2);
    heap.add_ref(2, 3);
    heap.remove_ref(1, 2); // должно удалить 2 и 3
    heap.dump_state();
}

void run_cycle_test(RCHeap &heap)
{
    std::cout << ">>> Cycle (RC leak) test\n";
    heap.allocate(1);
    heap.allocate(2);

    heap.add_ref(1, 2);
    heap.add_ref(2, 1);    // цикл
    heap.remove_ref(1, 2); // объекты не удалятся
    heap.dump_state();
}

int main()
{
    EventLogger logger("../logs/rc_events.log");
    RCHeap heap(logger);

    run_basic_test(heap);
    run_cascade_test(heap);
    run_cycle_test(heap);

    std::cout << ">>> Testing complete. Check logs/rc_events.log for event history.\n";
    return 0;
}
