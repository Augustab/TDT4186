#include <stdio.h>
//Creating some variables for showing different addresses in memory

static int staticGlobalInitializedInt = 1;
float globalFloatVariable = 35.2;
char globalUninitializedCharArray[10];

int *pointer = &staticGlobalInitializedInt;

int sum_n(int n) {
    char localChar[] = "a";
    int n3=2;

    if (n==1){
        // Print to show that deeper recursion leads to lower address
        printf("Address of local variable in recursive method (1) n3 is %p\n", &n3);
    }

    if (n==65530){
        // Print to show that shallow recursion leads to higher address
        printf("Address of local variabel in recursive method (65530) n3 is %p\n", &n3);
    }

    if (n!=0) {
        //Recursive call
        return n + sum_n(n-1);
    } 
    
    return 0;
}


void print_addresses_of_dif_types(){
    // code for adding variables in its own function so they are in their own
    // frame and are not affected by local variables of main()
    char c; // char has byte size 1
    int n1 = 35; // int has byte size 4
    int n2 = 40;

    printf("\nAddress of local char is %p\n", &c);
    printf("Address of local n1 is %p\n", &n1);
    printf("Address of local n2 is %p\n", &n2);
}


int main() {
    //works up to 65535, negative sum after
    //works up to ca. 174 000 
    
    int n = 65535;
    int sum = sum_n(n);

    printf("\nThe sum of numbers from 1 to %i is %i\n\n", n, sum);
    

  
    //Print addresses of different variables:
    printf("Address of staticGlobalInitializedInt is %p\n", &staticGlobalInitializedInt);
    printf("Address of globalFloatVariable is %p\n", &globalFloatVariable);
    printf("Address of globalUninitializedCharArray is %p\n", &globalUninitializedCharArray);
    printf("Address of globalPointer is %p\n", &pointer);
    printf("Address of local n is %p\n", &n);
    printf("Address of local sum is %p\n", &sum);

    print_addresses_of_dif_types();

    return 0;
}
