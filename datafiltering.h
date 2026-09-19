// #pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline std::vector<double> calculate(const std::string& filename)
{
	std::vector<double> data;
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Error opening file: " << filename << '\n';
		return data;
	}

	std::string line;
	std::getline(file, line);
	double last_close = 0.0;
	while (std::getline(file, line)) {
		if (line.empty()) {
			continue;
		}

		std::stringstream row(line);
		std::string cell;
		double close = 0.0;
		for (int column = 1; std::getline(row, cell, ','); ++column) {
			if (column == 5) {
				try {
					close = std::stod(cell);
				} catch (const std::exception&) {
					close = 0.0;
				}
				break;
			}
		}

		if (last_close > 0.0 && close > 0.0) {
			data.push_back((close - last_close) / last_close * 100.0);
		}
		if (close > 0.0) {
			last_close = close;
		}
	}
	return data;
}
