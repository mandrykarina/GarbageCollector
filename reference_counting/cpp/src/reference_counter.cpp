#include "reference_counter.h"
#include <iostream>

ReferenceCounter::ReferenceCounter(std::unordered_map<int, RCObject> &heap_, EventLogger &logger_)
    : heap(heap_), logger(logger_)
{
}

bool ReferenceCounter::add_ref(int from, int to)
{
    // Валидация: оба объекта должны существовать
    if (heap.find(from) == heap.end())
    {
        std::cerr << "Error: Source object " << from << " does not exist\n";
        return false;
    }
    if (heap.find(to) == heap.end())
    {
        std::cerr << "Error: Target object " << to << " does not exist\n";
        return false;
    }

    RCObject &from_obj = heap[from];
    RCObject &to_obj = heap[to];

    // Проверить, не существует ли уже такая ссылка
    if (from_obj.has_reference_to(to))
    {
        std::cerr << "Warning: Reference from " << from << " to " << to
                  << " already exists\n";
        return false;
    }

    // Добавить исходящую ссылку от source к target
    from_obj.add_outgoing_ref(to);

    // Увеличить счётчик входящих ссылок у целевого объекта
    to_obj.ref_count++;

    // Логировать операцию
    logger.log_add_ref(from, to, to_obj.ref_count);

    return true;
}

bool ReferenceCounter::remove_ref(int from, int to)
{
    // Валидация: оба объекта должны существовать
    if (heap.find(from) == heap.end())
    {
        std::cerr << "Error: Source object " << from << " does not exist\n";
        return false;
    }
    if (heap.find(to) == heap.end())
    {
        std::cerr << "Error: Target object " << to << " does not exist\n";
        return false;
    }

    RCObject &from_obj = heap[from];
    RCObject &to_obj = heap[to];

    // Проверить, существует ли такая ссылка
    if (!from_obj.has_reference_to(to))
    {
        std::cerr << "Error: No reference from " << from << " to " << to << "\n";
        return false;
    }

    // Удалить исходящую ссылку
    from_obj.remove_outgoing_ref(to);

    // Уменьшить счётчик входящих ссылок
    to_obj.ref_count--;

    // Защита от отрицательного счётчика
    if (to_obj.ref_count < 0)
    {
        std::cerr << "Error: ref_count became negative for object " << to << "\n";
        to_obj.ref_count = 0;
        return false;
    }

    // Логировать операцию
    logger.log_remove_ref(from, to, to_obj.ref_count);

    // Если ref_count == 0, начать каскадное удаление
    if (to_obj.ref_count == 0)
    {
        std::unordered_set<int> visited;
        cascade_delete(to, visited);
    }

    return true;
}

void ReferenceCounter::cascade_delete(int obj_id, std::unordered_set<int> &visited)
{
    // Проверить, существует ли объект
    if (heap.find(obj_id) == heap.end())
    {
        return;
    }

    // Избежать циклического удаления
    if (visited.count(obj_id))
    {
        return;
    }

    RCObject &obj = heap[obj_id];

    // Удалить только если ref_count == 0
    if (obj.ref_count > 0)
    {
        return;
    }

    visited.insert(obj_id);

    // Получить копию исходящих ссылок перед удалением объекта
    std::vector<int> children = obj.references;

    // Удалить объект из памяти
    heap.erase(obj_id);
    logger.log_delete(obj_id);

    // Рекурсивно удалить дочерние объекты с ref_count == 0
    for (int child_id : children)
    {
        if (heap.find(child_id) != heap.end())
        {
            RCObject &child = heap[child_id];

            // Уменьшить счётчик, так как родитель удалён
            child.ref_count--;
            if (child.ref_count < 0)
            {
                child.ref_count = 0;
            }

            // Если счётчик стал 0, удалить дочерний объект
            if (child.ref_count == 0)
            {
                cascade_delete(child_id, visited);
            }
        }
    }
}

bool ReferenceCounter::has_cycle(int start_id) const
{
    if (heap.find(start_id) == heap.end())
    {
        return false;
    }

    std::unordered_set<int> visited;
    std::unordered_set<int> rec_stack;

    return has_cycle_dfs(start_id, visited, rec_stack);
}

bool ReferenceCounter::has_cycle_dfs(int current_id,
                                     std::unordered_set<int> &visited,
                                     std::unordered_set<int> &rec_stack) const
{
    // Пометить как посещённый
    visited.insert(current_id);
    rec_stack.insert(current_id);

    if (heap.find(current_id) == heap.end())
    {
        return false;
    }

    const RCObject &current = heap.at(current_id);

    // Проверить все исходящие ссылки
    for (int neighbor : current.references)
    {
        if (heap.find(neighbor) == heap.end())
        {
            continue;
        }

        // Если сосед не посещён, рекурсивно проверить его
        if (!visited.count(neighbor))
        {
            if (has_cycle_dfs(neighbor, visited, rec_stack))
            {
                return true;
            }
        }
        // Если сосед находится в стеке рекурсии, то найден цикл
        else if (rec_stack.count(neighbor))
        {
            return true;
        }
    }

    // Удалить из стека рекурсии при возвращении
    rec_stack.erase(current_id);
    return false;
}
