
// Quicksort

void swap(char *a, char *b){
   	int tmp = *a;
   	*a = *b;
   	*b = tmp;
}

int partition (char *s, int left, int right){
	int pivot,i,j;
	pivot = s[left];
	i = left;
	j = right+1;

   while(TRUE)
   {
        do ++i; while(s[i] <= pivot && i <= right);
        do --j; while(s[j] > pivot);
        if(i >= j)
            break;
        swap(&s[i], &s[j]);
   }
   swap(&s[left], &s[j]);
   return j;
}
void quickSort ( char *string, int left, int right){
	int i;
	
	if ( left < right){
	j = partition( s, left, right);
	quickSort( s, left, i-1);
	quickSort( s, j+1, right);
	}
}