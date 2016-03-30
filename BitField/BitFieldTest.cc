#include "iostream"
#include "BitField.h"

using namespace std;

#include "gtest/gtest.h"
/*
int main (void)
{
	TBitField tBf(10);
	tBf.SetBit(1);
	cout << "Hello world!" << endl;
	cout << "Bit is " << tBf.GetBit(1) << " " << tBf.GetBit(0) << endl;
	return 0;
}
*/

TEST (BitTest, Set) {
	TBitField tBf(10);
	EXPECT_EQ(10, tBf.Width());
}
