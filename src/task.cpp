#include "task.h"

// ============================================================
//  TODO: реализуйте ВСЕ методы Task по спецификации.
//
//  Конструктор:
//    - id           = переданный id
//    - projectId    = переданный projectId (0 = без проекта)
//    - title        = переданный title
//    - description  = ""
//    - status       = "Todo"
//    - priority     = "Medium"
//
//  Геттеры (GetId, GetProjectId, GetTitle, GetDescription,
//  GetStatus, GetPriority) — возвращают значения полей.
//  Методы помечены const — не меняют объект.
//
//  Сеттеры (SetProjectId, SetTitle, SetDescription,
//  SetStatus, SetPriority) — присваивают значения.
//  SetStatus принимает только "Todo" | "In Progress" | "Done".
//  SetPriority принимает только "Low" | "Medium" | "High".
//  При недопустимом значении поле НЕ меняется.
// ============================================================

Task::Task(int id, std::string title, int projectId)
{
    // TODO: инициализировать поля
    (void)id;
    (void)title;
    (void)projectId;
}

int Task::GetId() const
{
    // TODO
    return 0;
}

int Task::GetProjectId() const
{
    // TODO
    return 0;
}

std::string Task::GetTitle() const
{
    // TODO
    return "";
}

std::string Task::GetDescription() const
{
    // TODO
    return "";
}

std::string Task::GetStatus() const
{
    // TODO
    return "";
}

std::string Task::GetPriority() const
{
    // TODO
    return "";
}

void Task::SetProjectId(int projectId)
{
    // TODO
    (void)projectId;
}

void Task::SetTitle(std::string title)
{
    // TODO
    (void)title;
}

void Task::SetDescription(std::string description)
{
    // TODO
    (void)description;
}

void Task::SetStatus(std::string status)
{
    // TODO
    (void)status;
}

void Task::SetPriority(std::string priority)
{
    // TODO
    (void)priority;
}