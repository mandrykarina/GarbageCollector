#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "rc_object.h"
#include "event_logger.h"

/**
 * @class ReferenceCounter
 * @brief Реализует подсчёт ссылок (Reference Counting) для управления памятью
 *
 * Отвечает за:
 * - Добавление и удаление ссылок между объектами
 * - Каскадное удаление объектов когда ref_count достигает 0
 * - Обнаружение циклических ссылок (которые вызывают утечку памяти)
 * - Логирование всех операций
 *
 * **ВАЖНО: В этой версии НЕ удаляем автоматически в цикле!**
 * **Только ref_count == 0 -> удаляем. Это показывает проблему RC с циклами!**
 */
class ReferenceCounter
{
public:
    /**
     * @brief Конструктор
     * @param heap Ссылка на контейнер объектов в памяти
     * @param logger Ссылка на логгер событий
     */
    ReferenceCounter(std::unordered_map<int, RCObject> &heap, EventLogger &logger);

    /**
     * @brief Добавить ссылку от одного объекта к другому
     * @param from ID объекта-источника ссылки
     * @param to ID объекта-цели ссылки
     * @return true, если ссылка была успешно добавлена
     */
    bool add_ref(int from, int to);

    /**
     * @brief Удалить ссылку от одного объекта к другому
     * @param from ID объекта-источника ссылки
     * @param to ID объекта-цели ссылки
     * @return true, если ссылка была успешно удалена
     */
    bool remove_ref(int from, int to);

    /**
     * @brief Выполнить каскадное удаление объекта и его зависимостей
     *
     * Удаляет объект только если ref_count == 0, затем рекурсивно
     * удаляет объекты, на которые он ссылается (если их ref_count == 0)
     *
     * @param obj_id ID объекта для удаления
     * @param visited Множество уже посещённых объектов (для предотвращения циклов)
     */
    void cascade_delete(int obj_id, std::unordered_set<int> &visited);

private:
    std::unordered_map<int, RCObject> &heap;
    EventLogger &logger;

    /**
     * @brief Проверить, находится ли цикл в графе ссылок
     * @param start_id ID объекта для начала проверки
     * @return true, если обнаружен цикл
     */
    bool has_cycle(int start_id) const;

    /**
     * @brief Вспомогательная функция для DFS поиска цикла
     * @param current_id Текущий узел DFS
     * @param visited Множество посещённых узлов
     * @param rec_stack Стек рекурсии для обнаружения цикла
     * @return true, если цикл найден
     */
    bool has_cycle_dfs(int current_id,
                       std::unordered_set<int> &visited,
                       std::unordered_set<int> &rec_stack) const;
};

#endif // REFERENCE_COUNTER_H
