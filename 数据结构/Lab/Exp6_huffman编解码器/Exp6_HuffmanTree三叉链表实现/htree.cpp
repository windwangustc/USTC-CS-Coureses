#ifndef _HUFFMANTREE_CPP_
#define _HUFFMANTREE_CPP_
//哈夫曼树 ：分配连续存储空间存储树

#include <cstdio>
#include <cstdlib>

//编码的最大长度
#define MCL 30
//定义叶节点数,8bit的unsigned char
#define n 256
//定义结点总数，2n-1
#define m 511

//定义树结点，包括三个指针父节点，左右孩子
typedef struct _HTNode{
	int id;
	long weight;
    struct _HTNode* leftchild;
    struct _HTNode* rightchild;
    struct _HTNode* parent;
}HTNode,*HTree;
//哈夫曼编码表
typedef struct{
    unsigned code;  //前面都是0，最后len位是编码,unsigned占4字节
    int len;        //编码长度
} HCode;

/*** 以下代码哈夫曼树生成 ***/
//构造哈夫曼树/二叉树,输入长度为n=256的权向量w（每个字符出现的次数,ASCII最多256个字符），返回根节点指针
HTree CreateHTree(long *w){
	int numZero = 0;
	int nonZero_num=0;
    long allw=0;        //用于后面初始化min1、min2
    HTNode** temp = (HTNode**)malloc(m * sizeof(HTNode*));  //开辟空间，数组的数据类型为HTNode*
    //把所有节点创建好，忽略权值为0的结点，把这些节点的指针存放在temp辅助数组的前256个内，以便后面构建链表访问
    for (int i=0;i<n;i++){  //对前256个进行初始化存放
        if(w[i]!=0){
            temp[i]= (HTNode*)malloc(sizeof(HTNode));
            temp[i]->id=i;
            temp[i]->weight=w[i];
            temp[i]->leftchild = temp[i]->rightchild = temp[i]->parent = NULL;
            allw += w[i];           //统计所有的权值
            nonZero_num++;          //统计非0权值的字符数目
        }
        else {
            temp[i] = NULL;
            numZero++; //统计没出现的字符个数
        }
    }
    //下面执行Huffman树的构造
	int curNode_num=n;      //当前结点号
	while (curNode_num<n-1+nonZero_num){
        //用于Debug的输出
        /*printf("当前所有的节点数量为:%d\n", current_node_num);
        for (int i = 0; i < current_node_num; i++) {
            if (temp[i]) {
                printf("id = %d   data = %d\n",temp[i]->id,temp[i]->data);
            }
        }*/
        //初始化min1，min2，这样的话循环内可以直接访问min的data域而不会发生错误
        HTNode nodemin1; HTNode nodemin2;
        HTNode* min1 = &nodemin1; HTNode* min2 = &nodemin2;
        min1->weight = allw; min2->weight = allw;     //设置最大的权值w
        //先通过一次循环找到2个最小权值
        for (int i = 0; i < curNode_num; i++){
            if (temp[i]) {           //忽略权值为0的结点
                if (!temp[i]->parent) {     //未配对的结点
                    if (temp[i]->weight < min1->weight){
                        min2=min1;
                        min1=temp[i];
                    }//min1<min2,找到比min1更小的ht[j].weight，那么min1替代min2， ht[j].w替代min1
                    else if(temp[i]->weight >= min1->weight && temp[i]->weight < min2->weight){  //比min1大比min2小
                        min2=temp[i];
                    }
                }
            }
        }
        //内层for结束，找到min1,min2最小的两个权值，准备构造非叶节点,连接链表
        temp[curNode_num] = (HTNode*)malloc(sizeof(HTNode));
        temp[curNode_num]->id = curNode_num;
        temp[curNode_num]->weight = min1->weight + min2->weight;    //新结点权值相加
        temp[curNode_num]->leftchild = min1;
        temp[curNode_num]->rightchild =min2;
        temp[curNode_num]->parent = NULL;
        min1->parent=min2->parent=temp[curNode_num];
        curNode_num++;
	}
	return temp[curNode_num-1];//返回根结点指针
}

