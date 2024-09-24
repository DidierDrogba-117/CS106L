#include "hashmap.h"

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : _size(0), _hash_function(H()), _buckets_array(kDefaultBuckets, nullptr) {};

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash):
    _size(0), 
    _hash_function(hash), 
    _buckets_array(bucket_count, nullptr) {};

template<typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    clear();
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const{
    return _size;
}

template<typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const{
    return _size == 0;
}

template<typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const{
    return ((float) _size) / _buckets_array.size();
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const {
    return _buckets_array.size();
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const {
    auto [pre_node, cur_node] = find_node(key);
    return cur_node != nullptr;
}

template<typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    /*
    1. use find_node to find the node with the key
    2. if the node is found, return the mapped value
    3. if not found, throw std::out_of_range exception
    */
    auto [pre_node, cur_node] = find_node(key);
    if (cur_node == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    auto& [cur_key, cur_val] = cur_node->value;
    return cur_val;
}

template<typename K, typename M, typename H>
const M& HashMap<K, M, H>::at(const K& key) const {
    return const_cast<HashMap<K, M, H> *>(this)->at(key);
}


template<typename K, typename M, typename H>
void HashMap<K, M, H>::clear() {
    for (auto& bucket : _buckets_array) {
        // bucket is the head of linkedlist, traver linkedlist and delete each node
        while (bucket != nullptr) {
            auto tmp_bkt = bucket->next;
            delete bucket;
            bucket = tmp_bkt;
        }
    }
    _size = 0; 
}

template<typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::iterator, bool> HashMap<K, M, H>::insert(const value_type& kv_pair) {
    /*
    1. Find the bucket index using the hash function
    2. if find node with the key, return {iterator to the newnode, false}
    3. if not found, create a new node and insert it to the tail of the linked list in the bucket (pre_node is the last node in the linked list), which is pre_node ->next = new_node
    4. Return {iterator to the new node, true}
    */
    auto [pre_node, cur_node] = find_node(kv_pair.first); 
    if (cur_node != nullptr) return {make_iterator(cur_node), false};
    size_t bucket_index = _hash_function(kv_pair.first) % _buckets_array.size();
    Node* new_node = new Node(kv_pair, nullptr);

    if (pre_node != nullptr) {pre_node->next = new_node;}
    else {_buckets_array[bucket_index] = new_node;}
    _size++;
    return {make_iterator(new_node), true};
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    /*
    1. get bucket index based on key's hash val 
    2. find the node with the key
    3. if the node is not found, return false
    4. if the node is found, remove the node from the linked list
    */

   size_t bucket_index = _hash_function(key) % _buckets_array.size();
   auto [pre_node, cur_node] = find_node(key);
   if (cur_node == nullptr) {return false;}

   Node* next_node = cur_node->next; 
   delete cur_node;
   // if the node is the head of the linked list
   if (pre_node == nullptr) {
        _buckets_array[bucket_index] = next_node;
   } else {
        pre_node->next = next_node;
   }
   _size--;
   return true;

}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::erase(const_iterator pos) {
    /*
    1. save node->next 2. rm node 
    */
   iterator tmp_node = make_iterator(pos._node);
   tmp_node++;
   if (pos._node != nullptr) {erase(pos._node->value.first);}
   return tmp_node;
}

template<typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_buckets) {
    /*
    1. Resize the Bucket Array
    2. Redistribute the Nodes
    */
    if (new_buckets == 0) 
    {throw std::out_of_range("HashMap<K, M, H>::rehash: new_buckets cannot be 0");}
    // shallow copy the old bucket array (copy the pointers stored in old array)
    bucket_array_type tmp_bkt_array = _buckets_array;
    _buckets_array.clear();
    _buckets_array.resize(new_buckets, nullptr);

    // traverse the old bucket array and redistribute the nodes
    for (auto& old_bucket_head : tmp_bkt_array) {
        while (old_bucket_head  != nullptr) {
            /*
            1. store the next node
            2. get the new bucket index
            3. insert the current node to the new bucket as new head 
            */
           Node* curr_node = old_bucket_head;
           old_bucket_head = old_bucket_head->next;
           // extract key-val from curr 
           const auto& [key, mapped] = curr_node->value;
           size_t new_bucket_index = _hash_function(key) % new_buckets;

           // insert the node to the new bucket as the head 
           curr_node->next = _buckets_array[new_bucket_index];
           _buckets_array[new_bucket_index] = curr_node;
        }
    }


}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::begin() {
    size_t index = first_not_empty_bucket();
    return make_iterator(_buckets_array[index]);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::begin() const 
{
    // this is a ptr points to current obj, which means this has the type of HashMap<K, M, H> * 
    return const_cast<HashMap<K, M, H> *>(this)->begin();
}


template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::end() {
    return make_iterator(nullptr);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::end() const {
    return const_cast<HashMap<K, M, H> *>(this)->end();
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::find(const K& key) {
    auto [prev_node, curr_node] = find_node(key);
    return make_iterator(curr_node);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::find(const K& key) const {
    return const_cast<HashMap<K, M, H> *>(this)->find(key);
}


template<typename K, typename M, typename H>
void HashMap<K, M, H>::debug() {
    std::cout << "HashMap Debug Info: " << std::endl;
    std::cout << "Number of Buckets = " << bucket_count()
              << " | Number of Elements = " << size()
              << " | Load Factor = " << load_factor() << std::endl;
    // Loop through each bucket in the bucket array
    for (size_t bucket_index = 0; bucket_index < _buckets_array.size(); bucket_index++) 
    {
        std::cout << "Bucket-" << bucket_index << ": ";
        Node* curr_node = _buckets_array[bucket_index];
        while (curr_node != nullptr) {
            std::cout << curr_node->value.first << "-" << curr_node->value.second << " ";
            curr_node = curr_node->next;
        }
        std::cout << std::endl;  // Move to the next line for the next bucket
    }
}


template<typename K, typename M, typename H>
template<typename InputIter>
HashMap<K, M, H>::HashMap(InputIter begin, InputIter end, size_t bucket_count, const H& hash):HashMap(bucket_count, hash) //  delegating constructor 
{
    for (InputIter it = begin; it != end; it++) {
        insert(*it);
    }
}

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H& hash):
    HashMap(init.begin(), init.end(), bucket_count, hash){}

template<typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    // value is default-initialized, meaning it is set to its "empty" or default state. like 0 or empty string or empty instance 
    value_type default_kv{key, {}};
    auto [iter, flag] = insert(default_kv);
    return iter->second; // return val 
}

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H>& map): 
    _size(0),
    _hash_function(map._hash_function),
    _buckets_array(map._buckets_array.size(), nullptr)
{   
    for (const auto& kv_pair : map) {
        insert(kv_pair);
    }
}


// move constructor
// HashMap<std::string, int> map2 = std::move(map1);

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H>&& map):
    _size(std::move(map._size)),
    _hash_function(std::move(map._hash_function)),
    _buckets_array(std::move(map._buckets_array)) 
{
    map._buckets_array.resize(_buckets_array.size(), nullptr);
    map._size = 0;
}



//  copy assignment operator
//  map2 = map1;  // Calls the copy assignment operator
template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(const HashMap<K, M, H>& map) {
    if (this == &map) {return *this;}
    this->clear();
    this->_hash_function = map._hash_function;
    for (const auto& kv_pair : map) {
        this->insert(kv_pair);
    }
    return *this;
}

// move assignment operator 
template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(HashMap<K, M, H>&& map) {
    // self-assignment check 
    if (this == &map) {return *this;}
    this->clear();

    this->_size = std::move(map._size);
    this->_hash_function = map._hash_function;
    this->_buckets_array = std::move(map._buckets_array);

    //reset the map
    map._size = 0;
    map._buckets_array.resize(this->_buckets_array.size(), nullptr);
    
    return *this;
}








template<typename K, typename M, typename H>
size_t HashMap<K, M, H>::first_not_empty_bucket() const {
    for (size_t i = 0; i < _buckets_array.size(); i++) {
        if (_buckets_array[i] != nullptr) {return i;}
    }
    return _buckets_array.size() - 1;
}


template<typename K, typename M, typename H> 
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const 
{
    /*
    1. Find the bucket index using the hash function
    2. Traverse the linked list in the bucket to find the node with the key
    3. Return the previous and current node
    4. If the key is not found, return {last node in correspoding bucket, nullptr}
    */

   size_t bucket_index = _hash_function(key) % _buckets_array.size();
   Node* prev = nullptr;
   Node* curr = _buckets_array[bucket_index];
   // traverse the linked list in the bucket
   while (curr != nullptr) {
    const auto& cur_key = curr->value.first;
    if (cur_key == key) {return {prev, curr};}
    prev = curr;
    curr = curr->next;
   }
    return {prev, curr};
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::make_iterator(Node* curr) {
    /*
    1. Create an iterator object with those info: 
    iterator(&_buckets_array, curr, index)
    */
   size_t index = bucket_count();
   if (curr != nullptr) {
    index = _hash_function(curr->value.first) % _buckets_array.size();
   }
   return iterator(&_buckets_array, curr, index);
} 

template<typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& output_stream, const HashMap<K, M, H>& map) {
    if (map.empty()) {return output_stream << "{}";}
    std::stringstream formatted_stream;

    for (const auto& kv_pair : map) {
        formatted_stream << kv_pair.first << ":" << kv_pair.second << ", ";
    }

    std::string map_string = formatted_stream.str();
    if (map_string.size() != 0) {map_string.erase(map_string.size() - 2, 2);}// remove the last ", "
    output_stream << "{" << map_string << "}";
    return output_stream;
}


template<typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    if (lhs.size() != rhs.size()) {return false;}

    for (const auto& lhs_pair : lhs) {
        if (!rhs.contains(lhs_pair.first)) {return false;}
        if (lhs_pair.second != rhs.at(lhs_pair.first)) {return false;}
    }
    return true;
}
template<typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    return !(lhs == rhs);
}