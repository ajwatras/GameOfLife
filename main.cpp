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
    bool useFixedWorldSize;

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
    vector<vector<char>> crop2DArray_(const vector<vector<char>>& array, int startRow, int endRow, int startCol, int endCol){
        vector<vector<char>> croppedArray;

        if (startRow < 0 || endRow + 1 > array.size() || startCol < 0|| endCol + 1 > array[0].size()){
            /*
            string message = "Invalid cropping indices: " + startRow + ", " + endRow
                             + ', ' + startCol + ', ' + endCol
                             + 'is outside array of size: ' + array.size() + ", " + array[0].size(); 
            throw invalid_argument(message);
            */
            throw invalid_argument("Invalid cropping indices.");
            return croppedArray;
        }
        for (int i = startRow; i <= endRow; i++){
            vector<char> row;
            for (int j = startCol; j <= endCol; j++){
                row.push_back(array[i][j]);
            }
            croppedArray.push_back(row);
        }
        return croppedArray;
    }
    public:

        // Initialize from file
        Universe(string filename){
            load(filename);
            wasChanged = true;
            useFixedWorldSize = false;
        }
        // Initialize from 2D vector with padding to desired size.
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
            useFixedWorldSize = true;
            
        }
        // Move 1 timestep forward in the game of life.
        void progress(){
            //pad array with zeros on all sides if we are tracking.
            if (!useFixedWorldSize) padArray_(1,1,1,1);

            int rows = arr.size();
            int cols = arr[0].size();
            vector<vector<int>> prefix(rows+1, vector<int>(cols+1,0));
            int min_x = cols - 1;
            int min_y = rows - 1;
            int max_x = 0;
            int max_y = 0;

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
                    
                    // Update bounding box.
                    if (!useFixedWorldSize) {
                        if (new_val == '1'){
                            min_y = min(min_y, i);
                            max_y = max(max_y, i);
                            min_x = min(min_x, j);
                            max_x = max(max_x, j);
                        }
                    }
                    if (new_val != arr[i][j]) {
                        arr[i][j] = new_val;
                        wasChanged = true;
                    }
                }
            }
            // crop down to bounding box
            if (!useFixedWorldSize) {
                arr = crop2DArray_(arr, min_y, max_y, min_x, max_x);
            }
        }
        // Function to display the 2d vector stored in the universe.
        void printArray(vector<vector<char>> arr) {
            for (const auto& row : arr) {
                for (const auto& element : row) {
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
                return;
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
        filename = "glider.csv";
        Universe uni(filename);
        // Run the game of life
        uni.animate();
    }
    if (argc == 2){
        filename = argv[1];
        Universe uni(filename);
        // Run the game of life
        uni.animate();
    }
    if (argc == 3){
        filename = argv[1];
        rows = cols = atoi(argv[2]);


        load_organism(filename, &arr);

        //initialize the game of life
        Universe uni(arr, rows, cols);
        uni.save("input_state.csv");
        uni.animate();

        // If game completes, load initial state and display it
        cout << "Loading initial state..." << endl;
        uni.load("input_state.csv");
        cout << "Initial State:" << endl;
        uni.print();
    }
    if (argc == 4){
        filename = argv[1];
        rows = atoi(argv[2]);
        cols = atoi(argv[3]);


        load_organism(filename, &arr);

        //initialize the game of life
        Universe uni(arr, rows, cols);
        uni.save("input_state.csv");
        uni.animate();

        // If game completes, load initial state and display it
        cout << "Loading initial state..." << endl;
        uni.load(filename);
        cout << "Initial State:" << endl;
        uni.print();
    }

}