#include "util.hpp"
#include <iostream>

int main()
{
    // let n be a unit vector
    // the expected value of dot(wi, n) with wi uniformly distributed over the hemisphere around n is equal to 1
    // let's test our sampling with this !
    
    glm::vec3 n = random_unit_sphere();
    float s = 0;
    const int N = 1000;
    for (int i = 0; i < N; i++)
    {
        glm::vec3 wi = random_unit_hemisphere(n);
        s += glm::dot(wi, n);
    }

    std::cout << s / (float)N << std::endl;
    return 0;
}
