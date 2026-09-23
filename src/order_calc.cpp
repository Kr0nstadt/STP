#include "order_calc.h"

#include <cmath>
#include <utility>

double applyDiscount(double total, bool isPremium) {
    if (total < 0) {
        return -1.0;
    }
    if (total >= 10000) {
        if (isPremium) {
            return total * 0.80;
        }
        return total * 0.85;
    }
    if (total >= 5000) {
        if (isPremium) {
            return total * 0.90;
        }
        return total * 0.95;
    }
    return total;
}

double calcShipping(double total) {
    if (total < 0) {
        return -1.0;
    }
    if (total >= 5000) {
        return 0.0;
    }
    return 300.0;
}

double finalPrice(double total, double discount, double shipping) {
    if (total < 0 || discount < 0 || shipping < 0) {
        return -1.0;
    }

    double result = total - discount + shipping;
    if (result < 0) {
        result = 0.0;
    }
    return std::round(result * 100.0) / 100.0;
}

int countExpensiveItems(
    const std::vector<double>& prices,
    double threshold) {
    int count = 0;
    for (double price : prices) {
        if (price > threshold) {
            ++count;
        }
    }
    return count;
}

void reversePrices(std::vector<double>& prices) {
    if (prices.empty()) {
        return;
    }

    std::size_t left = 0;
    std::size_t right = prices.size() - 1;
    while (left < right) {
        std::swap(prices[left], prices[right]);
        ++left;
        --right;
    }
}

double minOnAndAboveSecondaryDiagonal(
    const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) {
        return 0.0;
    }

    const std::size_t size = matrix.size();
    double minimum = 0.0;
    bool found = false;

    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t column = 0; column < matrix[row].size(); ++column) {
            if (row + column <= size - 1) {
                if (!found || matrix[row][column] < minimum) {
                    minimum = matrix[row][column];
                    found = true;
                }
            }
        }
    }

    return found ? minimum : 0.0;
}
