#include<stdio.h>
#include<time.h>
int* series_init(int n){//初始化1-n数组 
	int* series=(int*)malloc(n*sizeof(int));
	int i;
	for(i=0;i<n;i++){
		series[i]=i;
	}
	return series;
}
void swap(int* a,int* b){//交换值 
	int c=*a;
	*a=*b;
	*b=c;
}
void series_shuffle(int* series,int n){//随机化数组 
	int i;
	for(i=0;i<n;i++){
		swap(&series[i],&series[randInt(i)]);
	}
}
int randInt(int i){//随机大整数(0-i) 
	return ((rand()<<15)+rand())%(i+1);
}
int printP(int * panel,int n){//打印棋盘(使用外置java程序) 
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
int printPs(int * panel,int n){//按字符打印棋盘 
	int i,j;
	for(i=0;i<n;i++){
	   for(j=0;j<n;j++){
	   	if(j==panel[i])printf("1");
	   	else printf("0");
		}
		printf("\n");
	}
}

int main(){
	int n=8;
	printf("Input for n:>");
	scanf("%d",&n);
	int *diaX;//对角线一 
	int *diaY;//对角线二,记录对角线上的棋子个数 
	clock_t start,end; //计时 
	diaX=(int *)malloc((2*n-1)*sizeof(int));
	diaY=(int *)malloc((2*n-1)*sizeof(int));
	int i,j,p,q;
	int crash=0;//对角线上的碰撞对数计数 
	int restartC=0;//重启次数计数 
	int deltax=0;//临时碰撞改变 
	int crashcount;//对角线上存在一个以上点的位置的计数 
	start =clock();
	int *panel=series_init(n);//初始化棋盘,棋盘用长度为n的数组,每行填入唯一的0~n-1的数字表示棋子位置,这样就没有行列间的碰撞,只要调整对角线上的碰撞 
	series_shuffle(panel,n);//随机初始化棋盘 
	for(i=0;i<2*n-1;i++){//初始化对角线计数 
	   diaX[i]=0;
	   diaY[i]=0;
	}
	for(i=0;i<n;i++){//计算对角线上棋子个数
	 	diaX[panel[i]-i+n-1]++; 
		diaY[i+panel[i]]++;
	}
	for(i=0;i<2*n-1;i++){//计算对角线碰撞对数 
		crash+=diaX[i]*(diaX[i]-1)/2;
		crash+=diaY[i]*(diaY[i]-1)/2;
	}
	while(crash>0){//一直调整 ,直至下降到碰撞对数为0 
		crashcount=0;
		for(p=0;p<n;p++)
			if(diaX[panel[p]-p+n-1]>1||diaY[panel[p]+p]>1)
			{
				crashcount++;
				for(q=n-1;q>=0;q--){
					if(q!=p){//用对角线计算碰撞临时改变 
						deltax=0;
						deltax-=diaX[panel[p]-p+n-1]-1;
						deltax-=diaY[panel[p]+p]-1;
						deltax-=diaX[panel[q]-q+n-1]-1;
						deltax-=diaY[panel[q]+q]-1;
						deltax+=diaX[panel[q]-p+n-1];
						deltax+=diaY[panel[q]+p];
						deltax+=diaX[panel[p]-q+n-1];
						deltax+=diaY[panel[p]+q];
						if(panel[p]+q==panel[q]+p||panel[p]+p==panel[q]+q)deltax+=2;//如果p,q棋子本身就在一个对角线上,这里会有一个2的相差,补齐 
						if(deltax<0){//如果下降了就应用交换 
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
		if(restartC>n*50){//如果多次未成功,则随机初始化(事实上只在较小规模时可能触发) 
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
	end=clock();
	if(n<=100)printP(panel,n);//对较小棋盘打印结果 
	printf("Time:%d ms\n",end-start);
	scanf("%d",&i);
	return 114514;
}

