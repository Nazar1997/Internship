#include <algorithm>
#include <iostream>
#include <vector>
#include <set> 

using std::cin;
using std::cout;
using std::vector;
using std::pair;
using TAtribute = pair<size_t, double>;//номер атрибута и порог

struct Node {
    TAtribute the_atribute;
    Node *left;//если меньше порога
    Node *right;//если больше порога
    int main_class = -1; // класс не определен
};

vector<vector<double>> input_X() {
    vector<vector<double>> X;
    return X;
    //упорядочивание данных
}

vector<size_t> input_Y() {
    vector<size_t> Y;
    return Y;
    //упорядочивание данных
}

Node *Tree_Construction(const vector<vector<double>>& X, vector<size_t>& Y,const vector<size_t>& classes, const vector<TAtribute>& attributes, vector<size_t>& available_data, size_t depth = 0)
{
    vector<size_t> frequensly(classes.size(), 0);
    for (size_t i = 0; i != available_data.size(); ++i) {
        auto it = std::lower_bound(classes.begin(), classes.end(), Y[available_data[i]]);
        ++frequensly[it - classes.begin()];
    }
    bool is_node_a_leaf = false;
    size_t most_frequensly_class = 0;
    for (size_t i = 0; i != classes.size(); ++i) {
        if (frequensly[i] / available_data.size() > 0.8) {
            most_frequensly_class = i;
            is_node_a_leaf = true;
            break;
        }
    }
    if (is_node_a_leaf) {
        Node *the_answer = new Node;
        the_answer->main_class = most_frequensly_class;
        return the_answer;
    } else {
        size_t attribute_number = 0; // атрибут который выберем
        double Gini = -1;
        for (size_t i = 0; i != attributes.size(); ++i) {//анализируем все атрибуты
            vector<size_t> left_classes(classes.size()), right_classes(classes.size());
            for (size_t q = 0; q != available_data.size(); ++q) {
                if (X[available_data[q]][attributes[i].first] < attributes[i].second) {
                    auto it = std::lower_bound(left_classes.begin(), left_classes.end(), Y[available_data[q]]);
                    ++left_classes[it - left_classes.begin()];
                }
                else {
                    auto it = std::lower_bound(right_classes.begin(), right_classes.end(), Y[available_data[q]]);
                    ++right_classes[it - right_classes.begin()];
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
            if (Gini < left_elements*left_info + right_elemnts*right_info) {
                Gini = left_elements*left_info + right_elemnts*right_info;
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
        left = Tree_Construction(X, Y, classes, attributes, left_data, depth + 1);
        Node *right = new Node;
        right = Tree_Construction(X, Y, classes, attributes, right_data, depth + 1);
        the_answer->left = left;
        the_answer->right = right;
        return the_answer;
    }
}

vector<TAtribute> Making_atrributes(vector<vector<double>>& X) {
    vector<TAtribute> attributes;//номер атрибута и порог
    for (size_t i = 0; i != X[0].size(); ++i) {//проходим все атрибуты
        std::set<double> values;
        for (size_t q = 0; q != X.size(); ++q) {//создание коллекции значений
            values.insert(X[i][q]);
        }
        auto it = values.begin();
        while (it != --values.end()) {
            attributes.push_back(std::make_pair(i, (*it + *(++it)) / 2));//раставляем пороги
        }
    }
    return attributes;
}

int main() {
    vector<vector<double>> X = input_X();//данные на вход
    vector<size_t> Y = input_Y();//классы
    vector<TAtribute> attributes = Making_atrributes(X); // создаем атрибуты и пороги
    vector<size_t> available_data(X.size());//укажем всю выборку как доступную
    for (size_t i = 0; i != X.size(); ++i) {
        available_data[i] = i;
    }
    vector<size_t> classes;
    for (size_t i = 0; i != Y.size(); ++i) {
        auto it = find(classes.begin(), classes.end(), Y[i]);
        if (it == classes.end()) {
            classes.push_back(Y[i]);
        }
    }
    sort(classes.begin(), classes.end());
    Node *Root = Tree_Construction(X, Y, classes, attributes, available_data);
    return 0;
}


