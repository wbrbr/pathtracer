#ifndef PLOT_HPP
#define PLOT_HPP
#include <fstream>
#include <functional>
#include <string>

void plot(std::string outpath, std::function<float(float)> f, float beg, float end, float step)
{
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
