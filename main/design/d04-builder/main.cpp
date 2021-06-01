#include <iostream>
#include "builder.h"
#include "product.h"
#include "director.h"
using namespace std;

int main()
{
    Director *dt = new Director();
    //根据需要客户端的需要传入
    char ch;
    //客户到来，输入需要哪种套餐A或B
    for (int i = 0; i < 2; i++)
    {
        cout << "Please input A/B:" << endl;
        cin >> ch;
        cout << "-------------------" << endl;
        dt->foodInfoPoint(ch)->showFood();
    }
    delete dt;
    return 0;
}
