#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>
#include "datafiltering.h"

class StatEngine {
private:
    std::mt19937_64 rng;

public:
    explicit StatEngine(std::uint64_t seed = 42) : rng(seed) {}

    std::vector<double> run_bootstrap(const std::vector<double>& source_returns)
    {
        if (source_returns.empty()) {
            throw std::invalid_argument("Cannot bootstrap an empty data set");
        }

        std::uniform_int_distribution<std::size_t> distribution(
            0, source_returns.size() - 1);
        std::vector<double> bootstrap_returns(source_returns.size());
        for (double& value : bootstrap_returns) {
            value = source_returns[distribution(rng)];
        }
        return bootstrap_returns;
    }

    double calculate_permutation(const std::vector<double>& asset_a,
                                 const std::vector<double>& asset_b,
                                 int simulations)
    {
        if (asset_a.empty() || asset_b.empty() || simulations <= 0) {
            throw std::invalid_argument(
                "Both data sets must be non-empty and simulations must be positive");
        }

        const std::size_t size_a = asset_a.size();
        const double observed_difference =
            std::accumulate(asset_a.begin(), asset_a.end(), 0.0) / size_a -
            std::accumulate(asset_b.begin(), asset_b.end(), 0.0) / asset_b.size();

        std::vector<double> combined;
        combined.reserve(size_a + asset_b.size());
        combined.insert(combined.end(), asset_a.begin(), asset_a.end());
        combined.insert(combined.end(), asset_b.begin(), asset_b.end());

        std::ofstream csv_export("shuffled_differences.csv");
        if (csv_export) {
            csv_export << "Simulation_ID,Permuted_Difference\n";
            csv_export << std::setprecision(12);
        }

        int extreme_results = 0;
        for (int i = 0; i < simulations; ++i) {
            std::shuffle(combined.begin(), combined.end(), rng);
            const double permuted_difference =
                std::accumulate(combined.begin(), combined.begin() + size_a, 0.0) /
                    size_a -
                std::accumulate(combined.begin() + size_a, combined.end(), 0.0) /
                    asset_b.size();

            if (csv_export) {
                csv_export << i << ',' << permuted_difference << '\n';
            }

            if (std::abs(permuted_difference) >= std::abs(observed_difference)) {
                ++extreme_results;
            }
        }

        if (csv_export) {
            csv_export.close();
            std::cout << "Exported " << simulations
                      << " data rows to 'shuffled_differences.csv'\n";
        } else {
            std::cerr << "Warning: Could not open 'shuffled_differences.csv'\n";
        }

        return static_cast<double>(extreme_results) / simulations;
    }
};

int main()
{
    try {
        const std::vector<double> apple_returns = calculate("aapl_us_d.csv");
        const std::vector<double> samsung_returns = calculate("smsn_uk_d.csv");
        const int simulations = 10000;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "\nPercentage Changes by Trade\n";
        std::cout << std::string(52, '-') << '\n';
        std::cout << std::setw(12) << "Trade #"
                  << std::setw(20) << "Apple % change"
                  << std::setw(20) << "Samsung % change" << '\n';
        std::cout << std::string(52, '-') << '\n';

        const std::size_t trade_count =
            std::min(apple_returns.size(), samsung_returns.size());
        for (std::size_t index = 0; index < trade_count; ++index) {
            std::cout << std::setw(12) << index + 1
                      << std::setw(19) << apple_returns[index] << "%"
                      << std::setw(19) << samsung_returns[index] << "%";
            std::cout << '\n';
        }

        std::cout << std::string(52, '-') << '\n';

        StatEngine stats_engine(1234567);
        const double p_value = stats_engine.calculate_permutation(
            apple_returns, samsung_returns, simulations);

        std::cout << "P-value: " << p_value << '\n';
        std::cout << (p_value < 0.05 ? "Reject" : "Fail to reject")
                  << " the null hypothesis.\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}