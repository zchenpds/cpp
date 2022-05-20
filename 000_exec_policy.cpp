#include <execution>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>

using namespace std;
using namespace std::chrono;

auto sum1(const string& s){
    int sum{};
    for (char c: s) {
        sum += c - '0';
    }
    cout << "sum = " << sum << "\n";
    cout << "For-loop: ";
};

auto sum2(const string& s){
    int sum = transform_reduce(s.begin(), s.end(), 0, plus<int>(), [](char c)->int{return c - '0';});
    cout << "sum = " << sum << "\n";
    cout << "transform_reduce(...): ";
};

auto sum3(const string& s){
    int sum = transform_reduce(execution::seq, s.begin(), s.end(), 0, plus<int>(), [](char c)->int{return c - '0';});
    cout << "sum = " << sum << "\n";
    cout << "transform_reduce(execution::seq, ...): ";
};

auto sum4(const string& s){
    int sum = transform_reduce(execution::par, s.begin(), s.end(), 0, plus<int>(), [](char c)->int{return c - '0';});
    cout << "sum = " << sum << "\n";
    cout << "transform_reduce(execution::par, ...): ";
};

auto sum5(const string& s){
    int sum = transform_reduce(execution::par_unseq, s.begin(), s.end(), 0, plus<int>(), [](char c)->int{return c - '0';});
    cout << "sum = " << sum << "\n";
    cout << "transform_reduce(execution::par_unseq, ...): ";
};

auto sum6(const string& s){
    int sum = transform_reduce(execution::unseq, s.begin(), s.end(), 0, plus<int>(), [](char c)->int{return c - '0';});
    cout << "sum = " << sum << "\n";
    cout << "transform_reduce(execution::unseq, ...): ";
};

int main() {
    string s(1e6, '7');

    // Invented template parameter for operator()'s argument
    auto time = [&](auto f)
    {
        auto t0 = system_clock::now();
        f(s);
        auto t1 = system_clock::now();
        cout << duration_cast<microseconds>(t1 - t0).count() << " ms\n";
    };

    time(sum1);
    time(sum2);
    time(sum3);
    time(sum4);
    time(sum5);
    time(sum6);
    time_point t = system_clock::now();
    std::cout << t.time_since_epoch().count() << std::endl;
    return 0;
}