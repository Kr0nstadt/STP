#include "task.h"

#include <utility>

namespace
{
bool IsValidStatus(const std::string& status)
{
    return status == "Todo" || status == "In Progress" || status == "Done";
}

bool IsValidPriority(const std::string& priority)
{
    return priority == "Low" || priority == "Medium" || priority == "High";
}
}

Task::Task(int id, std::string title, int projectId)
    : id(id),
      projectId(projectId),
      title(std::move(title)),
      description(),
      status("Todo"),
      priority("Medium")
{
}

int Task::GetId() const
{
    return id;
}

int Task::GetProjectId() const
{
    return projectId;
}

std::string Task::GetTitle() const
{
    return title;
}

std::string Task::GetDescription() const
{
    return description;
}

std::string Task::GetStatus() const
{
    return status;
}

std::string Task::GetPriority() const
{
    return priority;
}

void Task::SetProjectId(int projectId)
{
    this->projectId = projectId;
}

void Task::SetTitle(std::string title)
{
    this->title = std::move(title);
}

void Task::SetDescription(std::string description)
{
    this->description = std::move(description);
}

void Task::SetStatus(std::string status)
{
    if (IsValidStatus(status))
    {
        this->status = std::move(status);
    }
}

void Task::SetPriority(std::string priority)
{
    if (IsValidPriority(priority))
    {
        this->priority = std::move(priority);
    }
}
