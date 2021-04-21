#include<stdio.h>
#include<time.h>
#include<math.h>
//测试品 使用分形初始化 
int* series_init(int n){
	int* series=(int*)malloc(n*sizeof(int));
	int i;
	for(i=0;i<n;i++){
		series[i]=i;
	}
	return series;
}
void swap(int* a,int* b){
	int c=*a;
	*a=*b;
	*b=c;
}
void series_shuffle(int* series,int n){
	int i;
	for(i=0;i<n;i++){
		swap(&series[i],&series[randInt(i)]);
	}
}
int randInt(int i){
	return ((rand()<<15)+rand())%(i+1);
}
int abs(int i){
	return i>0?i:-i;
}
int printP(int * panel,int n){
	int i;
	FILE *fp = NULL;
    fp = fopen("tmp.txt", "w+");
    fprintf(fp,"%d",n);
    for(i=0;i<n;i++)
	    fprintf(fp,"\n%d",panel[i]);
	fclose(fp);
	system("java -jar chessprt.jar");
	system("tmp.jpg");
}
int printPs(int * panel,int n){
	int i,j;
	for(i=0;i<n;i++){
	   for(j=0;j<n;j++){
	   	if(j==panel[i])printf("1");
	   	else printf("0");
		}
		printf("\n");
	}
}
int * forgePanel(int *initpanel,int n){
	int *diaX;
	int *diaY;
	diaX=(int *)malloc((2*n-1)*sizeof(int));
	diaY=(int *)malloc((2*n-1)*sizeof(int));
	int i,j,p,q;
	int crash=0;
	int restartC=0;
	int deltax=0;
	int crashcount;
	int *panel=initpanel;
	for(i=0;i<2*n-1;i++){
	   diaX[i]=0;
	   diaY[i]=0;
	}
	for(i=0;i<n;i++){
		diaX[panel[i]-i+n-1]++; 
		diaY[i+panel[i]]++;
	}
	for(i=0;i<2*n-1;i++){
		crash+=diaX[i]*(diaX[i]-1)/2;
		crash+=diaY[i]*(diaY[i]-1)/2;
	}
	while(crash>0){
		crashcount=0;
		for(p=0;p<n;p++)
			if(diaX[panel[p]-p+n-1]>1||diaY[panel[p]+p]>1)
			{
				crashcount++;
				for(q=n-1;q>=0;q--){
					if(q!=p){
						deltax=0;
						deltax-=diaX[panel[p]-p+n-1]-1;
						deltax-=diaY[panel[p]+p]-1;
						deltax-=diaX[panel[q]-q+n-1]-1;
						deltax-=diaY[panel[q]+q]-1;
						deltax+=diaX[panel[q]-p+n-1];
						deltax+=diaY[panel[q]+p];
						deltax+=diaX[panel[p]-q+n-1];
						deltax+=diaY[panel[p]+q];
						if(panel[p]+q==panel[q]+p||panel[p]+p==panel[q]+q)deltax+=2;
						if(deltax<0){
							crash+=deltax;
							diaX[panel[p]-p+n-1]--;
							diaY[panel[p]+p]--;
							diaX[panel[q]-q+n-1]--;
							diaY[panel[q]+q]--;
							diaX[panel[q]-p+n-1]++;
							diaY[panel[q]+p]++;
							diaX[panel[p]-q+n-1]++;
							diaY[panel[p]+q]++;
							swap(panel+p,panel+q);
							break;
						}
					}
				}
			}
		restartC+=crashcount;
		if(restartC>n*50){
			restartC=0;
			series_shuffle(panel,n);
			crash=0;
			for(i=0;i<2*n-1;i++){
	  			diaX[i]=0;
	  			diaY[i]=0;
			}
			for(i=0;i<n;i++){
				diaX[panel[i]-i+n-1]++; 
				diaY[i+panel[i]]++;
			}
			for(i=0;i<2*n-1;i++){
				crash+=diaX[i]*(diaX[i]-1)/2;
				crash+=diaY[i]*(diaY[i]-1)/2;
			}
		}
	}
	free(diaX);
	free(diaY);
	return panel;
}
int fractal_self_multiply(int *panel,int n){
	int* panelx=(int *)malloc(n*n*sizeof(int));
	int i,j;
	for(i=0;i<n;i++)
		for(j=0;j<n;j++){
			panelx[i*n+j]=panel[i]*n+panel[j];
		}
	return panelx;
}
int residual_join(int * panel,int n,int res){
	if(res!=0){
	int *panelx=(int *)realloc(panel,(n+res)*sizeof(int));
	int *series=panelx+n;
	int i;
	for(i=0;i<res;i++)series[i]=n+i;
	series_shuffle(series,res);
	return panelx;
	}
	return panel;
}
int main(){
	int n=8;
	printf("Input for n:>");
	scanf("%d",&n);
	clock_t start,end; 
	start =clock();
	int sqrtn=(int)sqrt(n);
	int * panel=series_init(sqrtn);
	panel=forgePanel(panel,sqrtn);
	panel=fractal_self_multiply(panel,sqrtn);
	panel=residual_join(panel,sqrtn*sqrtn,n-sqrtn*sqrtn);
	panel=forgePanel(panel,n);
	end=clock();
	if(n<=100)printP(panel,100);
	printf("Time:%d ms\n",end-start);
	scanf("%d");
	return 114514;
}

