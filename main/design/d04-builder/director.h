#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_
 
class Builder;
class ProductFood;
class Director      //指导者，担任这个角色的类调用具体建造者角色以创建产品对象。此处是收银员
{
    public:
        Director();
        ~Director();
        ProductFood *foodInfoPoint(char ch);
    protected:
    private:
        Builder *_bld;
};
 
#endif
