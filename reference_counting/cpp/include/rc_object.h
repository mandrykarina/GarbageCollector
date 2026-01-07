#pragma once
#include <vector>

struct RCObject
{
    int id;
    int ref_count;
    std::vector<int> references;

    RCObject() : id(-1), ref_count(0) {} // конструктор по умолчанию
    RCObject(int id_) : id(id_), ref_count(0) {}
};
