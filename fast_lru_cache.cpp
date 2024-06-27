#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class LRUCache {
private:
    const std::pair<int, int> TOMB_STONE = {10001, 0};
    int capacity;
    int array_size;
    std::unordered_map<int, int> cache;
    std::vector<std::pair<int, int>> order;
    int MRU;
    int LRU;

    void compact_order_array() {
        auto order_itr = std::remove_if(order.begin(), order.end(), [&](const std::pair<int, int>& x) {
            return x == TOMB_STONE;
        });
        order.resize(std::distance(order.begin(), order_itr), TOMB_STONE);
        std::vector<std::pair<int, int>> new_order_array(array_size, TOMB_STONE); // Initialize new order array with tombstone values
        int start_idx = capacity*3;
        int idx = start_idx; // Start index for copying non-tombstone elements
        LRU = array_size - 1; // Update LRU index
        MRU = start_idx-1; // Update MRU index
        for (const auto& node : order) {
            if (node != TOMB_STONE) {
                cache[node.first] = idx; // Update cache indices for non-tombstone elements
            }
            new_order_array[idx] = node;
            ++idx;
        }
        order = std::move(new_order_array); // Update order array
    }
    
    // -------------------IN-PLACE---------------------
    // void compact_order_array() {    
    //     std::stable_partition(order.begin(), order.end(), [&](const std::pair<int, int>& x) {
    //         return x == TOMB_STONE;
    //     });
    //     int start_idx = capacity*3;
    //     int idx = start_idx; // Start index for copying non-tombstone elements
    //     LRU = array_size - 1; // Update LRU index
    //     MRU = start_idx-1; // Update MRU index
    //     for (idx;idx<array_size;++idx) {
    //         int key = order[idx].first;
    //         if (key != TOMB_STONE.first)
    //             cache[key] = idx;
    //     }
    // }

    int update_recent(std::pair<int, int> node) {
        if (MRU == -1) {
            compact_order_array();
        }
        order[MRU] = node;
        --MRU;
        while (order[LRU] == TOMB_STONE) {
            --LRU;
        }
        return MRU + 1;
    }
public:
    LRUCache(int capacity) : capacity(capacity), array_size(capacity * 4), order(array_size, TOMB_STONE), MRU(array_size - 1), LRU(array_size - 1) {}

    int get(int key) {
        if (cache.find(key) != cache.end()) {
            int value = order[cache[key]].second;
            order[cache[key]] = TOMB_STONE;
            int idx = update_recent({key, value});
            cache[key] = idx;
            return value;
        }
        return -1;
    }

    void put(int key, int value) {
        if (MRU == -1) {
            compact_order_array();
        }
        if (cache.find(key) != cache.end()) {
            get(key);
            ++MRU;
        }
        order[MRU] = {key, value};
        cache[key] = MRU;
        --MRU;
        if (cache.size() > capacity) {
            cache.erase(order[LRU].first);
            order[LRU] = TOMB_STONE;
            while (order[LRU] == TOMB_STONE) {
                --LRU;
            }
        }
    }
};

// LL code

// class Node {
// public:
//     int k;
//     int val;
//     Node* prev;
//     Node* next;

//     Node(int key, int value) {
//         k = key;
//         val = value;
//         prev = NULL;
//         next = NULL;
//     }
// };

// class LRUCache {
// public:
//     LRUCache(int capacity) {
//         cap = capacity;

//         left = new Node(0, 0);
//         right = new Node(0, 0);

//         left->next = right;
//         right->prev = left;
//     }

//     int get(int key) {
//         if (cache.find(key) != cache.end()) {
//             remove(cache[key]);
//             insert(cache[key]);
//             return cache[key]->val;
//         }
//         return -1;
//     }

//     void put(int key, int value) {
//         if (cache.find(key) != cache.end()) {
//             remove(cache[key]);

//             // Free allocated memory for the removed node
//             delete cache[key];
//         }
//         cache[key] = new Node(key, value);
//         insert(cache[key]);

//         if (cache.size() > cap) {
//             // remove from list & delete LRU from map
//             Node* lru = left->next;
//             remove(lru);
//             cache.erase(lru->k);

//             // Free allocated memory for the removed node
//             delete lru;
//         }
//     }
// private:
//     int cap;
//     unordered_map<int, Node*> cache; // {key -> node}
//     Node* left;
//     Node* right;

//     // remove node from list
//     void remove(Node* node) {
//         Node* prev = node->prev;
//         Node* next = node->next;

//         prev->next = next;
//         next->prev = prev;
//     }

//     // insert node at right
//     void insert(Node* node) {
//         Node* prev = right->prev;
//         Node* next = right;

//         prev->next = node;
//         next->prev = node;

//         node->prev = prev;
//         node->next = next;
//     }
// };


int main() {
    
    ifstream file("five_million.txt");
    // ofstream output_file("output.txt");
    string line;
    vector<vector<int>> test_input;
    LRUCache cache(10000);

    // Read from file and store values in test_input
    while (getline(file, line)) {
        istringstream iss(line.substr(1, line.length() - 2));
        vector<int> values;
        string value;
        while (getline(iss, value, ',')) {
            values.push_back(stoi(value.size() > 0 ? value : "0"));
        }
        test_input.push_back(values);
    }

    auto start = chrono::steady_clock::now();
    int res = 0;
    for (int i = 0; i < test_input.size(); i++) {
        if (test_input[i].size() == 1) {
            // int res = cache.get(test_input[i][0]);
            if (cache.get(test_input[i][0]) != -1){
                // output_file << res << endl;
                ++res;

            }
        } else {
            cache.put(test_input[i][0], test_input[i][1]);
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    cout << "Duration: " << duration << " ns" << endl;
    cout << "Result: " << res << endl;
    file.close();
    // output_file.close();

    return 0;
}

// Benchmark :

// 5M ops max_key 10k
// Array -
//     Duration: 169605700 ns / 169.6 ms
//     Result: 2489376
// LL -
//     Duration: 360665000 ns / 360.7 ms
//     Result: 2489376

// 500k ops max_key 100
// Array -
//     Duration: 12562800 ns / 12.6 ms
//     Result: 249926
// LL -
//     Duration: 23592500 ns / 23.6 ms
//     Result: 249926