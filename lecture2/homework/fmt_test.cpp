#include <fmt/core.h> 

int main() {
    
    fmt::print("Hello, {}!\n", "world");

    std::string message = fmt::format("The answer is {}.\n", true);
    fmt::print("{}", message); 

    std::string name = "GYF";
    int age = 20;
    double score = 100;

    fmt::print("{} is {} years old and scored {:.1f} points.\n", name, age, score);
    
    system("pause");
    return 0;
}