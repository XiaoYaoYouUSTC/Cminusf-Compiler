int gcd (int u, int v) {
    if (v == 0) return u;
    else return gcd(v, u - u / v * v);
}

void main(void) {
    int x;
    int y;
    int temp;
    x = 7;
    y = 8;
    if (x < y) {
        temp = x;
        x = y;
        y = temp;
    }
    temp = gcd(x, y);
    output(temp);
    return;
}