/*Basic function part*/

float MyFuncA(int a, float x){
    if(a <= 0){
        return x+x*x;
    }else{
        return MyFuncA(a-1, x+.1);
    }
}

int main(void){
    float result;

    result = MyfuncA(5, 1.) + MyFuncA(0, MyFuncA(0, .1));

    return 0;
}