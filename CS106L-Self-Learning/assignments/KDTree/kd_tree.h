/**
 * File: kd_tree.h
 * Author: (your name here)
 * ------------------------
 * An interface representing a kd-tree in some number of dimensions. The tree
 * can be constructed from a set of data and then queried for membership and
 * nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include <map>
#include "point.h"
#include "math.h"
#include "bounded_priority_queue.h"

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.

template <size_t N, typename ElemType>
class KDTree {
public:
    struct Node {
        Point<N> point;
        ElemType element;
        Node* left_node;
        Node* right_node;
    };
    
    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree();
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree();
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const;
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const;
    bool empty() const;
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const;
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value);
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt);
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
    // TODO: Add implementation details here.
    size_t tree_size;
    Node* root_node;

    void delete_tree(Node* root);
    Node* insert_node(Node*& currNode, const Point<N>& pt, const ElemType& value, int currDimension);
    Node* find_node(Node* currNode, const Point<N>& pt, int currDimension) const;
    void find_knn(BoundedPriorityQueue<ElemType>* bpq, Node* currNode, const Point<N>& pt, size_t currDimension) const;
};

// step 1  
template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() : tree_size(0), root_node(nullptr) {}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {delete_tree(root_node);}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::delete_tree(Node* root) {
    if (root == nullptr) {return;}
    delete_tree(root->left_node);
    delete_tree(root->right_node);
    delete root; 
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    return N; 
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {return tree_size;}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const {return tree_size == 0;}


// Inserts the specified point into the KDTree with
// associated value value. If the point already exists in the
// KDTree, the old value is overwritten.
template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert (const Point<N>& pt, const ElemType& value) {
    insert_node(root_node, pt, value, 0);
}




// insert_node method 
template <size_t N, typename ElemType>
typename KDTree<N, ElemType>::Node* KDTree<N, ElemType>::insert_node(Node*& currNode, const Point<N>& pt, const ElemType& value, int currDimension) {
    /*
    1. If the current node is null, create a new node with the point and value and return.
    2. If the current point is equal to the point, update the value and return.
    3. recursively compare, and insert the point to the left or right of the current node.
    */
   if (currNode == nullptr) {
        currNode = new Node;
        *currNode = {pt, value, nullptr, nullptr}; // member init list
        tree_size++;
        return currNode;
   }

   if (currNode->point == pt) {
        currNode->element = value;
        return currNode;
   }

   if (pt[currDimension] > currNode->point[currDimension]) {
    // go to the right 
    return insert_node(currNode->right_node, pt, value, (currDimension + 1) % N);
   }
   else {
    // go to the left
    return insert_node(currNode->left_node, pt, value, (currDimension + 1) % N);
        }
}



// Returns whether the specified Point is contained in the KDTree.
template <size_t N, typename ElemType> 
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const {
    Node* node = find_node(root_node, pt, 0);
    return node != nullptr;
}

template <size_t N, typename ElemType> 
typename KDTree<N, ElemType>::Node* KDTree<N, ElemType>::find_node
(Node* currNode, const Point<N>& pt, int currDimension) const {
    if (currNode == nullptr) {return nullptr;}
    if (currNode->point == pt) {return currNode;}

    // go to right subtree
    if (pt[currDimension] > currNode->point[currDimension]) {
        return find_node(currNode->right_node, pt, (currDimension+1) % N);
    }
    else {return find_node(currNode->left_node, pt, (currDimension+1) % N);}
}

// Returns a reference to the value associated with the
// point pt. If the point does not exist in the KDTree, it is
// added with the default value of ElemType as its value,
// and a reference to this new value is returned. This is the
// same behavior as the STL map's operator[].
// Note that this function does not have a const overload
// because the function may mutate the tree.
template <size_t N, typename ElemType> 
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& pt)
{
    Node* node = find_node(root_node, pt, 0);
    if (node == nullptr) {
        ElemType default_value = ElemType();
        node = insert_node(root_node, pt, default_value, 0);
    }
    return node->element;
}




// Returns a reference to the value associated with the
// point pt, if it exists. If the point is not in the tree, then
// this function throws an out_of_range exception.
template <size_t N, typename ElemType> 
ElemType& KDTree<N, ElemType>::at(const Point<N>& pt){
    Node* node = find_node(root_node, pt, 0);   
    if (node == nullptr) {
        throw std::out_of_range("Point not found in KDTree");
    }
    return node->element;
}


// This function is const-overloaded, since it does not
// change the tree.
template <size_t N, typename ElemType> 
const ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) const{
    return const_cast<KDTree<N, ElemType>*>(this)->at(pt);
}
template <size_t N, typename ElemType> 
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const {
    /*
    1. Create a bounded priority queue of size k.
    2. use find_knn to enqueue the k nearest neighbors.
    3. return the most common value in the queue.
    */
   BoundedPriorityQueue<ElemType>* bpq = new BoundedPriorityQueue<ElemType>(k);
   find_knn(bpq, root_node, key, 0);

   std::map<ElemType, size_t> freq_map;

   while (!bpq->empty()) {
    ElemType elem = bpq->dequeueMin();
    if (freq_map.contains(elem)) {
        freq_map[elem]++;
    }
    else {
        freq_map[elem] = 1;
    }
   }

    ElemType most_common_elem;
    size_t max_freq = 0;

    for (const auto& [elem, freq] : freq_map) {
        if (freq > max_freq) {
            max_freq = freq;
            most_common_elem = elem;
        }
    }

    delete bpq;
    return most_common_elem;
}

template <size_t N, typename ElemType> 
void KDTree<N, ElemType>::find_knn(BoundedPriorityQueue<ElemType>* bpq, Node* currNode, const Point<N>& pt, size_t currDimension) const {
    /*
    1. because we use bpq, we can insert visited nodes with prority, bpq will automatically remove the node with the highest priority(farthest element, the one with highest distance).
    2. Recurse into the left or right subtree, based on the current dimension
    3. Check if we should explore the other subtree based on distance
    */
   if (currNode == nullptr) {return;}

   bpq->enqueue(currNode->element, Distance(pt, currNode->point));

   int next_dimension = (currDimension + 1) % N;
   if (currNode->point[currDimension] > pt[currDimension]) {
    // go left 
    find_knn(bpq, currNode->left_node, pt, next_dimension);
   }
   else {find_knn(bpq, currNode->right_node, pt, next_dimension);}

    // check if we should explore the other subtree
    double distance = fabs(currNode->point[currDimension] - pt[currDimension]);
    if (bpq->worst() >distance || bpq->size() < bpq->maxSize()) {
        if (currNode->point[currDimension] > pt[currDimension]) 
        // we already checked the left subtree in previous step
        {find_knn(bpq, currNode->right_node, pt, next_dimension);}
        // we already checked the right subtree in previous step
        else {find_knn(bpq, currNode->left_node, pt, next_dimension);}
    }
}





#endif // KDTREE_INCLUDED
