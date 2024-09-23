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
inline size_t HashMap<K, M, H>::size(){
    return _size;
}

template<typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty(){
    return _size == 0;
}

template<typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor(){
    return ((float) _size) / _buckets_array.size();
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() {
    return _buckets_array.size();
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) {
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
void HashMap<K, M, H>::clear() {
    for (auto& bucket : _buckets_array) {
        // bucket is the head of linkedlist, traver linkedlist and delete each node
        while (bucket != nullptr) {
            auto tmp_bkt = bucket->next;
            delete bucket;
            bucket = tmp_bkt;
        }
    }
    size = 0; 
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