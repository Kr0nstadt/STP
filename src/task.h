#ifndef TASK_H
#define TASK_H

#include <string>

class Task
{
private:
    int id;
    std::string title;
    std::string description;
    std::string status;
    std::string priority;

public:
    Task(int id, std::string title);

    int GetId() const;
    std::string GetTitle() const;
    std::string GetDescription() const;
    std::string GetStatus() const;
    std::string GetPriority() const;

    void SetTitle(std::string title);
    void SetDescription(std::string description);
    void SetStatus(std::string status);
    void SetPriority(std::string priority);
};

#endif
