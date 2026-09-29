#include <unity.h>
#include "MyVectorLib.h"

void setUp(void)
{
		
	return;
}
void tearDown(void)
{
	return;
}

void test_MyVectorLib_Init(void)
{	
	MyVectorLib_Init();
	TEST_ASSERT_EQUAL_INT(0, MyVectorLib_Len());	
}
	
void test_MyVectorLib_AddCheckSize(void)
{	
	MyVectorLib_Add(0);
	MyVectorLib_Add(27);
	MyVectorLib_Add(491);
	MyVectorLib_Add(520);
	MyVectorLib_Add(900);
	MyVectorLib_Add(3213);
	MyVectorLib_Add(12343);
	MyVectorLib_Add(999);
	TEST_ASSERT_EQUAL_INT(8, MyVectorLib_Len());	
}	

void test_MyVectorLib_Find_NotThere(void)
{
	TEST_ASSERT_EQUAL_INT(0, MyVectorLib_Find(-14));	
	TEST_ASSERT_EQUAL_INT(0, MyVectorLib_Find(40000));
	TEST_ASSERT_EQUAL_INT(0, MyVectorLib_Find(999999));
}

void test_MyVectorLib_Find_AreThere(void)
{
	TEST_ASSERT_EQUAL_INT(1, MyVectorLib_Find(0));
	TEST_ASSERT_EQUAL_INT(2, MyVectorLib_Find(27));
	TEST_ASSERT_EQUAL_INT(5, MyVectorLib_Find(900));
	TEST_ASSERT_EQUAL_INT(8, MyVectorLib_Find(999));
	
}
void test_MyVectorLib_Len_RightSize(void)
{
	TEST_ASSERT_EQUAL_INT(8, MyVectorLib_Len());
}

void test_MyVectorLib_Removelast(void)
{
	MyVectorLib_Removelast();
	TEST_ASSERT_EQUAL_INT(7, MyVectorLib_Len());
}

int main(void)
{
	UNITY_BEGIN();
	
	RUN_TEST(test_MyVectorLib_AddCheckSize);			
	RUN_TEST(test_MyVectorLib_Find_NotThere);
	RUN_TEST(test_MyVectorLib_Find_AreThere);
	RUN_TEST(test_MyVectorLib_Len_RightSize);
	RUN_TEST(test_MyVectorLib_Removelast);
		
	return UNITY_END();
}
