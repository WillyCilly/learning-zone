#include <iostream>
#include <map>
#include <list>

using namespace std;

class LRUCache {

private:
    int capacity;
    list<int> usage_list;
    map<int, int> cache_map;

public:
    LRUCache(int capacity) {
        this->capacity = capacity;
    }

    int get(int key) {
        auto it = cache_map.find(key);
        if(it != cache_map.end()){
            // find the key in cache_map
            for(auto iter = usage_list.begin(); iter != usage_list.end(); iter++){
                // in usage_list, move the key to the back
                if((*iter) == key){
                    usage_list.erase(iter);
                    break;
                }
            }
            usage_list.push_back(key);
            return it->second;
        }else{
            // key doesn't exist in cache_map
            return -1;
        }
    }

    void put(int key, int value) {
        int key_present = this->get(key);
        if(key_present != -1){
            // set value
            cache_map[key] = value;
        }else{
            // insert value
            if(usage_list.size() >= this->capacity){
                int key_erased = usage_list.front();
                usage_list.pop_front();
                cache_map.erase(key_erased);
            }
            cache_map.insert(pair<int,int>(key, value));
            usage_list.push_back(key);
        }
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */

int main(){
  int capacity = 2;
  LRUCache* obj = new LRUCache(capacity);
  obj->put(1,1);
  obj->put(2,2);
  cout << obj->get(1) << endl;
  obj->put(3,3);
  cout << obj->get(2) << endl;
  obj->put(4,4);
  cout << obj->get(1) << endl;
  cout << obj->get(3) << endl;
  cout << obj->get(4) << endl;
  return 0;
}
