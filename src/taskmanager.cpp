#include "taskmanager.h"

#include <utility>

TaskManager::TaskManager()
    : nextTaskId(1), nextProjectId(1)
{
}

void TaskManager::AddTask(std::string title,
                          std::string description,
                          std::string priority,
                          std::string status,
                          int projectId)
{
    Task task(nextTaskId, std::move(title), projectId);
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
        return;
    }

    Task task = std::move(tasks[index]);
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

std::vector<Task> TaskManager::GetTasks()
{
    return taskRepository.GetAll();
}

bool TaskManager::MoveTaskToProject(int taskIndex, int projectId)
{
    std::vector<Task> tasks = taskRepository.GetAll();
    if (taskIndex < 0 || taskIndex >= static_cast<int>(tasks.size()))
    {
        return false;
    }

    if (projectId != 0)
    {
        bool projectExists = false;
        for (Project& project : projectRepository.GetAll())
        {
            if (project.getId() == projectId)
            {
                projectExists = true;
                break;
            }
        }

        if (!projectExists)
        {
            return false;
        }
    }

    Task task = std::move(tasks[taskIndex]);
    task.SetProjectId(projectId);
    taskRepository.Update(taskIndex, std::move(task));
    return true;
}

int TaskManager::CountTasksInProject(int projectId)
{
    int count = 0;
    for (const Task& task : taskRepository.GetAll())
    {
        if (task.GetProjectId() == projectId)
        {
            ++count;
        }
    }
    return count;
}

int TaskManager::CountTasksInProjectByStatus(int projectId, std::string status)
{
    int count = 0;
    for (const Task& task : taskRepository.GetAll())
    {
        if (task.GetProjectId() == projectId && task.GetStatus() == status)
        {
            ++count;
        }
    }
    return count;
}

double TaskManager::CompletionPercent(int projectId)
{
    int total = 0;
    int completed = 0;
    for (const Task& task : taskRepository.GetAll())
    {
        if (task.GetProjectId() != projectId)
        {
            continue;
        }

        ++total;
        if (task.GetStatus() == "Done")
        {
            ++completed;
        }
    }

    return total == 0 ? 0.0 : 100.0 * completed / total;
}

void TaskManager::AddProject(std::string name)
{
    projectRepository.Add(Project(nextProjectId, std::move(name)));
    ++nextProjectId;
}

void TaskManager::DeleteProject(int index)
{
    std::vector<Project> projects = projectRepository.GetAll();
    if (index < 0 || index >= static_cast<int>(projects.size()))
    {
        return;
    }

    const int projectId = projects[index].getId();
    std::vector<Task> tasks = taskRepository.GetAll();
    for (int taskIndex = static_cast<int>(tasks.size()) - 1; taskIndex >= 0; --taskIndex)
    {
        if (tasks[taskIndex].GetProjectId() == projectId)
        {
            taskRepository.Remove(taskIndex);
        }
    }

    projectRepository.Remove(index);
}

std::vector<Project> TaskManager::GetProjects()
{
    return projectRepository.GetAll();
}
