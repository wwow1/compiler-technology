int wow(){
	int a=2,b;
	b=5;
	while(a<10){
		a=a+1;
		if(a<b){
			b=b-1;
		}
		else{
			return a;
		}
	}
	return a>b;
}
