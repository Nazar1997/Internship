#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <set> 
#include <iterator>

using std::vector;
using std::pair;

class Decision_Tree {
private:
    using TAtribute = pair<size_t, double>; // номер атрибута и порог
    struct Node {
        TAtribute the_atribute;
        Node *left;//если меньше порога
        Node *right;//если больше порога
        int main_class = -1; // класс не определен
    };
    size_t depth = 0;
    Node* Root;
    size_t training_set_quantity = 0;; //количество объектов учебной выборки
    vector<vector<double>> X;
    vector<size_t> Y;
    vector<size_t> classes;
    vector<TAtribute> attributes;
    Node *Tree_Construction(vector<size_t>& available_data) {
        vector<size_t> frequensly(classes.size(), 0);
        for (size_t i = 0; i != available_data.size(); ++i) {
            auto it = std::lower_bound(classes.begin(), classes.end(), Y[available_data[i]]);
            ++frequensly[it - classes.begin()];
        }
        bool is_node_a_leaf = false;
        size_t most_frequensly_class = 0;
        for (size_t i = 0; i != classes.size(); ++i) {
            if (frequensly[i] * 1.0 / available_data.size() > 0.95) {
                most_frequensly_class = classes[i];
                is_node_a_leaf = true;
                break;
            }
        }
        if (is_node_a_leaf) {
            Node *the_answer = new Node;
            the_answer->main_class = most_frequensly_class;
            return the_answer;
        }
        else {
            int attribute_number = -1; // атрибут который выберем
            double Gini = -1;
            for (size_t i = 0; i != attributes.size(); ++i) {//анализируем все атрибуты
                vector<size_t> left_classes(classes.size(), 0), right_classes(classes.size(), 0);
                for (size_t q = 0; q != available_data.size(); ++q) {
                    if (X[available_data[q]][attributes[i].first] < attributes[i].second) {
                        auto it = std::lower_bound(classes.begin(), classes.end(), Y[available_data[q]]);
                        ++left_classes[it - classes.begin()];
                    }
                    else {
                        auto it = std::lower_bound(classes.begin(), classes.end(), Y[available_data[q]]);
                        ++right_classes[it - classes.begin()];
                    }
                }
                size_t left_elements = 0, left_info = 0;
                size_t right_elemnts = 0, right_info = 0;
                for (size_t q = 0; q != classes.size(); ++q) {
                    left_elements += left_classes[q];
                    right_elemnts += right_classes[q];
                    left_info += left_classes[q] * left_classes[q];
                    right_info += right_classes[q] * right_classes[q];
                }
                if (left_elements*right_elemnts != 0 && Gini < left_info / left_elements + right_info / right_elemnts) {
                    Gini = left_info / left_elements + right_info / right_elemnts;
                    attribute_number = i;
                }
            }
            Node *the_answer = new Node;
            the_answer->the_atribute = attributes[attribute_number];
            vector<size_t> left_data, right_data;
            for (size_t i = 0; i != available_data.size(); ++i) {
                if (X[available_data[i]][attributes[attribute_number].first] > attributes[attribute_number].second) {
                    right_data.push_back(available_data[i]);
                }
                else {
                    left_data.push_back(available_data[i]);
                }
            }
            Node *left = new Node;
            left = Tree_Construction(left_data);
            Node *right = new Node;
            right = Tree_Construction(right_data);
            the_answer->left = left;
            the_answer->right = right;
            return the_answer;
        }
    }
    void Tree_Destruction(Node *the_node) {
        if (the_node->main_class != -1) {
            return;
        }
        else {
            Tree_Destruction(the_node->right);
            delete the_node->right;
            Tree_Destruction(the_node->left);
            delete the_node->left;
        }
    }
public:

    Decision_Tree() {

    }
    ~Decision_Tree() {
        Tree_Destruction(Root);
        delete Root;
    }
    void Pruning() {


    }
    void training() {
        std::ifstream fin("data.txt");
        size_t attribute_quantity;
        fin >> attribute_quantity;
        size_t object_quantity;
        fin >> object_quantity;
        for (size_t i = 0; i != object_quantity; ++i) {
            vector<double> object;
            for (size_t q = 0; q != attribute_quantity; ++q) {
                double data;
                fin >> data;
                object.push_back(data);
            }
            X.push_back(object);
            size_t class_of_object;
            fin >> class_of_object;
            Y.push_back(class_of_object);
        }
        for (size_t i = 0; i != X[0].size(); ++i) {//проходим все атрибуты
            std::set<double> values;
            for (size_t q = 0; q != X.size(); ++q) {//создание коллекции значений
                values.insert(X[q][i]);
            }
            auto it = values.begin();
            while (it != --values.end()) {
                attributes.push_back(std::make_pair(i, (*it + *(++it)) / 2));//раставляем пороги
            }
        }
        for (size_t i = 0; i != Y.size(); ++i) {
            auto it = find(classes.begin(), classes.end(), Y[i]);
            if (it == classes.end()) {
                classes.push_back(Y[i]);
            }
        }
        sort(classes.begin(), classes.end());
        double qualiti = 0; // процент верных ответов построенного дерева
        while (qualiti < 0.8 && training_set_quantity < object_quantity / 2) {
            training_set_quantity += object_quantity / 20;
            vector<size_t> available_data(training_set_quantity);//укажем доступную выборку
            for (size_t i = 0; i != training_set_quantity; ++i) {
                available_data[i] = i;
            }
            Root = Tree_Construction(available_data);
            size_t right = 0, mistakes = 0;
            for (size_t i = training_set_quantity; i != X.size(); ++i) {
                if (classificator(X[i]) == Y[i]) {
                    ++right;
                } else {
                    ++mistakes;
                }
            }
            qualiti = right * 1.0 / (right + mistakes);
        }
    }
    size_t classificator(vector<double>& object) {
        Node* the_Node = Root;
        size_t dep = 0;
        while (the_Node->main_class == -1) {
            if (object[the_Node->the_atribute.first] > the_Node->the_atribute.second) {
                the_Node = the_Node->right;
            }
            else {
                the_Node = the_Node->left;
            }
            ++dep;
            if (dep > depth) {
                depth = dep;
            }
        }
        return the_Node->main_class;
    }
    double purest() {
        size_t a=0, b=0;
        for (size_t i = 0; i != X.size(); ++i) {
            if (classificator(X[i]) == Y[i]) {
                ++a;
            }
            else {
                ++b;
            }
        }
        return a * 1.0 / (a + b);
    }
};

int main() {
    Decision_Tree the_Tree;
    the_Tree.training();
    double i = the_Tree.purest();
    std::cout << i << std::endl;
    return 0;
}