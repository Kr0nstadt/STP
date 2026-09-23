#include <gtest/gtest.h>

#include "order_calc.h"

TEST(ApplyDiscountTests, RejectsNegativeTotal) {
    EXPECT_DOUBLE_EQ(applyDiscount(-1.0, false), -1.0);
}

TEST(ApplyDiscountTests, KeepsSmallOrderTotal) {
    EXPECT_DOUBLE_EQ(applyDiscount(4999.0, false), 4999.0);
}

TEST(ApplyDiscountTests, AppliesFivePercentRegularDiscount) {
    EXPECT_DOUBLE_EQ(applyDiscount(5000.0, false), 4750.0);
}

TEST(ApplyDiscountTests, AppliesTenPercentPremiumDiscount) {
    EXPECT_DOUBLE_EQ(applyDiscount(7500.0, true), 6750.0);
}

TEST(ApplyDiscountTests, AppliesFifteenPercentRegularDiscount) {
    EXPECT_DOUBLE_EQ(applyDiscount(10000.0, false), 8500.0);
}

TEST(ApplyDiscountTests, AppliesTwentyPercentPremiumDiscount) {
    EXPECT_DOUBLE_EQ(applyDiscount(12000.0, true), 9600.0);
}

TEST(ShippingTests, RejectsNegativeTotal) {
    EXPECT_DOUBLE_EQ(calcShipping(-0.01), -1.0);
}

TEST(ShippingTests, ChargesForOrderBelowBoundary) {
    EXPECT_DOUBLE_EQ(calcShipping(4999.99), 300.0);
}

TEST(ShippingTests, IsFreeFromFiveThousand) {
    EXPECT_DOUBLE_EQ(calcShipping(5000.0), 0.0);
    EXPECT_DOUBLE_EQ(calcShipping(10000.0), 0.0);
}

TEST(FinalPriceTests, RejectsEachNegativeArgument) {
    EXPECT_DOUBLE_EQ(finalPrice(-1.0, 0.0, 0.0), -1.0);
    EXPECT_DOUBLE_EQ(finalPrice(1.0, -1.0, 0.0), -1.0);
    EXPECT_DOUBLE_EQ(finalPrice(1.0, 0.0, -1.0), -1.0);
}

TEST(FinalPriceTests, ClampsNegativeResultToZero) {
    EXPECT_DOUBLE_EQ(finalPrice(100.0, 200.0, 50.0), 0.0);
}

TEST(FinalPriceTests, CalculatesAndRoundsNormalResult) {
    EXPECT_DOUBLE_EQ(finalPrice(1000.125, 200.0, 100.0), 900.13);
}

TEST(ExpensiveItemsTests, ReturnsZeroForEmptyVector) {
    EXPECT_EQ(countExpensiveItems({}, 100.0), 0);
}

TEST(ExpensiveItemsTests, CountsOnlyPricesStrictlyAboveThreshold) {
    EXPECT_EQ(countExpensiveItems({99.0, 100.0, 101.0, 250.0}, 100.0), 2);
}

TEST(ExpensiveItemsTests, ReturnsZeroWhenNoPriceIsAboveThreshold) {
    EXPECT_EQ(countExpensiveItems({10.0, 20.0}, 50.0), 0);
}

TEST(ReversePricesTests, LeavesEmptyVectorUnchanged) {
    std::vector<double> prices;
    reversePrices(prices);
    EXPECT_TRUE(prices.empty());
}

TEST(ReversePricesTests, LeavesSinglePriceUnchanged) {
    std::vector<double> prices{15.0};
    reversePrices(prices);
    EXPECT_EQ(prices, (std::vector<double>{15.0}));
}

TEST(ReversePricesTests, ReversesEvenNumberOfPrices) {
    std::vector<double> prices{10.0, 20.0, 30.0, 40.0};
    reversePrices(prices);
    EXPECT_EQ(prices, (std::vector<double>{40.0, 30.0, 20.0, 10.0}));
}

TEST(ReversePricesTests, ReversesOddNumberOfPrices) {
    std::vector<double> prices{10.0, 20.0, 30.0};
    reversePrices(prices);
    EXPECT_EQ(prices, (std::vector<double>{30.0, 20.0, 10.0}));
}

TEST(MatrixMinimumTests, ReturnsZeroForEmptyMatrix) {
    EXPECT_DOUBLE_EQ(minOnAndAboveSecondaryDiagonal({}), 0.0);
}

TEST(MatrixMinimumTests, ReturnsOnlyValueForOneByOneMatrix) {
    EXPECT_DOUBLE_EQ(minOnAndAboveSecondaryDiagonal({{7.5}}), 7.5);
}

TEST(MatrixMinimumTests, ConsidersOnlyOnAndAboveSecondaryDiagonal) {
    const std::vector<std::vector<double>> matrix{
        {8.0, 2.0, 7.0},
        {9.0, 4.0, -10.0},
        {3.0, -20.0, 6.0},
    };

    EXPECT_DOUBLE_EQ(minOnAndAboveSecondaryDiagonal(matrix), 2.0);
}

TEST(MatrixMinimumTests, HandlesMatrixWithoutEligibleElements) {
    const std::vector<std::vector<double>> matrix{{}, {}, {}};
    EXPECT_DOUBLE_EQ(minOnAndAboveSecondaryDiagonal(matrix), 0.0);
}