//在Huffman树上搜索一个叶子节点，并返回叶子节点的指针
HTree Searchnode(HTree t, unsigned char c,int flag_search) {
    //指示是否找到
    static HTree search_result = NULL;
    if (!flag_search&&t) {
        if (t->id == c) {
            search_result = t;
            Searchnode(t->leftchild, c, 1);
            Searchnode(t->rightchild, c,1);
        }
        Searchnode(t->leftchild, c,0);
        Searchnode(t->rightchild,c,0);
    }
    return search_result;
}

//给定huffman树的头结点，先序遍历打印Huffman树，用于测试
void PrintHTree(HTree t) {
    if (!t)
        return;
    else {
        printf("节点 id=%d data=%d\n", t->id, t->weight);
        PrintHTree(t->leftchild);
        PrintHTree(t->rightchild);
    }
}

/*** 以下代码分析文件和从文件中生成哈夫曼编码表 ***/
//从filename中二进制读取字符，统计每个字符出现的次数，写入w并返回
//w数组长度为256，如果字符不足256，那么没出现的字符权值为0
unsigned char *parseFile(const char filename[], long *w,long *fsize){
	FILE *fp = fopen(filename,"rb");
	if(fp == NULL){
        printf("无法打开文件!\n");
        exit(0);
    }
    fseek(fp,0,SEEK_END);
	*fsize = ftell(fp);
	rewind(fp);
	unsigned char *dataArray;
	dataArray = (unsigned char *)malloc(sizeof(unsigned char)*(*fsize));
	if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	fread(dataArray,sizeof(unsigned char),*fsize,fp); //读取待压缩文件
	fclose(fp);

	for (int i=0;i<n;++i) //权值清空
		w[i] = 0;

	for(int i=0;i<*fsize;++i) //权值+1，若对应字符出现
		w[dataArray[i]]++;      //dataArray为char型
	return dataArray;
}

//输入任何字符c，得到它的哈夫曼编码, 被genCodes()调用
void getCode(HTree t, int c,unsigned *code1, int *clen1){
	int clen = 0;        //编码长度
	unsigned code = 0;   //编码
    //接下来位操作
	int size = sizeof(code);       //编码的最大bits数
	unsigned mask = 1<<(size*8-1); //左移31位，使得最高位为1其它为0，用来设置最高位为1

    //printf("搜索结果的节点data=%d\n", Searchnode(t, c)->data);
    //找到字符c所在结点的父结点
    HTNode* parent = Searchnode(t,c,0)->parent;
	while(parent){
		code = code >> 1;           //右移一位
		parent->leftchild->id==c?code:code|= mask;  //如果是右孩子，那么最高位设置为1，否则就是0
		clen++;                     //每走一步编码长度+1
		c = parent->id;
		parent = parent->parent;
	}//不断寻找父节点，来得到哈夫曼编码

	//code的前面clen位是哈夫曼编码，将其移动到最后
	code=code>>(size*8-clen);       //右移，保留clen位
	//printf("0x%x-%d\n",code,clen);
	*code1 = code;  //返回值
	*clen1 = clen;  //返回值
}

//从哈夫曼树ht生成完整的编码表hc，输出hc为encode()的输入参数
//先序遍历生成编码表hc
void genHCodes(HCode hc[], HTree t){
    static HTree root_genHCodes = t;        //每次保存当前的根结点
    if (t) {                          //构造的Huffman树上不会存在weight=0的节点
        if (t->id <=n-1 && t->id >= 0) {
            getCode(root_genHCodes, t->id, &(hc[t->id].code), &(hc[t->id].len));
        }
        genHCodes(hc,t->leftchild);         //遍历左孩子
        genHCodes(hc, t->rightchild);       //遍历右孩子
    }
}

