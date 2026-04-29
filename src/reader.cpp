#include "config.hpp"

vector<vector<string>> read_csv(const string filepath, char sep) {
    vector<vector<string>> data;
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filepath << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, sep)) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}
