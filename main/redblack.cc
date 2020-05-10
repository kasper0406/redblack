#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

enum Color { Red, Black };

template<class T> struct Node {
    Node(T value) : left(nullptr), right(nullptr), parent(nullptr), color(Red), value(value) {}

    unique_ptr<Node> left;
    unique_ptr<Node> right;
    Node* parent;
    Color color;
    T value;
};

template<class T> class Tree {
public:
    Tree() : m_root(nullptr), size(0) { }

    void Insert(T value) {
        // cout << "Insert: " << value << endl;
        if (size == 0) {
            m_root = unique_ptr<Node<T>>(new Node<T>(value));
            m_root->color = Black;
        } else {
            assert(m_root != nullptr);
            InsertInternal(value);
        }
        size += 1;
    }

    template<class Traverser> void Traverse(Traverser& t) {
        t.Traverse(m_root.get());
    }

private:
    unique_ptr<Node<T>> m_root;
    size_t size;

    void InsertInternal(T value) {
        auto node = unique_ptr<Node<T>>(new Node<T>(value));
        Node<T>* node_ptr = node.get();
        auto insert_node = GetInsertNode(node.get());

        node->parent = insert_node;
        if (node->value < insert_node->value) {
            assert(insert_node->left == nullptr);
            insert_node->left = move(node);
        } else {
            assert(insert_node->right == nullptr);
            insert_node->right = move(node);
        }

        if (insert_node->color == Red) {
            Fix(insert_node, node_ptr);
        }
    }

    Node<T>* GetInsertNode(Node<T>* node) {
        Node<T>* ip_parent = m_root.get();
        Node<T>* ip_child = ip_parent;
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

    void Fix(Node<T>* parent, Node<T>* node) {
        // cout << "Fixing node: " << node << ", value: " << node->value << endl;
        if (parent == nullptr) {
            node->color = Black;
            return;
        }

        if (parent->color != Red) {
            // Nothing to do, as rule is not violated
            return;
        }
        
        if (Parent(parent) == nullptr) {
            // We are the root. In this case we can simply change our color to black
            parent->color = Black;
        } else if (Sibling(parent) != nullptr && Sibling(parent)->color == Red) {
            // cout << "Propagating red color" << endl;
            parent->color = Black;
            Sibling(parent)->color = Black;
            Parent(parent)->color = Red;
            // cout << "FOOBAR" << endl;
            Fix(Parent(Parent(parent)), Parent(parent));
        } else if (parent->parent->left.get() == parent) {
            Node<T>* grandparent = parent->parent;
            // cout << "Case 1" << endl;
            if (node == parent->right.get()) {
                RotateLeft(parent);
            }
            RotateRight(grandparent);
        } else if (parent->parent->right.get() == parent) {
            Node<T>* grandparent = parent->parent;
            // cout << "Case 2" << endl;
            if (node == parent->left.get()) {
                RotateRight(parent);
            }
            RotateLeft(grandparent);
        } else {
            throw runtime_error("Unexpected case!");
        }
    }

    void RotateLeft(Node<T>* node) {
        // cout << "Rotating left" << endl;

        if (node->right == nullptr) {
            // Not possible to rotate left
            return;
        }

        // cout << "Node value: " << node->value << ", node parent: " << node->parent << endl;

        unique_ptr<Node<T>> new_parent = move(node->right);
        Node<T>* new_parent_ptr = new_parent.get();
        if (new_parent->left != nullptr) {
            new_parent->left->parent = node;
        }
        node->right = move(new_parent->left);

        if (node->parent == nullptr) {
            // cout << "Root change" << endl;
            new_parent->left = move(m_root);
            new_parent->parent = nullptr;
            m_root = move(new_parent);
        } else if (node->parent->left.get() == node) {
            new_parent->left = move(node->parent->left);
            new_parent->parent = node->parent;
            node->parent->left = move(new_parent);
        } else if (node->parent->right.get() == node) {
            new_parent->left = move(node->parent->right);
            new_parent->parent = node->parent;
            node->parent->right = move(new_parent);
        }

        node->parent = new_parent_ptr;

        new_parent_ptr->color = Black;
        node->color = Red;
    }

    void RotateRight(Node<T>* node) {
        // cout << "Rotating right" << endl;

        if (node->left == nullptr) {
            // Not possible to rotate right
            return;
        }

        // cout << "Node value: " << node->value << ", node parent: " << node->parent << endl;

        unique_ptr<Node<T>> new_parent = move(node->left);
        Node<T>* new_parent_ptr = new_parent.get();
        if (new_parent->right != nullptr) {
            new_parent->right->parent = node;
        }
        node->left = move(new_parent->right);

        if (node->parent == nullptr) {
            // cout << "Root change" << endl;
            new_parent->right = move(m_root);
            new_parent->parent = nullptr;
            m_root = move(new_parent);
        } else if (node->parent->left.get() == node) {
            new_parent->right = move(node->parent->left);
            new_parent->parent = node->parent;
            node->parent->left = move(new_parent);
        } else if (node->parent->right.get() == node) {
            new_parent->right = move(node->parent->right);
            new_parent->parent = node->parent;
            node->parent->right = move(new_parent);
        }

        node->parent = new_parent_ptr;

        new_parent_ptr->color = Black;
        node->color = Red;
    }

    Node<T>* Parent(Node<T>* node) {
        if (node == nullptr) {
            return nullptr;
        }
        return node->parent;
    }

    Node<T>* Sibling(Node<T>* node) {
        if (node->parent == nullptr) {
            return nullptr;
        }
        if (node->parent->left.get() == node) {
            return node->parent->right.get();
        } else {
            return node->parent->left.get();
        }
    }
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

template<class T> class RedBlackValidator {
public:
    void Traverse(Node<T>* node) {
        size_t target = 0;
        Node<T>* current = node;
        while (current != nullptr) {
            if (current->color == Black) {
                target++;
            }
            current = current->left.get();
        }
        m_target_depth = target;

        InternalTraverse(node, 0);
    }

private:
    size_t m_target_depth;

    void InternalTraverse(Node<T>* node, size_t depth) {
        if (node == nullptr) {
            if (depth != m_target_depth) {
                throw runtime_error("Black depth constraint violated");
            }
            return;
        }

        if (node->left != nullptr) {
            if (node->left->parent != node) {
                throw runtime_error("Invalid integrity: Left parent pointer invalid");
            }
        }
        if (node->right != nullptr) {
            if (node->right->parent != node) {
                throw runtime_error("Invalid integrity: Right parent pointer invalid");
            }
        }

        if (node->parent != nullptr) {
            if (node->parent->color == Red && node->color == Red) {
                throw runtime_error("Violation of red-black property");
            }
        }

        size_t new_depth = depth;
        if (node->color == Black) {
            new_depth += 1;
        }

        InternalTraverse(node->left.get(), new_depth);
        InternalTraverse(node->right.get(), new_depth);
    }
};

template<class T> class DotTraverser {
public:
    DotTraverser(string filename) {
        m_file = ofstream(filename);
        if (!m_file) {
            throw runtime_error("Failed to open file: " + filename);
        }
    }

    void Traverse(Node<T>* node) {
        m_file << "digraph T {" << endl;
        TraverseInternal(node);
        m_file << "}" << endl;
    }

private:
    ofstream m_file;

    void TraverseInternal(Node<T>* node) {
        if (node == nullptr) {
            return;
        }

        m_file << NodeName(node) << " [label=\"" << node->value << "\", color=" << NodeColor(node) << "];" << endl;

        if (node->left != nullptr) {
            TraverseInternal(node->left.get());
            m_file << NodeName(node) << " -> " << NodeName(node->left.get()) << " [color=green];" << endl;
        }
        if (node->right != nullptr) {
            TraverseInternal(node->right.get());
            m_file << NodeName(node) << " -> " << NodeName(node->right.get()) << " [color=blue];" << endl;
        }
        if (node->parent != nullptr) {
            m_file << NodeName(node) << " -> " << NodeName(node->parent) << " [style=dotted];" << endl;
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
    vector<int64_t> to_insert;
    for (int i = 0; i < 1000; i++) {
        to_insert.push_back(i);
    }
    random_shuffle(to_insert.begin(), to_insert.end());

    RedBlackValidator<int64_t> validator;
    Tree<int64_t> tree;
    for (auto v : to_insert) {
        tree.Insert(v);

        tree.Traverse(validator);
    }

    DotTraverser<int64_t> dot_traverser("/Volumes/git/red-black/tree.dot");
    tree.Traverse(dot_traverser);

    return 0;
}
