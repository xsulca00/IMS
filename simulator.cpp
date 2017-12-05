#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <tuple>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct Battery {
    Battery(double mc, double mp)
        : maxCapacity{mc*3600}, maxPower{mp} {}

    // TODO: Return back over charged energy
    void Charge(double p) {
        if (currentCapacity + p > maxCapacity) {
            currentCapacity = maxCapacity;
        } else {
            currentCapacity += p;
        }
    }

    double NeedPower(double p) {
        p = min(maxPower, p);
        if(currentCapacity - p > 0.0) {
            currentCapacity -= p;
            return p;
        }

        if (!Empty()) {
            double power {currentCapacity};
            currentCapacity = 0;
            return power;
        }
        return currentCapacity = 0;
    }
    
    bool Empty() const { return currentCapacity <= numeric_limits<double>::epsilon(); }
    bool Full() const { return maxCapacity - currentCapacity <= numeric_limits<double>::epsilon(); }

    double CurrentCapacity() const { return currentCapacity / maxCapacity * 100; }

    double maxCapacity;
    double maxPower;
    double currentCapacity {0.0};
};

pair<double, double> drain(double appliance, double generator) {
    if (generator > appliance)
        return make_pair(appliance, generator - appliance);
    return make_pair(generator, 0.0);
}

double calculate_wattage(double appliance, double solar, Battery& battery, double grid) {
    double sum {0.0};
    double consumed {};
    double grid_consumed {};

    cout << "========\n";

    if (sum < appliance) {
        // solar power
        tie(consumed, solar) = drain(appliance, solar);
        sum += consumed;
        cout << "Solar consumed:    " << consumed << '\n';
    }

    if (sum < appliance) {
        // battery power
        if (!battery.Empty()) {
            double batteryOut {};
            tie(consumed, batteryOut) = drain(appliance-sum, battery.NeedPower(appliance-sum));
            sum += consumed;
            cout << "Battery consumed:  " << consumed << '\n';
        }
    } else if (!battery.Full()) {
        // charge battery
        battery.Charge(solar);
        cout << "Battery charging:  " << solar << '\n';
        solar -= solar;
    }

    if (sum < appliance) {
        // grid power
        tie(grid_consumed, grid) = drain(appliance-sum, grid);
        sum += consumed;
        cout << "Grid consumed:     " << consumed << '\n';
    }

    cout << "\nSolar:             " << solar << '\n'
         << "Battery:           " << battery.currentCapacity << '\n'
         << "Grid:              " << grid << '\n'
         << "Appliance:         " << appliance << "\n";
    cout << "========\n\n";
    return grid_consumed;
}

int main() {
    // do for vector
    
    ofstream plot {"result.txt"};

    if (!plot) {
        throw runtime_error{"Cannot create and open result.txt!"};
    }

    plot.precision(6);
    plot << fixed << 1 << ' ' << 1 << '\n';
    plot << fixed << 2 << ' ' << 2 << '\n';

    ifstream input {"jaro.txt"};
    if (!input) {
        throw runtime_error{"Cannot open jaro.txt!"};
    }

    double grid {numeric_limits<double>::max()};
    Battery battery {15.0, 5.0};

    vector<pair<double, double>> values;

    for (string s; getline(input, s);) {
        double appliance {};
        char delimiter {};
        double solarPower {};

        istringstream ss {s};

        ss >> appliance >> delimiter >>  solarPower;

        values.emplace_back(appliance, solarPower);
    }


    double sum {};

    for (const auto& p : values) {
        double appliance {p.first};
        double solarPower {p.second};

        sum += calculate_wattage(appliance, solarPower, battery, grid);
    }


    double sum2 {0.0};
    Battery battery2 {0.0, 0.0};
    // charge my battery :)
    for (const auto& p : values) {
        double appliance {p.first};

        sum2 += calculate_wattage(appliance, 0.0, battery2, grid);
    }

    cout << setprecision(4) << "Energy from grid: " << sum << " kWh" << '\n';
    cout << setprecision(4) << "Energy without solar and battery: " << sum2 << " kWh" << '\n';

    return 0;
}
