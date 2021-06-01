#include "product.h"

ProductFood::ProductFood()
{
    mFoodName.clear();
    mPrice.clear();
}

ProductFood::~ProductFood()
{
}

void ProductFood::addFood(string foodName, int price)
{
    mFoodName.push_back(foodName);
    mPrice.push_back(price);
}

void ProductFood::showFood()
{
    int totalPrice = 0;
    cout << "------food list:------" << endl;
    for (int i = 0; i < mFoodName.size(); ++i)
    {
        totalPrice += mPrice[i];
        cout << mFoodName[i] << " = " << mPrice[i] << " yuan" << endl;
    }
    cout << "******total = " << totalPrice << " yuan" << endl;
}
