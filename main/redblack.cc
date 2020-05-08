#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

enum Color { Red, Black };

template<class T> struct Node {
    Node(T value) : left(nullptr), right(nullptr), parent(nullptr), color(Red), value(value) {}

    unique_ptr<Node> left;
    unique_ptr<Node> right;
    Node* parent;
    Color color;
    T value;

    void Insert(T value) {
        auto node = unique_ptr<Node<T>>(new Node<T>(value));
        auto insert_node = this->GetInsertNode(node.get());

        node->parent = insert_node;
        if (node->value < insert_node->value) {
            assert(insert_node->left == nullptr);
            insert_node->left = move(node);
        } else {
            assert(insert_node->right == nullptr);
            insert_node->right = move(node);
        }
    }

private:
    Node* GetInsertNode(Node* node) {
        Node* ip_parent = this;
        Node* ip_child = ip_parent;
        while (ip_child != nullptr) {
            ip_parent = ip_child;
            if (node->value < ip_child->value) {
                ip_child = ip_child->left.get();
            } else {
                ip_child = ip_child->right.get();
            }
        }
        return ip_parent;
    }
};

template<class T> class Tree {
public:
    Tree() : m_root(nullptr), size(0) { }

    void Insert(T value) {
        cout << "Insert: " << value << endl;
        if (size == 0) {
            m_root = unique_ptr<Node<T>>(new Node<T>(value));
            m_root->color = Black;
        } else {
            assert(m_root != nullptr);
            m_root->Insert(value);
        }
        size += 1;
    }

    template<class Traverser> void Traverse(Traverser t) {
        t.Traverse(m_root.get());
    }

private:
    unique_ptr<Node<T>> m_root;
    size_t size;
};

template<class T> class InorderTraverser {
public:
    void Traverse(Node<T>* node) {
        if (node == nullptr) {
            return;
        }

        Traverse(node->left.get());
        cout << node->value << endl;
        Traverse(node->right.get());
    }
};

template<class T> class DotTraverser {
public:
    void Traverse(Node<T>* node) {
        cout << "graph T {" << endl;
        TraverseInternal(node);
        cout << "}" << endl;
    }

private:
    void TraverseInternal(Node<T>* node) {
        if (node == nullptr) {
            return;
        }

        cout << NodeName(node) << " [label=\"" << node->value << "\", color=" << NodeColor(node) << "];" << endl;

        if (node->left != nullptr) {
            TraverseInternal(node->left.get());
            cout << NodeName(node) << " -> " << NodeName(node->left.get()) << ";" << endl;
        }
        if (node->right != nullptr) {
            TraverseInternal(node->right.get());
            cout << NodeName(node) << " -> " << NodeName(node->right.get()) << ";" << endl;
        }
        if (node->parent != nullptr) {
            cout << NodeName(node) << " -> " << NodeName(node->parent) << " [style=dotted];" << endl;
        }
    }

    string NodeName(Node<T>* node) {
        stringstream s;
        s << "\"" << node << "\"";
        return s.str();
    }

    string NodeColor(Node<T>* node) {
        if (node->color == Black) {
            return "black";
        } else if (node->color == Red) {
            return "red";
        }
        throw runtime_error("Unexpected color");
    }
};

int main() {
    Tree<int64_t> tree;
    tree.Insert(24);
    tree.Insert(56);
    tree.Insert(1);
    tree.Insert(2);
    tree.Insert(3);
    tree.Insert(4);
    tree.Insert(5);

    cout << "Starting traversal..." << endl;

    DotTraverser<int64_t> traverser;
    tree.Traverse(traverser);

    return 0;
}
