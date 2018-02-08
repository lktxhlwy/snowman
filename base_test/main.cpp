#include<iostream>
#include<stdlib.h>

class A
{
public:
	virtual void foo(int * a) = 0;
};

class B :public A
{
public:
	void foo(int * a) {
		printf("foo() in B, number = %d\n",*a);
	}
};

class C :public A
{
public:
	void foo(int *a) {
		printf("foo() in C, number = %d\n", *a);
	}
};

int main()
{
	B b;
	C c;
	A* pb = &b;
	A* pc = &c;
	int i = 123;
	pb->foo(&i);
	pc->foo(&i);

	system("pause");

	return 0;
}