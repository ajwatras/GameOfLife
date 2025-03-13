#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
using namespace std;

// Function to clear the console screen
void clearScreen() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    }

// Function to introduce a delay
void delay(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

class Universe {
    private:
    vector<vector<char>> arr;
    bool wasChanged;

    // Apply padding to a 2d array. 
    void padArray_(int pad_left, int pad_right, int pad_top, int pad_bot){
        int rows = arr.size();
        int cols = arr[0].size();
        vector<char> left_pad_vec(pad_left, '0');
        vector<char> right_pad_vec(pad_right, '0');
        for (int i = 0; i < arr.size(); i++){
            arr[i].insert(arr[i].begin(), left_pad_vec.begin(), left_pad_vec.end());
            arr[i].insert(arr[i].end(), right_pad_vec.begin(), right_pad_vec.end());
        }
        vector<char> row_pad_vec(cols+pad_left+pad_right, '0');
        for (int i = 0; i < pad_top; i++){
            arr.insert(arr.begin(), row_pad_vec);
        }
        for (int i = 0; i < pad_bot; i++){
            arr.push_back(row_pad_vec);
        }
    }

    // Encodes the array into a sparse representation.
    vector<tuple<int, int>> encode_(){
        vector<tuple<int, int>> encoded_arr;
        for (int i = 0; i < arr.size(); i++){
            for (int j = 0; j < arr[0].size(); j++){
                if (arr[i][j] == '1'){
                    encoded_arr.push_back(make_tuple(i, j));
                }
            }
        }
        return encoded_arr;
    }
    //Crops array to minimum and maximum nonzero elements
    public:
        Universe(vector<vector<char>> initial_state, size_t rows, size_t cols) : arr(initial_state) {
            int pad_left, pad_right, pad_top, pad_bot;
            if (rows - arr.size() <= 0) pad_top = pad_bot = 0;
            else if ((rows - arr.size()) % 2 == 0) pad_top = pad_bot = (rows - arr.size()) / 2;
            else {
                pad_top = (rows - arr.size()) / 2;
                pad_bot = pad_top + 1;
            }
            if (cols - arr[0].size() <= 0) pad_left = pad_right = 0;
            else if ((cols - arr[0].size()) % 2 == 0) pad_left = pad_right = (cols - arr.size()) / 2;
            else {
                pad_left = (cols - arr.size()) / 2;
                pad_right = pad_left + 1;
            }
            padArray_(pad_left, pad_right, pad_top, pad_bot);
            wasChanged = true;
            
        }
        // Move 1 timestep forward in the game of life.
        void progress(){
            int rows = arr.size();
            int cols = arr[0].size();
            vector<vector<int>> prefix(rows+1, vector<int>(cols+1,0));

            wasChanged = false;

            // Perform a prefix sum to efficiently get neighborhood values.
            for (int i = 1; i <= rows; i++){
                for (int j = 1; j <= cols; j++){
                    int curr_val = arr[i-1][j-1] - '0';
                    prefix[i][j] = curr_val + prefix[i-1][j] + prefix[i][j-1] - prefix[i-1][j-1]; 
                }
            }
            vector<vector<char>> out(rows, vector<char>(cols,'0'));
            // Compute the new state for each cell
            for (int i = 0; i < rows; i++){
                for (int j = 0; j < cols; j++){
                    char new_val; 
                    int curr_val = arr[i][j] - '0';
                    //create a bounding box for the neighborhood.
                    int r1 = max(0, i-1), r2 = min(rows - 1, i+1);
                    int c1 = max(0, j-1), c2 = min(cols - 1, j + 1);

                    // Using prefix sum, we can easily compute the neighborhood
                    int sum = prefix[r2 + 1][c2 + 1] - prefix[r1][c2 + 1] - prefix[r2 + 1][c1] + prefix[r1][c1];
                    // Remove center of the window to only count neighbors.
                    sum -= curr_val;

                    // compute update criteria
                    if (arr[i][j] == '0' && sum == 3) new_val = '1';
                    else if (arr[i][j] == '1' && (sum == 2 || sum == 3)) new_val ='1';
                    else new_val = '0';

                    if (new_val != arr[i][j]) {
                        arr[i][j] = new_val;
                        wasChanged = true;
                    }
                }
            }
        }
        // Function to display the 2d vector stored in the universe.
        void printArray(vector<vector<char>> arr) {
            for (const auto& row : arr) {
                for (const auto& element : row) {
                    //const char out_char = element;
                    const char* out_char = (element == '1') ? "@" : ".";
                    cout << out_char << " ";
                }
                cout << endl;
            }
        }
        //function to print out the current state of the universe. 
        void print(){
            printArray(arr);
        }
        //Function that loops through the game of life to display it as an animation
        void animate(){
            while ( wasChanged){
                clearScreen();
                print();
                progress();
                delay(200);
            }
        }

        // Save current universe state to sparse encoded text file.
        void save(string filename){
            ofstream outputFile(filename);
            if (!outputFile.is_open()){
                throw invalid_argument("Could not open " + filename);
            }
            outputFile << arr.size() << "," << arr[0].size() << endl;
            vector<tuple<int, int>> encoded_arr = encode_();
            for (int i = 0; i < encoded_arr.size(); i++){
                outputFile << get<0>(encoded_arr[i]) <<"," << get<1>(encoded_arr[i])<< endl;
            }
        }

        // Load the Universe from a sparse encoded text file.
        void load(string filename){
            ifstream in;
            string temp;
            int rows, cols;
            in.open(filename);
            if (!in.is_open()) {
                throw invalid_argument("Could not open "+ filename);
            }
            string line;
            if (getline(in, line)){
                stringstream ss(line);
                getline(ss, temp, ',');
                rows = stoi(temp);
                getline(ss, temp, ',');
                cols = stoi(temp);
                //initialize the arr
                arr.assign(rows, vector<char>(cols,'0'));
            }
            // Loop over nonzero points.
            while(getline(in, line)){
                stringstream ss(line);
                int x,y;
                getline(ss, temp, ',');
                x = stoi(temp);
                getline(ss, temp, ',');
                y = stoi(temp);
                
                //make sure point is valid.
                if (x >= 0 && x < rows && y >= 0 && y < cols){
                    arr[x][y] = '1';
                }

            }

        }
};

// Loads a csv file into a 2d array.
int load_organism(string filename, vector<vector<char>>* out) {
    ifstream in;
    in.open(filename);
    if (!in.is_open()) {
    throw invalid_argument("Could not open "+ filename);
    }

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;
        vector<char> fields;
        while (getline(ss, field, ',')) {
            fields.push_back(field[0]);
        }
        out->push_back(fields);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    string filename;
    vector<vector<char>> arr;
    int rows, cols;
    // Input handling
    if (argc == 1){
        rows = cols = 50;
        filename = "glider.csv";
    }
    if (argc == 2){
        filename = argv[1];
        rows = cols = 50;
    }
    if (argc == 3){
        filename = argv[1];
        rows = cols = stoi(argv[2]);
    }
    if (argc == 4){
        filename = argv[1];
        rows = stoi(argv[2]);
        cols = stoi(argv[3]);
    }

    // Load from csv file
    try{
        load_organism(filename, &arr);
    } catch (const invalid_argument& e) {
        cerr << "Error: " << e.what() << endl;
    }
    //initialize the game of life
    Universe uni(arr, rows, cols);
    uni.save("glider2.csv");
    // Run the game of life
    uni.animate();
    cout << "Loading initial state..." << endl;
    uni.load("glider2.csv");
    cout << "Initial State:" << endl;
    uni.print();

    
}