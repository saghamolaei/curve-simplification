#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#define n 30

int main()
{
	double x, y;
	int i = 0;
	int j = 0;
	while (cin >> x >> y)
	{
		string s;
		stringstream ssin;
		ssin<<"chunk"<< j << ".txt";
		s = ssin.str();
		ofstream fout(s,ios::app);
		fout << x << " "<< y << endl;
		i++;
		if (i >= n)
		{
			i = 0;
			j++;
		}
	}

	return 0;
}