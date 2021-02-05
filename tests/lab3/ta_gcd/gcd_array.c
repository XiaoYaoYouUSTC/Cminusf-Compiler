int x[1];
int y[1];

int gcd (int u, int v) { 
    if (v == 0) return u;
    else return gcd(v, u - u / v * v); 
}

int funArray (int u[], int v[]) { 
    int a;
    int b;
    int temp;
    a = u[0];
    b = v[0];
    if (a < b) {
        temp = a;
        a = b;
        b = temp;
    }
    return gcd(a, b);
}

int main(void) {
    x[0] = 90;
    y[0] = 18;
    return funArray(x, y);
}