/*** 以下为编解码实现 ***/
//编码,原长olen的内容orgi，压缩编码后为长nlen的内容newc，需要利用编码表hc
void encode(unsigned char *orgi, long olen, unsigned char *newc, long *nlen, HCode hc[]){
	int sizec = sizeof(unsigned int);  //要确保字符编码的最大长度要小于sizec*8-8
	long i=0; //orgi的下标
	long j=0; //newc的下标
	newc[j] = 0;
	unsigned code;
	int len;
	int usedbits=0; //newc[j]中被占用的bits数
	while (i<olen){//对原始数据逐个字符处理
		int idx = orgi[i]; //获得hc下标
		code = hc[idx].code;
		len = hc[idx].len;
		while (len+usedbits>=8)
		{//code需要左移usedbits长的一段，然后和newc[j]位或操作
			unsigned code1 = code >> len+usedbits-8;
			newc[j] = newc[j] | code1; //填满前一个编码字符
			len = len+usedbits-8;      //code现在有效长度
			usedbits = 0;
			newc[++j]=0;
			//printf("\n编码的第 %d 个字符为：#%2x\n",j-1,newc[j-1]);
		}//循环，使得len+usedbits <8
		//将code的前面各位清0,因为有效位是len
		code = code & ((1<<len)-1); //code和后面k个1，其它全为0的数据进行与，保留后面len位
		code = code << 8-len-usedbits;//向前移动若干位
		newc[j] = newc[j] | code;
		usedbits = len+usedbits;
		i++;
	}
	*nlen = j+1;
}

//生成和保存压缩文件,指定文件名fout，将所用的哈夫曼树存入文件
//这一步与结构体数组实现最大的不同就是无法保存链表，因为涉及到地址，因此只保存权值数组，解压的时候现场构建Huffman树
void zip(const char fin[],const char fout[]){
    //读取分析待压缩文件的内容，即读取权值列表和文件大小两项
    long fsize;
    long wDist[n] = {0};
    unsigned char* content = parseFile(fin, wDist, &fsize);
    //根据权值列表生成Huffman树，返回的根存在root里
    HTree root;
    root= CreateHTree(wDist);
    //根据Huffman树生成完整的Huffman编码表，其中出现次数为0的跳过
    HCode hc[n];
    genHCodes(hc,root);

    //根据Huffman编码表对读取到的内容content进行编码
	unsigned char *zipContent; //编码后的内容
	long zipsize;//压缩后文件大小
    //压缩后的文件可能更大，考虑将fsize扩大一点
	zipContent = (unsigned char *)malloc(sizeof(unsigned char)*(fsize+10000));
	if (!zipContent) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
	//利用编码表编码
	encode(content,fsize,zipContent,&zipsize,hc); //编码后返回长度zipsize的内容zipContent

	FILE *fp = fopen(fout,"wb");
	if(fp == NULL){
        printf("无法打开写入文件!\n");
        exit(0);
    }
    //long ht_size = sizeof(HTNode)*m;      //哈夫曼编码表的大小
    //fwrite(&ht_size,sizeof(ht_size),1,fp);//保存哈夫曼树的大小

    fwrite(wDist,sizeof(long),n,fp);                 //保存权值列表，解压的时候可以用这个列表构建Huffman树
	fwrite(&zipsize,sizeof(zipsize),1,fp);    //保存编码内容的大小
	fwrite(&fsize,sizeof(fsize),1,fp);        //保存原始内容的大小
    fwrite(zipContent,sizeof(unsigned char),zipsize,fp);//保存编码后的内容
	fclose(fp);

	free(zipContent);	//释放文件内容
	printf("压缩文件 %s 已经生成！\n",fout);
}

