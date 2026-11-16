#include <iostream>
#include <exception>
#include "system.hpp"

using namespace std;

int main() {
    try {
        DisasterSystem system;

        cout << "========================================\n";
        cout << "          DRASTIC VANTAGE\n";
        cout << " Disaster Relief Supply Distribution\n";
        cout << "========================================\n";

        cout << "\nWelcome to Drastic Vantage!\n";
        cout << "Disaster Relief Management System\n";

        system.run();

        cout << "\nThank you for using Drastic Vantage.\n";

    }
    catch (const exception& e) {
        cerr << "\nFatal Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}