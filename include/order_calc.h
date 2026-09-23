#pragma once

#include <vector>

double applyDiscount(double total, bool isPremium);
double calcShipping(double total);
double finalPrice(double total, double discount, double shipping);
int countExpensiveItems(const std::vector<double>& prices, double threshold);

// Individual assignment, variant 4.
void reversePrices(std::vector<double>& prices);
double minOnAndAboveSecondaryDiagonal(
    const std::vector<std::vector<double>>& matrix);
