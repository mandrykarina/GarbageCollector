#include "event_logger.h"

EventLogger::EventLogger(const std::string &filename)
{
    file.open(filename);
}

EventLogger::~EventLogger()
{
    if (file.is_open())
        file.close();
}

void EventLogger::write(const std::string &json)
{
    file << json << std::endl;
}

void EventLogger::log_allocate(int obj_id)
{
    write("{\"event\":\"allocate\",\"object\":" + std::to_string(obj_id) + "}");
}

void EventLogger::log_add_ref(int from, int to, int new_ref_count)
{
    write("{\"event\":\"add_ref\",\"from\":" + std::to_string(from) +
          ",\"to\":" + std::to_string(to) +
          ",\"ref_count\":" + std::to_string(new_ref_count) + "}");
}

void EventLogger::log_remove_ref(int from, int to, int new_ref_count)
{
    write("{\"event\":\"remove_ref\",\"from\":" + std::to_string(from) +
          ",\"to\":" + std::to_string(to) +
          ",\"ref_count\":" + std::to_string(new_ref_count) + "}");
}

void EventLogger::log_delete(int obj_id)
{
    write("{\"event\":\"delete\",\"object\":" + std::to_string(obj_id) + "}");
}

void EventLogger::log_leak(int obj_id)
{
    write("{\"event\":\"leak\",\"object\":" + std::to_string(obj_id) + "}");
}
