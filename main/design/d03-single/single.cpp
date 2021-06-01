#include <iostream>
using namespace std;
class Singleton
{
public:
    static Singleton *GetInstance();
    static void DestoryInstance();

protected:
    Singleton();
    ~Singleton();

private:
    static Singleton *_instance;
};

/** implement **/
Singleton *Singleton::_instance = NULL;

Singleton::Singleton()
{
    cout << "Singleton constructor" << endl;
}
Singleton::~Singleton()
{
    cout << "Singleton destructor" << endl;
}
void Singleton::DestoryInstance()
{
    cout << "---DestoryInstance" << endl;
    if (_instance)
    {
        delete _instance;
        _instance = NULL;
    }
}
Singleton *Singleton::GetInstance()
{
    cout << "---GetInstance" << endl;
    if (_instance == NULL)
    {
        cout << "------new Singleton()" << endl;
        _instance = new Singleton();
    }
    else
    {
        cout << "------not new Singleton()" << endl;
    }
    return _instance;
}

int main()
{
    Singleton *sign = Singleton::GetInstance();
    Singleton *sign2 = Singleton::GetInstance();

    Singleton::DestoryInstance();
    return 0;
}
