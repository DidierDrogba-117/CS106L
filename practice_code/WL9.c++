#include <iostream>
#include <vector>
#include <numeric>  // for std::inner_product

int dotProduct(const std::vector<int>& v1, const std::vector<int>& v2) {
    // Check if the vectors are of the same size
    if (v1.size() != v2.size()) {
        std::cerr << "Error: Vectors must be of the same length." << std::endl;
        return 0;  // Or you could throw an exception
    }

    // Use std::inner_product to compute the dot product
    return std::inner_product(v1.begin(), v1.end(), v2.begin(), 0);
}

int main() {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {4, 5, 6};

    int result = dotProduct(v1, v2);
    std::cout << "Dot Product: " << result << std::endl;

    return 0;
}
