#include <string.h> 		/* For memset */
#include <MyVectorLib.h>

static int MyVectorLib_Vect[MYVECTLIB_MAXLEN]; 	/* Vector */
static int MyVectLen=0;							/* Number of elements in the vector*/

/* Initializes the vector */
void MyVectorLib_Init(void){
	MyVectLen=0;
	memset(MyVectorLib_Vect,0,sizeof(MyVectorLib_Vect));
	return;
} 		

/* Adds an element to the vector */
int MyVectorLib_Add(int number){
	MyVectorLib_Vect[MyVectLen]=number;
	MyVectLen++; 	
	return MYVECTLIB_OK;
}	

/* Removes the last element from the vector */
void MyVectorLib_Removelast(){
	if (MyVectLen > 0) {
		MyVectLen--;
	}
}

/* Returns the position of number (is it an index or ordinal?) */
int MyVectorLib_Find(int number)
{
    int i = 0;
        
    while (i < MyVectLen) {		
        if (MyVectorLib_Vect[i] == number)
			return (i+1);
		i++;
	}
	
    return MYVECTLIB_NOTFOUND;
}

/* Returns the number of elements in the vector */
int MyVectorLib_Len(void)
{
	return MyVectLen;
}
