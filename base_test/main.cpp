#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
using namespace std;

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

int abc()
{
	std::vector<unsigned char> in(2049 * 2049);
	std::ifstream f;
	f.open("terrain.raw", std::ios_base::binary);

	if (f) {
		f.read((char*)&in[0], (std::streamsize)in.size());
		f.close();
	}
	else {
		printf("load file failed\n");
	}

	std::ofstream of;
	of.open("terrain_512.raw", std::ios_base::binary);

	for (int i = 0; i < 2049; i++) {
		if (i % 4 == 0) {
			for (int j = 0; j < 2049; j++) {
				if (j % 4 == 0) {
					of << in[i * 2049 + j];
				}
			}
		}
		else
			continue;
	}

	of.close();

	return 0;
}

int main()
{
	std::vector<int> a(10);
	std::vector<unsigned int> b(100);

	a[b[10]] = 10;
}