#ifndef PLOT_HPP
#define PLOT_HPP
#include <string>
#include <functional>
#include <fstream>

void plot(std::string outpath, std::function<float(float)> f, float beg, float end, float step) {
    std::string out = "";
    float t = beg;
    while (t <= end) {
        out += std::to_string(t);
        out += " ";
        out += std::to_string(f(t));
        out += "\n";
        t += step;
    }
    std::ofstream outfile(outpath);
    outfile << out;
    outfile.close();
}
#endif
