#include <sstream>
#include <iostream>
int main(){
    std::stringstream ss;
    ss << "hello ";
    ss << "11";
    ss << "\n";
    std::cout << ss.str();
}