#include <iostream>
#include <string>

using namespace std;

template <class T>

T my_min(T x, T y)
{
    return x < y ? x : y;
}

template <typename T1, typename T2>

class MyClass1
{
public:
    MyClass1(T1 a, T2 b);

public:
    void show();

private:
    T1 mem_a_;
    T2 mem_b_;
};

template <typename T1, typename T2>
MyClass1<T1, T2>::MyClass1(T1 a, T2 b) : mem_a_(a), mem_b_(b)
{
}

template <typename T1, typename T2>
void MyClass1<T1, T2>::show()
{
    cout << "mem_a_ is " << mem_a_ << ", mem_b_ is " << mem_b_ << endl;
}

/** List **/
template <typename Type>
class List
{
public:
    typedef unsigned size_type;

public:
    List();
    void push_back(const Type &value);
    void push_front(const Type &value);
    Type &front() const;
    Type &back() const;
    // Type* begin() const;
    // Type* end() const;
    size_type size() const;
    ~List();

private:
    struct DataType
    {
        Type val;
        struct DataType *next;
    } head_;
    struct DataType *end_;
};

template <typename Type>
List<Type>::List()
{
    head_.next = nullptr;
    end_ = &head_;
}
template <typename Type>
void List<Type>::push_back(const Type &val)
{
    DataType *n = new DataType;
    n->val = val;
    n->next = nullptr;
    end_->next = n;
    end_ = n;
}
template <typename Type>
void List<Type>::push_front(const Type &val)
{
    DataType *n = new DataType;
    n->val = val;
    n->next = head_.next;
    head_.next = n;
}
template <typename Type>
Type &List<Type>::front() const
{
    return head_.next->val;
}
template <typename Type>
Type &List<Type>::back() const
{
    return end_->val;
}
// template <typename Type>
// Type* List<Type>::begin() const
// {
//     return &head_;
// }
// template <typename Type>
// Type* List<Type>::end() const
// {
//     return end_;
// }
template <typename Type>
typename List<Type>::size_type List<Type>::size() const
{
    size_type size = 0;
    struct DataType *iter = &head_;
    while (iter->next)
    {
        iter = iter->next;
        size++;
    }
    return size;
}
template <typename Type>
List<Type>::~List()
{
    struct DataType *node = (&head_);
    while (node = node->next)
    {
        head_.next = node->next;
        delete node;
        node = &head_;
    }
}

int main()
{
    cout << "template test:" << endl;
    int a = 5, b = 6;
    cout << "min: " << my_min(a, b) << endl;
    double c = 5.6, d = 99.0;
    cout << "min: " << my_min(c, d) << endl;

    MyClass1<int, int> c1(3, 5);
    c1.show();
    MyClass1<int, char> c2(3, 'a');
    c2.show();
    MyClass1<int, string> c3(3, "hello");
    c3.show();

    cout << "template list:" << endl;
    List<int> list;
    list.push_back(10);
    list.push_front(200);
    list.push_back(3);
    cout << "front " << list.front() << ", end " << list.back() << endl;

    return 0;
}
