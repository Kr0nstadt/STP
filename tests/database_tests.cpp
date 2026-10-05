#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>

#include <pqxx/pqxx>

#include "db.h"

class DatabaseTests : public testing::Test
{
protected:
    void SetUp() override
    {
        const char* value = std::getenv("PICKUP_TEST_DB");
        if (value == nullptr)
        {
            GTEST_SKIP() << "PICKUP_TEST_DB is not set";
        }

        connectionString = value;
        pqxx::connection connection(connectionString);
        pqxx::work transaction(connection);
        transaction.exec("TRUNCATE TABLE orders RESTART IDENTITY");
        transaction.commit();
        database = std::make_unique<Database>(connectionString);
    }

    std::string connectionString;
    std::unique_ptr<Database> database;
};

TEST_F(DatabaseTests, AddsPhoneAndArticleOrders)
{
    const std::string phoneCode = database->addOrder("+7 (999) 000-11-22", "A1");
    const std::string articleCode = database->addOrder("ART-123456789", "");

    EXPECT_EQ(phoneCode, "PVZ-000001");
    EXPECT_EQ(articleCode, "PVZ-000002");

    const std::vector<Order> orders = database->listOrders();
    ASSERT_EQ(orders.size(), 2U);
    EXPECT_EQ(orders[0].code, articleCode);
    EXPECT_TRUE(orders[0].phone.empty());
    EXPECT_EQ(orders[0].article, "ART-123456789");
    EXPECT_TRUE(orders[0].cell.empty());
    EXPECT_EQ(orders[1].phone, "+7 (999) 000-11-22");
    EXPECT_TRUE(orders[1].article.empty());
    EXPECT_EQ(orders[1].cell, "A1");
}

TEST_F(DatabaseTests, SearchesAllSupportedFieldsSafely)
{
    database->addOrder("+79990001122", "A1");
    database->addOrder("ART-ABC-42", "B2");

    EXPECT_EQ(database->listOrders("+7999").size(), 1U);
    EXPECT_EQ(database->listOrders("art-abc").size(), 1U);
    EXPECT_EQ(database->listOrders("000002").size(), 1U);
    EXPECT_TRUE(database->listOrders("' OR '1'='1").empty());
}

TEST_F(DatabaseTests, IssuesAndCancelsReadyOrdersOnly)
{
    const std::string issuedCode = database->addOrder("+79990001122", "A1");
    const std::string cancelledCode = database->addOrder("ART-123456", "B2");

    EXPECT_TRUE(database->issueOrder(issuedCode));
    EXPECT_FALSE(database->issueOrder(issuedCode));
    EXPECT_FALSE(database->cancelOrder(issuedCode));
    EXPECT_TRUE(database->cancelOrder(cancelledCode));
    EXPECT_FALSE(database->cancelOrder(cancelledCode));

    EXPECT_EQ(database->listByStatus("issued").size(), 1U);
    EXPECT_EQ(database->listByStatus("cancelled").size(), 1U);
    EXPECT_EQ(database->listByStatus("ready").size(), 0U);
}

TEST_F(DatabaseTests, BuildsReportAndDeletesClosedOrders)
{
    const std::string readyCode = database->addOrder("+79990000001", "A1");
    const std::string issuedCode = database->addOrder("+79990000002", "A2");
    const std::string cancelledCode = database->addOrder("ART-00003", "A3");
    database->issueOrder(issuedCode);
    database->cancelOrder(cancelledCode);

    const Report report = database->buildReport();
    EXPECT_EQ(report.ready_count, 1);
    EXPECT_EQ(report.issued_count, 1);
    EXPECT_EQ(report.cancelled_count, 1);
    EXPECT_EQ(database->countClosedOrders(), 2);
    EXPECT_EQ(database->listByStatus("").size(), 3U);

    EXPECT_EQ(database->deleteClosedOrders(), 2);
    EXPECT_EQ(database->countClosedOrders(), 0);
    const std::vector<Order> remaining = database->listOrders();
    ASSERT_EQ(remaining.size(), 1U);
    EXPECT_EQ(remaining[0].code, readyCode);
}
