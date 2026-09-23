#include "taskmanager.h"

#include <utility>

TaskManager::TaskManager()
    : nextTaskId(1), nextProjectId(1)
{
}

void TaskManager::AddTask(std::string title,
                          std::string description,
                          std::string priority,
                          std::string status)
{
    Task task(nextTaskId, std::move(title));
    task.SetDescription(std::move(description));
    task.SetPriority(std::move(priority));
    task.SetStatus(std::move(status));

    taskRepository.Add(std::move(task));
    ++nextTaskId;
}

void TaskManager::UpdateTask(int index,
                             std::string title,
                             std::string description,
                             std::string priority,
                             std::string status)
{
    std::vector<Task> tasks = taskRepository.GetAll();
    if (index < 0 || index >= static_cast<int>(tasks.size()))
    {
        throw std::out_of_range("task index out of range");
    }

    Task task = tasks[index];
    task.SetTitle(std::move(title));
    task.SetDescription(std::move(description));
    task.SetPriority(std::move(priority));
    task.SetStatus(std::move(status));
    taskRepository.Update(index, std::move(task));
}

void TaskManager::DeleteTask(int index)
{
    taskRepository.Remove(index);
}

std::vector<Task> TaskManager::GetTasks() const
{
    return taskRepository.GetAll();
}

void TaskManager::AddProject(std::string name)
{
    projectRepository.Add(Project(nextProjectId, std::move(name)));
    ++nextProjectId;
}

void TaskManager::DeleteProject(int index)
{
    projectRepository.Remove(index);
}

std::vector<Project> TaskManager::GetProjects() const
{
    return projectRepository.GetAll();
}
