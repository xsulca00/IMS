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
        : maxCapacity{mc*15*60}, maxPower{mp*15*60} {}

    double Charge(double p) {
        if (currentCapacity + p > maxCapacity) {
            currentCapacity = maxCapacity;
            return p - currentCapacity;
        } else {
            currentCapacity += p;
            return p;
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

ofstream result {"result.txt"};

double calculate_wattage(bool write, double appliance, double solar, Battery& battery, double grid) {
    static int n {};

    double sum {};

    double solarConsumed {};
    double batteryConsumed {};
    double gridConsumed {};

    cout << "========\n";

    if (sum < appliance && solar > 0) {
        // solar power
        tie(solarConsumed, solar) = drain(appliance, solar);
        sum += solarConsumed;
        cout << "Solar consumed:    " << solarConsumed << '\n';
    }

    if (solar > 0) {
        if (!battery.Full()) {
            // charge battery
            double charged {battery.Charge(solar)};
            solar -= charged;
            cout << "Battery charging:  " << charged << '\n';
        }
    } else if (sum < appliance && !battery.Empty()) {
        // battery power
        double batteryOut {};
        tie(batteryConsumed, batteryOut) = drain(appliance-sum, battery.NeedPower(appliance-sum));
        sum += batteryConsumed;
        cout << "Battery consumed:  " << batteryConsumed << '\n';
    }

    if (sum < appliance) {
        // grid power
        tie(gridConsumed, grid) = drain(appliance-sum, grid);
        sum += gridConsumed;
        cout << "Grid consumed:     " << gridConsumed << '\n';
    }

    if (write) {
        result << n << ' ' << solarConsumed << ' ' << batteryConsumed << ' ' << gridConsumed << ' ' << appliance << '\n';
        n += 15;
    } else {
        n = 0;
    }

    cout << "\nSolar:             " << solar << '\n'
         << "Battery:           " << battery.currentCapacity << '\n'
         << "Grid:              " << grid << '\n'
         << "Appliance:         " << appliance << "\n";
    cout << "========\n\n";
    return gridConsumed;
}

vector<pair<double, double>> LoadValuesFromFile(const string& name) {
    ifstream input {name};
    if (!input) {
        throw runtime_error{"Cannot open: " + name};
    }

    vector<pair<double, double>> values;

    for (string s; getline(input, s);) {
        double appliance {};
        double solarPower {};

        istringstream ss {s};

        if (!(ss >> appliance >> ws >>  solarPower) || !(ss >> ws).eof()) {
            cerr << "Invalid line: " << ss.str() << '\n';
            continue;
        }

        values.emplace_back(appliance, solarPower);
    }

    return values;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        throw runtime_error{string{"Usage: "} + argv[0] + " file"};
    }

    auto values = LoadValuesFromFile(argv[1]);

    double sum {};
    {
        double grid {numeric_limits<double>::max()};
        Battery battery {13.5, 5.0};

        for (const auto& p : values) {
            double appliance; 
            double solarPower; 
            tie(appliance, solarPower) = p;

            sum += calculate_wattage(true, appliance, solarPower, battery, grid);
        }
    }

    cout << "Delimiter\n";

    double sum2 {};
    {
        double grid {numeric_limits<double>::max()};
        Battery battery {0.0, 0.0};

        for (const auto& p : values) {
            double appliance {p.first};

            sum2 += calculate_wattage(false, appliance, 0.0, battery, grid);
        }
    }

    cout << setprecision(4) << "Energy consumed from grid: " << sum << " kWh" << '\n';
    cout << setprecision(4) << "Energy consumed from grid without solar and battery: " << sum2 << " kWh" << '\n';

    return 0;
}
