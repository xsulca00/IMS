#include <iostream>
#include <chrono>
#include <tuple>
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

void calculate_wattage(double appliance, double solar, Battery& battery, double grid) {
    double sum {0.0};
    double consumed {};

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
        tie(consumed, grid) = drain(appliance-sum, grid);
        sum += consumed;
        cout << "Grid consumed:     " << consumed << '\n';
    }

    cout << "\nSolar:             " << solar << '\n'
         << "Battery:           " << battery.currentCapacity << '\n'
         << "Grid:              " << grid << '\n'
         << "Appliance:         " << appliance << "\n";
    cout << "========\n\n";
}

int main() {
    double appliance {0.1};
    double solarPower {3.4};
    double grid {numeric_limits<double>::max()};
    Battery battery {15.0, 5.0};

    for (seconds s {0}; s != 12600s; ++s)
        calculate_wattage(appliance, solarPower, battery, grid);

    cout << "\nCurCap: " << battery.CurrentCapacity() << '\n';

    appliance = 17;

    for (seconds s {0}; s != 8317s; ++s)
        calculate_wattage(appliance, solarPower, battery, grid);

    // cout << "Battery charged: " << setprecision(4) << b.currentCapacity / b.maxCapacity * 100 << '\n';

    return 0;
}
