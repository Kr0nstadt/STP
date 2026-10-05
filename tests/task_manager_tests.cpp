#include <gtest/gtest.h>

#include "task.h"
#include "taskmanager.h"

TEST(TaskTests, ConstructorSetsAllValues)
{
    Task task(7, "Write report", 3);

    EXPECT_EQ(task.GetId(), 7);
    EXPECT_EQ(task.GetProjectId(), 3);
    EXPECT_EQ(task.GetTitle(), "Write report");
    EXPECT_TRUE(task.GetDescription().empty());
    EXPECT_EQ(task.GetStatus(), "Todo");
    EXPECT_EQ(task.GetPriority(), "Medium");
}

TEST(TaskTests, SettersUpdateValidValues)
{
    Task task(1, "Draft");

    task.SetProjectId(4);
    task.SetTitle("Release");
    task.SetDescription("Publish release");
    task.SetStatus("Done");
    task.SetPriority("High");

    EXPECT_EQ(task.GetProjectId(), 4);
    EXPECT_EQ(task.GetTitle(), "Release");
    EXPECT_EQ(task.GetDescription(), "Publish release");
    EXPECT_EQ(task.GetStatus(), "Done");
    EXPECT_EQ(task.GetPriority(), "High");
}

TEST(TaskTests, InvalidStatusAndPriorityLeaveValuesUnchanged)
{
    Task task(1, "Task");
    task.SetStatus("In Progress");
    task.SetPriority("Low");

    task.SetStatus("Closed");
    task.SetPriority("Urgent");

    EXPECT_EQ(task.GetStatus(), "In Progress");
    EXPECT_EQ(task.GetPriority(), "Low");
}

TEST(TaskManagerTests, AddsAndUpdatesLinkedTask)
{
    TaskManager manager;
    manager.AddProject("Course");
    manager.AddTask("Lab", "Implement", "High", "In Progress", 1);

    manager.UpdateTask(0, "Lab 4", "Verify", "Medium", "Done");

    const std::vector<Task> tasks = manager.GetTasks();
    ASSERT_EQ(tasks.size(), 1U);
    EXPECT_EQ(tasks[0].GetId(), 1);
    EXPECT_EQ(tasks[0].GetProjectId(), 1);
    EXPECT_EQ(tasks[0].GetTitle(), "Lab 4");
    EXPECT_EQ(tasks[0].GetDescription(), "Verify");
    EXPECT_EQ(tasks[0].GetPriority(), "Medium");
    EXPECT_EQ(tasks[0].GetStatus(), "Done");
}

TEST(TaskManagerTests, MovesAndUnlinksTask)
{
    TaskManager manager;
    manager.AddProject("First");
    manager.AddProject("Second");
    manager.AddTask("Task", "", "Medium", "Todo", 1);

    EXPECT_TRUE(manager.MoveTaskToProject(0, 2));
    EXPECT_EQ(manager.GetTasks()[0].GetProjectId(), 2);
    EXPECT_TRUE(manager.MoveTaskToProject(0, 0));
    EXPECT_EQ(manager.GetTasks()[0].GetProjectId(), 0);
}

TEST(TaskManagerTests, RejectsInvalidMoveWithoutChangingTask)
{
    TaskManager manager;
    manager.AddProject("Project");
    manager.AddTask("Task", "", "Medium", "Todo", 1);

    EXPECT_FALSE(manager.MoveTaskToProject(-1, 1));
    EXPECT_FALSE(manager.MoveTaskToProject(0, 99));
    EXPECT_EQ(manager.GetTasks()[0].GetProjectId(), 1);
}

TEST(TaskManagerTests, CalculatesProjectStatistics)
{
    TaskManager manager;
    manager.AddProject("First");
    manager.AddProject("Second");
    manager.AddTask("A", "", "Low", "Done", 1);
    manager.AddTask("B", "", "Medium", "Todo", 1);
    manager.AddTask("C", "", "High", "Done", 1);
    manager.AddTask("D", "", "Low", "Done", 2);
    manager.AddTask("E", "", "Low", "Done", 0);

    EXPECT_EQ(manager.CountTasksInProject(1), 3);
    EXPECT_EQ(manager.CountTasksInProjectByStatus(1, "Done"), 2);
    EXPECT_DOUBLE_EQ(manager.CompletionPercent(1), 200.0 / 3.0);
    EXPECT_DOUBLE_EQ(manager.CompletionPercent(99), 0.0);
}

TEST(TaskManagerTests, DeletesProjectTasksOnly)
{
    TaskManager manager;
    manager.AddProject("Delete");
    manager.AddProject("Keep");
    manager.AddTask("Delete first", "", "Low", "Todo", 1);
    manager.AddTask("Keep project", "", "Low", "Todo", 2);
    manager.AddTask("Delete second", "", "Low", "Done", 1);
    manager.AddTask("Keep unlinked", "", "Low", "Todo", 0);

    manager.DeleteProject(0);

    std::vector<Project> projects = manager.GetProjects();
    const std::vector<Task> tasks = manager.GetTasks();
    ASSERT_EQ(projects.size(), 1U);
    ASSERT_EQ(tasks.size(), 2U);
    EXPECT_EQ(projects[0].getId(), 2);
    EXPECT_EQ(tasks[0].GetTitle(), "Keep project");
    EXPECT_EQ(tasks[1].GetTitle(), "Keep unlinked");
}

TEST(TaskManagerTests, InvalidIndexesDoNotModifyRepositories)
{
    TaskManager manager;
    manager.AddProject("Project");
    manager.AddTask("Task", "", "Medium", "Todo", 1);

    manager.UpdateTask(3, "Changed", "", "High", "Done");
    manager.DeleteTask(-1);
    manager.DeleteProject(4);

    EXPECT_EQ(manager.GetTasks().size(), 1U);
    EXPECT_EQ(manager.GetTasks()[0].GetTitle(), "Task");
    EXPECT_EQ(manager.GetProjects().size(), 1U);
}
