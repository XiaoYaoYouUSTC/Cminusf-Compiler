int global[25];
void main(void){
	int i;
	int j;

	int a[25];

	i=0;
	while (i < 25){
		a[i] = i;
		i=i+1;
	}
	
	i = 0;
	while (i < 5){
		j = 0;
		while (j < 5){
			int x;
			int y;

			x = i+j;
			y = i*j;

			if(a[x] > a[y])
			{
				global[i * 5 + j] = a[x];
			}
			else
			{
				global[i * 5 + j] = a[y];
			}
			j = j + 1;
		}
		i = 1 + i;
	}

	i=0;
	while (i < 5){
		j=0;
		while (j < 5){
			output(global[i * 5 + j]);
			j = j + 1;
		}
		i = 1 + i;
	}
	return;
}