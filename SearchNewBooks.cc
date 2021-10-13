// Copyright 2021 Andy Lee
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cctype>
#include <algorithm>

class Clock{ // clock class to measure time
    std::chrono::high_resolution_clock::time_point start;
 public:
    void Reset() { // starts time
        start = std::chrono::high_resolution_clock::now();
    }
    void currentTime() { // ends time and print end-start time
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed_us = std::chrono::duration<double,
                std::micro>(end-start).count();
        std::cout << "CPU time: " << elapsed_us << " microseconds" << std::endl;
    }
};

class Book{ // Book class to store ISNB, language, type for each book
    int ISNB;
    std::string language;
    std::string type;
 public:
    explicit Book(int inputISNB, std::string inputLang, std::string inputType) {
        ISNB = inputISNB;
        language = inputLang;
        type = inputType;
    }
    int getISNB() { // return book ISNB
        return ISNB;
    }
    std::string getLanguage() { // return book language
        return language;
    }
    std::string getType() { // return book type
        return type;
    }
};

std::vector<Book> readFile(const std::string& path) { // function that read file and return a vector of books
    std::ifstream inputFile;
    inputFile.open(path);
    std::string line;
    std::vector<Book> result;
    if (inputFile.is_open()) {
        while (inputFile.good()) {
            int inputISNB;
            std::vector<std::string> info;
            std::string inputLang;
            std::string inputType;
            getline(inputFile, line);
            std::stringstream s_stream(line);
            while (s_stream.good()) {
                std::string item;
                getline(s_stream, item, ',');
                info.push_back(item);
            }
            if (info.size() == 3) {
                std::stringstream temp(info[0]);
                temp >> inputISNB;
                inputLang = info[1];
                inputType = info[2];
                result.push_back(Book(inputISNB, inputLang, inputType));
            }
        }
        inputFile.close();
    } else { // error not being able to open file
        std::cerr << "Error: cannot open file " << path << std::endl;
        exit(1);
    }
    return result;
}

bool compareBook(Book a, Book b) { //return true if two books are same
    if (a.getISNB() != b.getISNB()) { // compare ISNB
        return false; // return false if not same
    }
    if (a.getType() != b.getType()) { // compare type
        return false; // return false if not same
    }
    if (a.getLanguage() != b.getLanguage()) { // compare language
        return false; // return false if not same
    }
    return true;
}

bool operator<(Book a, Book b) { // operator overloading to compare two books
    if (a.getISNB() == b.getISNB()) { // if ISNB is same compare type
        if (a.getType() == b.getType()) { // if type is same compare language
            return a.getLanguage() > b.getLanguage(); // return in reverse alphabetical order
        } else { // if type is different return in order of new, used, digital
            if (a.getType() == "new") { // if a is new then a has higher precedent
                return true;
            } else if (b.getType() == "new") { // if b is new then b has higher precedent
                return false;
            } else if (a.getType() == "used") { // if a,b are not new and a is used a has higher precedent
                return true;
            }
            return false; // if a,b is not new and a is not used b has higher precedent
        }
    }
    return a.getISNB() > b.getISNB(); //return in order of ISNB from largest to smallest
}

int linearSearch(const std::vector<Book>& bookList,
        const std::vector<Book>& requestList) { // linear search
    int count = 0;
    for (unsigned int i = 0; i < requestList.size(); i++) { // iterate each request book list
        for (unsigned int j = 0; j < bookList.size(); j++) { // find request book from book list
            if (compareBook(bookList[j], requestList[i])) { // if two books are same
                ++count; // if two books are same increase count by one
                break; // break for loop to not count duplicates
            }
        }
    }
    return count;
}

int binarySearch(const std::vector<Book>& bookList,
        const std::vector<Book>& requestList) { // binary search
    int count = 0;
    for (unsigned int i = 0; i < requestList.size(); i++) { // iterate each request book list
        int left = 0; // left most index
        int right = bookList.size()-1; // right most index
        while (left < right) { // stops when there is one book left to read
            int m = (left+right)/2; // find the middle index of sorted list
            if (bookList[m] < requestList[i]) { // compare two books
                left = m + 1; // search right half list
            } else {
                right = m; // search left half list
            }
        }
        if (compareBook(requestList[i], bookList[left])) {
            ++count; // if two books are same increase count by one
        }
    }
    return count;
}

int main(int argc, char**argv) {
    if (argc != 4) { //if 3 command line argument not given, return error
        std::cerr << "Usage: .SearchNewBooks <newBooks.dat> <request.dat> "
                     "<result_file.dat>" << std::endl;
        exit(1);
    }
    std::string newbookpath = argv[1];
    std::string requestpath = argv[2];
    std::string resultFile_name = argv[3];
    std::vector<Book> newBook = readFile(newbookpath); // sort file into vector of books
    std::vector<Book> requestBook = readFile(requestpath); // sort file into vector of books
    Clock ct;
    char input;
    int count = 0;
    std::cout << "Choice of search method ([l]inear, [b]inary)?" << std::endl; // ask user choice of search
    do {
        std::cin >> input; // get input from user
        ct.Reset(); // start clock
        if (input == 'l') { // run linear search
            count = linearSearch(newBook, requestBook);
            break;
        } else if (input == 'b') { // run binary search
            std::sort(newBook.begin(), newBook.end()); // sort list before running binary search
            count = binarySearch(newBook, requestBook);
            break;
        } else {
            std::cerr << "Incorrect choice" << std::endl; // print error message if input is wrong
        }
    }while(input != 'l' && input != 'b'); // repeat until correct input is given
    ct.currentTime(); // stop clock and print time taken
    std::ofstream resultFile(resultFile_name); // create file for output
    resultFile << count; // write the result in the file
    resultFile.close(); // close file
    return 0;
}
