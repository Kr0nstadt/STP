#include <gtest/gtest.h>

#include <stdexcept>

#include "project.h"
#include "repository.h"
#include "task.h"
#include "taskmanager.h"

TEST(TaskTests, ConstructorSetsRequiredDefaults)
{
    Task task(7, "Write report");

    EXPECT_EQ(task.GetId(), 7);
    EXPECT_EQ(task.GetTitle(), "Write report");
    EXPECT_EQ(task.GetDescription(), "");
    EXPECT_EQ(task.GetStatus(), "Todo");
    EXPECT_EQ(task.GetPriority(), "Medium");
}

TEST(TaskTests, SettersChangeTaskData)
{
    Task task(1, "Draft");

    task.SetTitle("Final report");
    task.SetDescription("Add test results");
    task.SetStatus("In Progress");
    task.SetPriority("High");

    EXPECT_EQ(task.GetTitle(), "Final report");
    EXPECT_EQ(task.GetDescription(), "Add test results");
    EXPECT_EQ(task.GetStatus(), "In Progress");
    EXPECT_EQ(task.GetPriority(), "High");
}

TEST(TaskTests, StatusAcceptsOnlySpecifiedValues)
{
    Task task(1, "Task");

    for (const std::string status : {"Todo", "In Progress", "Done"})
    {
        EXPECT_NO_THROW(task.SetStatus(status));
        EXPECT_EQ(task.GetStatus(), status);
    }

    EXPECT_THROW(task.SetStatus("Closed"), std::invalid_argument);
    EXPECT_EQ(task.GetStatus(), "Done");
}

TEST(TaskTests, PriorityAcceptsOnlySpecifiedValues)
{
    Task task(1, "Task");

    for (const std::string priority : {"Low", "Medium", "High"})
    {
        EXPECT_NO_THROW(task.SetPriority(priority));
        EXPECT_EQ(task.GetPriority(), priority);
    }

    EXPECT_THROW(task.SetPriority("Urgent"), std::invalid_argument);
    EXPECT_EQ(task.GetPriority(), "High");
}

TEST(RepositoryTests, AddsAndReturnsObjectsInInsertionOrder)
{
    Repository<Task> repository;
    repository.Add(Task(1, "First"));
    repository.Add(Task(2, "Second"));

    const std::vector<Task> tasks = repository.GetAll();
    ASSERT_EQ(repository.Size(), 2);
    EXPECT_EQ(tasks[0].GetTitle(), "First");
    EXPECT_EQ(tasks[1].GetTitle(), "Second");
}

TEST(RepositoryTests, UpdatesAndRemovesObjects)
{
    Repository<Project> repository;
    repository.Add(Project(1, "Old name"));
    repository.Add(Project(2, "Keep"));

    repository.Update(0, Project(1, "New name"));
    EXPECT_EQ(repository.GetAll()[0].getName(), "New name");

    repository.Remove(0);
    ASSERT_EQ(repository.Size(), 1);
    EXPECT_EQ(repository.GetAll()[0].getId(), 2);
}

TEST(RepositoryTests, ClearRemovesEveryObject)
{
    Repository<Task> repository;
    repository.Add(Task(1, "First"));
    repository.Add(Task(2, "Second"));

    repository.Clear();

    EXPECT_EQ(repository.Size(), 0);
    EXPECT_TRUE(repository.GetAll().empty());
}

TEST(RepositoryTests, RejectsNegativeAndTooLargeIndexes)
{
    Repository<Task> repository;
    repository.Add(Task(1, "Only"));

    EXPECT_THROW(repository.Remove(-1), std::out_of_range);
    EXPECT_THROW(repository.Remove(1), std::out_of_range);
    EXPECT_THROW(repository.Update(2, Task(2, "Other")), std::out_of_range);
    EXPECT_EQ(repository.Size(), 1);
}

TEST(TaskManagerTests, StartsWithEmptyRepositories)
{
    TaskManager manager;

    EXPECT_TRUE(manager.GetTasks().empty());
    EXPECT_TRUE(manager.GetProjects().empty());
}

TEST(TaskManagerTests, AddsTasksWithSequentialIdentifiers)
{
    TaskManager manager;
    manager.AddTask("First", "Description 1", "Low", "Todo");
    manager.AddTask("Second", "Description 2", "High", "Done");

    const std::vector<Task> tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 2U);
    EXPECT_EQ(tasks[0].GetId(), 1);
    EXPECT_EQ(tasks[1].GetId(), 2);
    EXPECT_EQ(tasks[1].GetPriority(), "High");
    EXPECT_EQ(tasks[1].GetStatus(), "Done");
}

TEST(TaskManagerTests, FailedTaskCreationDoesNotConsumeIdentifier)
{
    TaskManager manager;
    EXPECT_THROW(
        manager.AddTask("Invalid", "", "Urgent", "Todo"),
        std::invalid_argument);

    manager.AddTask("Valid", "", "Medium", "Todo");

    ASSERT_EQ(manager.GetTasks().size(), 1U);
    EXPECT_EQ(manager.GetTasks()[0].GetId(), 1);
}

TEST(TaskManagerTests, UpdatesTaskWithoutChangingIdentifier)
{
    TaskManager manager;
    manager.AddTask("Draft", "", "Low", "Todo");

    manager.UpdateTask(0, "Ready", "Reviewed", "High", "Done");

    const Task task = manager.GetTasks()[0];
    EXPECT_EQ(task.GetId(), 1);
    EXPECT_EQ(task.GetTitle(), "Ready");
    EXPECT_EQ(task.GetDescription(), "Reviewed");
    EXPECT_EQ(task.GetPriority(), "High");
    EXPECT_EQ(task.GetStatus(), "Done");
}

TEST(TaskManagerTests, DeletesTasksAndChecksIndexes)
{
    TaskManager manager;
    manager.AddTask("First", "", "Medium", "Todo");
    manager.AddTask("Second", "", "Medium", "Todo");

    manager.DeleteTask(0);

    ASSERT_EQ(manager.GetTasks().size(), 1U);
    EXPECT_EQ(manager.GetTasks()[0].GetTitle(), "Second");
    EXPECT_THROW(manager.DeleteTask(2), std::out_of_range);
    EXPECT_THROW(
        manager.UpdateTask(-1, "", "", "Low", "Todo"),
        std::out_of_range);
}

TEST(TaskManagerTests, AddsAndDeletesProjectsWithSequentialIdentifiers)
{
    TaskManager manager;
    manager.AddProject("Study");
    manager.AddProject("Personal");

    ASSERT_EQ(manager.GetProjects().size(), 2U);
    EXPECT_EQ(manager.GetProjects()[0].getId(), 1);
    EXPECT_EQ(manager.GetProjects()[1].getId(), 2);

    manager.DeleteProject(0);
    ASSERT_EQ(manager.GetProjects().size(), 1U);
    EXPECT_EQ(manager.GetProjects()[0].getName(), "Personal");
}