//读取压缩文件，解压
//这一步对应上面zip()函数保存的内容，需要调用CreateHTree()函数来从权值列表创建Huffman树
void unzip(const char zfile[],const char ofile[]){
    //打开压缩文件zfile
	FILE *fp = fopen(zfile,"rb");
	if(fp == NULL){
        printf("无法打开压缩文件进行读取!\n");
        exit(0);
    }
    //按区域读取各数据,对应zip内的顺序是权值列表、压缩内容大小、原始内容大小、压缩内容
    long w[n];//权值列表
    long zipsize1, fsize1;
    HTree root; //树的根结点下标
    fread(w,sizeof(long),n,fp);                       //读取权值列表
    fread(&zipsize1, sizeof(zipsize1), 1, fp); //压缩后内容的大小
    fread(&fsize1, sizeof(fsize1), 1, fp);     //被压缩内容的大小

	unsigned char *zcontent = (unsigned char *)malloc(zipsize1);//存放读取的编码数据
	unsigned char *ocontent = (unsigned char *)malloc(fsize1);//解码后的数据

	fread(zcontent,zipsize1,1,fp); //读取编码数据
	fclose(fp);
	//根据读取到的权值重新构建Huffman树
    root = CreateHTree(w);
    //打开解压后文件，准备开始写入
	fp = fopen(ofile,"wb");
	if(fp == NULL){
        printf("无法打开解压后文件进行解压!\n");
        exit(0);
    }

    HTNode* current_node = root;//从根开始
	int j,k=0,i=0;      //zcontent的下标 k,ocontent的下标i
	while (i<fsize1){   //依次写入解压后数据的每个字节
		j =128; //10000000
		while (j>0){
			if ((zcontent[k] & j)>0){ //向右走 1
				if (!current_node->rightchild){ //走到头了，说明此时处于叶子节点
					ocontent[i++] = current_node->id;
                    current_node = root;//解码了下一个字符的第一个bit
					fprintf(fp,"%c",ocontent[i-1]);
					if (i>=fsize1) //解码结束
						break;
					j=j<<1;
				}else
					current_node=current_node->rightchild;  //还没走到叶子结点
			}else{//向左走 0
				if (!current_node->leftchild){
					ocontent[i++] = current_node->id;
					current_node = root;//解码了下一个字符的第一个bit
					fprintf(fp,"%c",ocontent[i-1]);
					if (i>=fsize1) //解码结束
						break;
					j=j<<1;
				}else
					current_node = current_node->leftchild;//还没走到叶子节点
			}
			j=j>>1; //j控制while循环8次，求出zcontent[k]的每一位
		}
		if (i>=fsize1) //解码结束
		    break;
		k++;//准备读取下一个字符
	}
	fclose(fp);
	free(ocontent);
	free(zcontent);
	printf("文件 %s 已经成功解压为 %s ！\n",zfile,ofile);
}
//计算压缩率，rate=sizeof(f1)/sizeof(f2)
double zip_rate(char* filein, char* fileout)
{
    FILE *fp1=fopen(filein, "r");
    FILE *fp2=fopen(fileout, "r");

    if(!fp1||!fp2) return -1;
    fseek(fp1,0L,SEEK_END);
    int size1=ftell(fp1);
    fclose(fp1);

    fseek(fp2,0L,SEEK_END);
    int size2=ftell(fp2);
    fclose(fp2);

    return 1.0*size2/size1;
}

int main(){
    //Huffman树测试部分
    long w_test[256] = { 0 };
    w_test[0] = 2; w_test[1] = 8; w_test[2] = 6; w_test[3] = 4; w_test[4] = 3; w_test[5] = 4;
    HTree root = NULL;
    root = CreateHTree(w_test);
    PrintHTree(root);       //先序遍历打印Huffman树

    //getCode()函数测试部分
    unsigned int code1; int clen1;
    getCode(root,5,&code1,&clen1);
    printf("code1 = %d  clen1 = %d ",code1,clen1);

    //生成完整Huffman编码表函数genHCode()测试部分
    //HCode list_test[n];
    //genHCodes(list_test,root);
    //printf("%d\n",list_test[1].code);
    //printf("%d\n",list_test[4].code);

	/** 读取文件，分析文件字符出现规律，构建哈夫曼树，生成哈夫曼编码表 **/
	char infile[]="data.txt";
	char zfile[256]="ChenWangLab05.lzip";
	char outfile[256]="dataout.txt";
    zip(infile,zfile);
    unzip(zfile,outfile);

/*    char infile[]="test.pdf";
    char zfile[256]="ChenWangLab05.lzip";
    char outfile[256]="testout.pdf";
    zip(infile,zfile);
    unzip(zfile,outfile);*/

    //测试压缩率，
    //不同类型的文件压缩比是不同的，哈夫曼压缩是无损压缩/平均码长最优编码，默认字符独立，不考虑文件字符的关联性，考虑关联性/有损压缩比可以更低。
    //用指针由于没有保存哈夫曼树和哈夫曼编码，而是保存权重数组现场恢复，因此获得了更高的压缩率，但是牺牲了时间效率。
    printf("\n压缩比为：%f",zip_rate(infile,zfile));

	return 0;
}

#endif
