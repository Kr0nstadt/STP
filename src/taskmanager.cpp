#include "taskmanager.h"

// ============================================================
//  TODO: реализуйте ВСЕ методы TaskManager по спецификации.
//
//  ----- Базовые операции (были в лабораторной №3) -----
//
//  Конструктор:
//    nextTaskId    = 1
//    nextProjectId = 1
//    Репозитории пусты по умолчанию.
//
//  AddProject(name):
//    - создать Project(nextProjectId, name);
//    - увеличить nextProjectId на 1;
//    - добавить в projectRepository.
//
//  DeleteProject(index):
//    - если index вне диапазона — выйти;
//    - взять id проекта;
//    - УДАЛИТЬ ВСЕ ЗАДАЧИ ЭТОГО ПРОЕКТА (обход С КОНЦА,
//      иначе индексы сдвинутся после Remove);
//    - удалить проект из projectRepository.
//
//  GetProjects():
//    - вернуть projectRepository.GetAll().
//
//  AddTask(title, description, priority, status, projectId):
//    - создать Task(nextTaskId, title, projectId);
//    - увеличить nextTaskId на 1;
//    - установить description, priority, status;
//    - добавить в taskRepository.
//
//  UpdateTask(index, title, description, priority, status):
//    - если index вне диапазона — выйти;
//    - взять задачу по индексу;
//    - поменять title, description, priority, status
//      (id и projectId НЕ трогать!);
//    - положить обратно через taskRepository.Update.
//
//  DeleteTask(index):
//    - taskRepository.Remove(index).
//
//  GetTasks():
//    - вернуть taskRepository.GetAll().
//
//  ----- Новые операции (лабораторная №4) -----
//
//  MoveTaskToProject(taskIndex, projectId):
//    - если taskIndex вне диапазона — вернуть false;
//    - если projectId != 0 и такого проекта нет — вернуть false;
//    - взять задачу, вызвать SetProjectId(projectId);
//    - обновить через taskRepository.Update;
//    - вернуть true.
//    id задачи НЕ меняется.
//
//  CountTasksInProject(projectId):
//    - посчитать задачи, у которых GetProjectId() == projectId.
//
//  CountTasksInProjectByStatus(projectId, status):
//    - посчитать задачи проекта с указанным статусом.
//
//  CompletionPercent(projectId):
//    - если задач в проекте нет — вернуть 0.0;
//    - иначе 100.0 * (число задач со статусом "Done") / (всего задач).
// ============================================================

TaskManager::TaskManager()
{
    // TODO: nextTaskId = 1; nextProjectId = 1;
}

void TaskManager::AddTask(std::string title,
                          std::string description,
                          std::string priority,
                          std::string status,
                          int projectId)
{
    // TODO
    (void)title;
    (void)description;
    (void)priority;
    (void)status;
    (void)projectId;
}

void TaskManager::UpdateTask(int index,
                             std::string title,
                             std::string description,
                             std::string priority,
                             std::string status)
{
    // TODO
    (void)index;
    (void)title;
    (void)description;
    (void)priority;
    (void)status;
}

void TaskManager::DeleteTask(int index)
{
    // TODO
    (void)index;
}

std::vector<Task> TaskManager::GetTasks()
{
    // TODO
    return {};
}

bool TaskManager::MoveTaskToProject(int taskIndex, int projectId)
{
    // TODO
    (void)taskIndex;
    (void)projectId;
    return false;
}

int TaskManager::CountTasksInProject(int projectId)
{
    // TODO
    (void)projectId;
    return 0;
}

int TaskManager::CountTasksInProjectByStatus(int projectId, std::string status)
{
    // TODO
    (void)projectId;
    (void)status;
    return 0;
}

double TaskManager::CompletionPercent(int projectId)
{
    // TODO
    (void)projectId;
    return 0.0;
}

void TaskManager::AddProject(std::string name)
{
    // TODO
    (void)name;
}

void TaskManager::DeleteProject(int index)
{
    // TODO
    (void)index;
}

std::vector<Project> TaskManager::GetProjects()
{
    // TODO
    return {};
}