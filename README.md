# pbl-2nd-year-techno-pathsdr
GEHU PBL Academic Project - Team TECHNO PATHS


Drastic Vantage

Disaster Relief Supply Distribution Management System

A menu-driven disaster relief management application developed using C, C++, Data Structures and Object-Oriented Programming.

Project Overview

Drastic Vantage is designed to help coordinate the distribution of essential supplies to relief camps during natural disasters and emergency situations.

The system manages supply inventory, registers relief camps, handles emergency requests based on priority, calculates shortest delivery routes and maintains dispatch records.

Project Objectives

- Manage relief supply inventory efficiently.
- Register and maintain disaster relief camp information.
- Prioritize emergency requests based on urgency.
- Find efficient delivery routes using Dijkstra's algorithm.
- Track active and completed deliveries.
- Demonstrate the practical integration of Data Structures and Object-Oriented Programming.

Technologies Used

- Languages: C and C++
- IDE: Visual Studio Code
- Compiler: GCC and G++ (MinGW)
- Programming Concepts: DSA, OOP and File Handling

Data Structures and Algorithms

Data Structure / Algorithm| Application
Singly Linked List| Supply inventory management
Binary Search Tree| Relief camp registration and searching
Graph| Road network representation
Dijkstra's Algorithm| Shortest delivery route calculation
Priority Queue (Heap)| Emergency request prioritization
Circular Queue| Active dispatch management
Stack| Completed delivery history
Vector| Requests waiting for resources

Object-Oriented Programming Concepts

Encapsulation: Private data members with controlled access through methods.

Inheritance: Supply and Camp classes inherit from the ReliefEntity base class.

Polymorphism: Virtual functions and overriding allow different entities to display their information.

Abstraction: ReliefEntity is an abstract class containing a pure virtual function.

Operator Overloading: The comparison operator is overloaded to prioritize relief requests.

Constructors and Destructors: Used for object initialization and resource management.

Main Features

1. Add, search, update and remove relief supplies.
2. Register and search relief camps.
3. Create and manage road networks.
4. Calculate shortest routes using Dijkstra's algorithm.
5. Create emergency supply requests with priority levels.
6. Process requests based on urgency.
7. Maintain dispatch queues and delivery history.
8. Generate system reports.
9. Save and load application data.

Project Files

File| Purpose
inventory.h / inventory.c| Linked list inventory
graph.h / graph.c| Graph and shortest path
structures.h / structures.c| BST, circular queue and stack
models.hpp| C++ OOP classes
system.hpp| Main system declarations
system.cpp| Application logic and menu
storage.cpp| File handling
main.cpp| Program entry point

How to Compile and Run

Requirements: GCC, G++ and a terminal with access to both compilers.

Compile the C source files:

"gcc -std=c11 -c inventory.c -o inventory.o"

"gcc -std=c11 -c graph.c -o graph.o"

"gcc -std=c11 -c structures.c -o structures.o"

Compile the C++ source files:

"g++ -std=c++17 -c system.cpp -o system.o"

"g++ -std=c++17 -c storage.cpp -o storage.o"

"g++ -std=c++17 -c main.cpp -o main.o"

Link all object files:

"g++ inventory.o graph.o structures.o system.o storage.o main.o -o DrasticVantage.exe"

Run the executable on Windows:

".\DrasticVantage.exe"

Team Members

1. Uttkarsh Bhatt
2. Diljot
3. Vaibhav Chauhan
4. Yagya Chauhan

Academic Details

Project: Drastic Vantage

Programme: B.Tech Computer Science and Engineering

Semester: III

Institution: Graphic Era Hill University, Dehradun

Evaluation: Project-Based Learning — Phase III (2026)

Future Scope

- Develop a graphical user interface.
- Integrate GPS-based route tracking.
- Add real-time disaster alerts.
- Integrate a centralized database.
- Develop a web-based dashboard for relief organizations.

---

Drastic Vantage — Making Disaster Relief Distribution More Organized and Efficient.
