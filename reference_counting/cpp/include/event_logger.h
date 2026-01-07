#pragma once
#include <string>
#include <fstream>

class EventLogger
{
public:
    explicit EventLogger(const std::string &filename);
    ~EventLogger();

    void log_allocate(int obj_id);
    void log_add_ref(int from, int to, int new_ref_count);
    void log_remove_ref(int from, int to, int new_ref_count);
    void log_delete(int obj_id);
    void log_leak(int obj_id);

private:
    std::ofstream file;
    void write(const std::string &json);
};
