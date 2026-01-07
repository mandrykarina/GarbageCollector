#include "rc_heap.h"
#include <iostream>
#include <algorithm>

RCHeap::RCHeap(EventLogger &logger_)
    : rc(objects, logger_), logger(logger_)
{
}

bool RCHeap::allocate(int obj_id)
{
    // Проверить, не существует ли уже объект с таким ID
    if (objects.count(obj_id) > 0)
    {
        std::cerr << "Error: Object " << obj_id << " already exists\n";
        return false;
    }

    // Проверить валидность ID
    if (obj_id < 0)
    {
        std::cerr << "Error: Invalid object ID " << obj_id << "\n";
        return false;
    }

    // Выделить новый объект
    objects.emplace(obj_id, RCObject(obj_id));
    logger.log_allocate(obj_id);

    return true;
}

bool RCHeap::add_ref(int from, int to)
{
    // Валидация ID'ов
    if (from < 0 || to < 0)
    {
        std::cerr << "Error: Invalid object IDs\n";
        return false;
    }

    // Проверить, существуют ли оба объекта
    if (!object_exists(from))
    {
        std::cerr << "Error: Source object " << from << " does not exist\n";
        return false;
    }
    if (!object_exists(to))
    {
        std::cerr << "Error: Target object " << to << " does not exist\n";
        return false;
    }

    // Запретить саморефренцию
    if (from == to)
    {
        std::cerr << "Error: Self-reference not allowed\n";
        return false;
    }

    // Делегировать ReferenceCounter
    return rc.add_ref(from, to);
}

bool RCHeap::remove_ref(int from, int to)
{
    // Валидация ID'ов
    if (from < 0 || to < 0)
    {
        std::cerr << "Error: Invalid object IDs\n";
        return false;
    }

    // Проверить, существует ли source объект
    if (!object_exists(from))
    {
        std::cerr << "Error: Source object " << from << " does not exist\n";
        return false;
    }

    // Целевой объект может быть удален во время операции, но проверим перед удалением
    if (!object_exists(to))
    {
        std::cerr << "Error: Target object " << to << " does not exist\n";
        return false;
    }

    // Делегировать ReferenceCounter
    return rc.remove_ref(from, to);
}

void RCHeap::dump_state() const
{
    std::cout << "=== HEAP STATE ===\n";

    if (objects.empty())
    {
        std::cout << "[empty]\n";
    }
    else
    {
        // Вывести объекты отсортированные по ID для консистентности
        std::vector<int> ids;
        for (const auto &[id, _] : objects)
        {
            ids.push_back(id);
        }
        std::sort(ids.begin(), ids.end());

        for (int id : ids)
        {
            const RCObject &obj = objects.at(id);
            std::cout << "Object " << id
                      << " | ref_count=" << obj.ref_count
                      << " | refs: ";

            // Вывести исходящие ссылки
            for (int ref : obj.references)
            {
                std::cout << ref << " ";
            }
            std::cout << "\n";
        }
    }

    std::cout << "=================\n\n";
}

void RCHeap::run_scenario(const ScenarioOp ops[], int size)
{
    for (int i = 0; i < size; ++i)
    {
        const ScenarioOp &op = ops[i];

        if (op.op == "allocate")
        {
            allocate(op.id);
        }
        else if (op.op == "add_ref")
        {
            add_ref(op.from, op.to);
        }
        else if (op.op == "remove_ref")
        {
            remove_ref(op.from, op.to);
        }
        else
        {
            std::cerr << "Unknown operation: " << op.op << "\n";
        }

        dump_state();
    }
}

int RCHeap::get_ref_count(int obj_id) const
{
    auto it = objects.find(obj_id);
    if (it != objects.end())
    {
        return it->second.ref_count;
    }
    return -1; // Объект не существует
}

void RCHeap::detect_and_log_leaks()
{
    for (const auto &[id, obj] : objects)
    {
        // Логировать объекты с ref_count > 0 (потенциальная утечка)
        if (obj.ref_count > 0)
        {
            logger.log_leak(id);
        }
    }
}

RCObject *RCHeap::get_object(int obj_id)
{
    auto it = objects.find(obj_id);
    if (it != objects.end())
    {
        return &it->second;
    }
    return nullptr;
}

const RCObject *RCHeap::get_object(int obj_id) const
{
    auto it = objects.find(obj_id);
    if (it != objects.end())
    {
        return &it->second;
    }
    return nullptr;
}
