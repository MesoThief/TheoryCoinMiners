#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

using namespace std;
using json = nlohmann::json;

/**
 * Converts TXT file from DNA sequence dataset to JSON file
 */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input-file-path> <output-file-path>\n";
        return 1;
    }

    filesystem::path input_path(argv[1]);
    string file_name = input_path.stem();

    ifstream input(argv[1]);
    string line;
    getline(input, line);

    json sequences = json::array();
    int id = 0;
    while (getline(input, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string sequence;
        int dna_class;
        if (iss >> sequence >> dna_class &&
            sequence.size() < 600 &&
            sequence.find('N') == string::npos
        ) {
            json obj = json::object();
            obj["id"] = ++id;
            obj["class"] = dna_class;
            obj["sequence"] = sequence;
            obj["sequence_length"] = sequence.size();
            sequences.push_back(obj);
        }
    }

    json data = json::object();
    data["animal"] = file_name;
    data["sequences"] = sequences;
    
    ofstream output(argv[2]);
    output << data.dump(4);

    input.close();
    output.close();

    return 0;
}