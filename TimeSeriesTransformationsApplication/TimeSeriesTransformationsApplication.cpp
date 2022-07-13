using namespace std;
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include<algorithm>
#include<numeric>

#include "../TimeSeriesTransformations\TimeSeriesTransformations.h"

template <typename T>
void print(const std::vector<T>& vector)
{
	int sizeVec = vector.size();
	for (int i = 0; i < sizeVec; i++)
	{
		std::cout << vector[i] << '\n';
	}
}

int main()
{
	// READY TO USE.

	system("pause>0");
	return 0;
}