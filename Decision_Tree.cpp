#include <algorithm>
#include <iostream>
#include <vector>
#include <set> 
#include "math.h"

using std::cin;
using std::cout;
using std::vector;
using std::pair;
using TAtribute = pair<size_t, double>;

struct Node{
    TAtribute the_atribute;
    Node *left;
    Node *right;
    bool main_func(double value) {
        return value > the_atribute.second;
    }
};




int main()
{

    vector<vector<double>> X;//������ �� ����
    vector<double> Y;//������
    input(X, Y);
    vector<TAtribute> attributes;//����� �������� � �����
    for (size_t i = 0; i != X.size(); ++i) {
        std::set<double> values;
        for (size_t q = 0; q != X[i].size();++q) {//�������� ��������� ��������
            values.insert(X[i][q]);
        }
        auto it = values.begin();
        while (it != --values.end()) {
            attributes.push_back(std::make_pair(i, (*it + *(++it)) / 2));//���������� ������
        }
    }


    return 0;
}

void input(vector<vector<double>>, vector<double>) {
    //�������������� ������
}

Node Tree_Construction(vector<vector<double>> X, vector<double> Y, vector<TAtribute> attributes) {


}