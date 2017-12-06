#include <random>
#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include <functional>

using namespace std;
using namespace std::chrono;

using TimePoint = time_point<system_clock, duration<double,ratio<60>>>;

bool IsValidHours(int h) {
    return 0 <= h && h < 24;
}

bool IsValidMinutes(int m) {
    return 0 <= m && m < 60;
}

double MakeHours(const string& s) {
    istringstream ss {s};
    int hours {};
    char colon {};
    int minutes {};

    ss >> hours >> colon >> minutes;

    if (!IsValidHours(hours) || colon != ':'  || !IsValidMinutes(minutes)) {
        throw runtime_error{"Invalid time format: " + s};
    }

    return TimePoint{seconds{hours*60 + minutes}}.time_since_epoch().count();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " mean_kWh interval_start interval_end\n";
        return 1;
    }

    double mean {stod(argv[1])};
    double begin {MakeHours(argv[2])};
    double end {MakeHours(argv[3])};

    cout << "Mean: " << mean << '\n';
    cout << "Begin: " << begin << '\n';
    cout << "End: " << end << '\n';

    if (begin >= end) {
        cerr << "begin >= end!\n";
        return 1;
    }

    double span {end - begin};

    // 15 min intervals
    int buckets {static_cast<int>(span/0.25)};
    double bucket {0.25};

    // double size because buckets is only half of X in normal distribution
    int max {buckets};

    vector<int> histogram(max);

    cout << "Span: " << span << '\n';
    cout << "Max: " << max << '\n';
    cout << "Bucket: " << bucket << '\n';

    auto gen = bind(normal_distribution<double>{(begin+end)/2, 2},
                   mt19937{static_cast<unsigned>(system_clock::now().time_since_epoch().count())});

    constexpr int rolls {10'000'000};

    for (int i {0}; i != rolls; ++i) {
        double n {gen()};

        double b {begin};
        for (int& d : histogram) {
            if (b <= n &&  n < b+bucket) {
                ++d;
                break;
            }
            b += bucket;
        }
    }

    int sum {accumulate(histogram.begin(), histogram.end(), 0)};

    double scale {mean/sum};

    double area {};
    for (int d : histogram) {
        double kWh {scale*d};
        string s;
        for (int i{}; i != (int)(kWh*50.0); ++i)
            s += '*';
        cout << s << '\n';
        area += kWh;
    }

    cout << "Area: " << area << '\n';

    return 0;
}
