#ifndef _PRODUCT_H_
#define _PRODUCT_H_
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class ProductFood
{
public:
    ProductFood();
    ~ProductFood();
    void addFood(string foodName, int price);
    void showFood();

private:
    vector<string> mFoodName; //食品名称
    vector<int> mPrice;       //食品价格
};

#endif
