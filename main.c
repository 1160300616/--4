#include <stdlib.h>
#include <stdio.h>
#include "extmem-c\extmem.c"
#include <time.h>
#include <memory.h>
#include <assert.h>
#define Raddress 0
#define Saddress 100
#define Block_size 1
#include <math.h>
#define MapRSize 50
#define MapSSize 80
#define _BTREE_H
#define MIN_T 3
#define MAX_T (MIN_T * 2)

typedef struct BTreeNodedata BTreeNodedata;
typedef struct BTreeNodedata *BTreeNode;
typedef struct BTreedata BTreedata;
typedef struct BTreedata *BTree;

/*
 * B树结点结构体
 */
typedef struct
{
    int A;
    int B;
    int D;
}TypeT;
struct BTreeNodedata
{
 int n;	//关键字个数
 int leaf;
 int key[MAX_T - 1];	//关键字
 BTreeNode child[MAX_T];	//子结点
};

/*
 * B树的结构体
 */
struct BTreedata
{
 BTreeNode	root;	//B树的根结点
};

#define BTREE_NODE_SIZE sizeof(BTreeNodedata)
#define BTREE_SIZE sizeof(BTreedata)

BTreeNode  allocate_node();	//为结点分配空间
void btree_create(BTree tree);	//初始化树
void btree_search(BTreeNode node, int key);	//寻找关键字位置
void btree_split_child(BTreeNode node, int location);	//分裂子结点
void btree_insert_nonfull(BTreeNode node, int key);	//向未满结点插入关键字
void btree_insert(BTree tree, int key);	//向树插入关键字
void display_node(BTreeNode *node_first, int n);	//显示以结点node_first为父结点的树
void display_btree(BTree tree);	//显示整棵树
BTreeNode btree_minimum(BTreeNode node);	//以node为根结点，寻找最小关键字
BTreeNode btree_maximum(BTreeNode node);	//以node为根结点，寻找最大关键字
void btree_min(BTree tree);	//在整棵树中寻找最小关键字
void btree_max(BTree tree);	//在整棵树中寻找最大关键字
void btree_left(BTreeNode parent, BTreeNode node, BTreeNode othernode, int location);	//将父结点、右兄弟、该结点的关键字调整
void btree_right(BTreeNode parent, BTreeNode node, BTreeNode othernode, int location);	//将父结点、左兄弟、该结点的关键字调整
int btree_merge_child(BTreeNode parent, int location);	//合并子结点，并返回下降子结点的位置
void btree_delete_leaf(BTreeNode node, int location);	//删除叶子结点关键字
int btree_delete_node_in(BTreeNode r_node, int i);	//删除内结点关键字，并返回下降子结点的位置
void btree_delete_node(BTreeNode r_node, int key);	//删除以r_node为根结点的树中关键字
void btree_delete(BTree tree, int key);	//删除树中的关键字

BTreeNode allocate_node()
{
	BTreeNode node = (BTreeNode) malloc (BTREE_NODE_SIZE);

	return node;
}


void btree_create(BTree tree)
{
	BTreeNode r_node = allocate_node();
	(r_node)->n = 0;
	(r_node)->leaf = 1;
	(tree)->root = r_node;
}

void  btree_search(BTreeNode node, int key)
{

	int j = 0;

	/*
     * 遍历当前结点，寻找恰当的关键字，如果找到相等的关键字，返回结点并将关键字位置保存在location
     * 如果没找到相等结点，且该结点为叶子结点，则报错
     * 否则递归寻找
     */
	while(j < node->n && key > node->key[j])
		j++;
	if(j < node->n && key == node->key[j])
	{
		printf("the %d key's location is %d \n", key, j);
	}
	else if(node->leaf)
	{
		printf("error:there is no a key\n");
	}
	else  btree_search(node->child[j], key);
}


void btree_split_child(BTreeNode node, int location)
{
	/* 建立新的空结点 */
	BTreeNode newnode = allocate_node();
	BTreeNode childnode = node->child[location];

	int i = 0;

	/* 初始化空结点newnode，将子结点childnode的信息复制到新结点node中 */
	newnode->leaf = childnode->leaf;
	newnode->n = MIN_T - 1;

	/* 将子结点childnode后T-1个关键字复制到新结点中，并改变子结点的n值 */
	for(i = 0;i <= MIN_T - 2;i++)
		newnode->key[i] = childnode->key[i + MIN_T];

	childnode->n = MIN_T - 1;

	/* 如果子结点非叶子结点，则相应的将子结点的结点点复制到新结点中 */
	if(!childnode->leaf)
		for(i = 0;i <= MIN_T - 1;i++)
			newnode->child[i] = childnode->child[i + MIN_T];

	/* 将父结点对应的关键字以及子结点位置向后移动一位 */
	for(i = node->n;i > location;i--)
	{
		node->key[i] = node->key[i - 1];
		node->child[i+1] = node->child[i];
	}

	/* 为父结点增加新的关键字和子结点，并修改n值 */
	node->child[location + 1] = newnode;
	node->key[location] = childnode->key[MIN_T - 1];
	node->n = node->n + 1;

}


void btree_insert_nonfull(BTreeNode node, int key)
{
	int i = node->n - 1;

	if(node->leaf)
	{
		/* 该结点为叶子结点时，找到对应位置，将关键字插入，并对结点node做出修改 */
		while(i >=0 && key < node->key[i])
		{
			node->key[i+1] = node->key[i];
			i--;
		}

		node->key[i+1] = key;
		node->n = node->n + 1;
	}
	else
	{
		/* 非叶子结点时，查找对应子结点，判断其是否为满结点，是，则分裂，否递归插入 */
		while(i >=0 && key < node->key[i])
			i--;
		i++;
		if(node->child[i]->n == MAX_T - 1)
		{
			btree_split_child(node, i);
			if(key > node->key[i])
				i++;
		}

		btree_insert_nonfull(node->child[i], key);
	}
}


void btree_insert(BTree tree, int key)
{
	BTreeNode r_node = tree->root;

	if(r_node->n == MAX_T - 1)
	{
		BTreeNode r_node_new = allocate_node();

		r_node_new->leaf = 0;
		r_node_new->n = 0;
		r_node_new->child[0] = r_node;
		tree->root = r_node_new;
		btree_split_child(r_node_new, 0);
		btree_insert_nonfull(r_node_new, key);
	}
	else btree_insert_nonfull(r_node, key);
}

/*
 * 为了验证插入以及删除结果正确，添加输出函数
 * 输出以parent为父结点的子树的所有关键字
 * 这里将所有的同一层的结点放入到一个数组中，方便输出
 * 第一个参数node_first作为每一层结点数组的起始地址
 * n为该层结点数
 */
void display_node(BTreeNode *node_first, int n)
{
	int i = 0, j = 0, k = 0,all = 0;
	BTreeNode *node = node_first;

	/* 将该层的结点所有的关键字输出，不同结点以“  ”为分隔，每层以“$$”为分隔	*/
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < (*(node + i))->n; j++)
		{
			printf("%d ", (*(node + i))->key[j]);
		}
		all = all + (*(node + i))->n + 1;
		//printf(" %p ", *(node + i));
		printf("  ");
	}
	printf("$$\n");

	if(!(*node)->leaf)
	{
		BTreeNode nodes[all];
		i = 0;
		for(j = 0; j < n; j++)
		{
			for(k = 0; k <= (*(node + j))->n; k++)
			{
				nodes[i] = (*(node + j))->child[k];
				i++;
			}
		}
		display_node(nodes, all);
	}
}



void
display_btree(BTree tree)
{
	BTreeNode r_node = tree->root;

	display_node(&r_node, 1);
}

/*
 * 返回以node为根结点树的最小关键字的结点，关键字的位置肯定为0
 */
BTreeNode  btree_minimum(BTreeNode node)
{
	BTreeNode newnode = node;

	if(newnode->n < 1)
	{
		printf("this is null tree\n");
		return NULL;
	}

	if(node->leaf)
		return newnode;
	else
		newnode = btree_minimum(node->child[0]);

	return newnode;
}


/*
 * 返回以node为根结点树的最大关键字的结点，关键字的位置肯定为该结点的n-1值
 */
BTreeNode  btree_maximum(BTreeNode node)
{
	BTreeNode newnode = node;

	if(newnode->n < 1)
	{
		printf("this is null tree\n");
		return NULL;
	}

	if(node->leaf)
		return newnode;
	else
		newnode = btree_maximum(node->child[node->n]);

	return newnode;
}

/*
 * 输出整棵树的最小关键字
 */
void btree_min(BTree tree)
{
	BTreeNode r_node = tree->root;
	BTreeNode n_node = btree_minimum(r_node);

	printf("the min is %d\n", n_node->key[0]);
}

/*
 * 输出整棵树的最大关键字
 */
void btree_max(BTree tree)
{
	BTreeNode r_node = tree->root;
	BTreeNode n_node = btree_maximum(r_node);

	printf("the max is %d\n", n_node->key[n_node->n - 1]);
}

/*
 * 当下降的结点node的关键字个数为T-1时，
 * 为了满足下降过程中，遇到的结点的关键字个数大于等于T，
 * 对结点parent、node、othernode三个结点的关键字做调整。
 * 当node在other左侧时，即node的右结点时（父结点的右子结点）， * 在T+1位置，增加一个关键字，其值为父结点对应的关键字值，
 * 将父结点对应关键字值赋值为右子结点中的第一个关键字。
 * 将右子结点的关键字和子结点（如果有的话）向前移动一位
 * 修改右子结点以及该结点的n值
 */
void btree_left(BTreeNode parent, BTreeNode node, BTreeNode othernode, int location)
{
	int i = 0;
	node->key[node->n] = parent->key[location];
	parent->key[location] = othernode->key[0];

	for(i = 0; i <= othernode->n - 2; i++)
		othernode->key[i] = othernode->key[i + 1];

	if(!othernode->leaf)
	{
		node->child[node->n + 1] = othernode->child[0];
		for(i = 0; i <= othernode->n - 1; i++)
			othernode->child[i] = othernode->child[i + 1];

	}

	node->n = node->n + 1;
	othernode->n = othernode->n - 1;
}

/*
 * 当下降的结点node的关键字个数为T-1时，
 * 为了满足下降过程中，遇到的结点的关键字个数大于等于T，
 * 对结点parent、node、othernode三个结点的关键字做调整。
 * 当node在other右侧时，即node的左结点时（父结点的左子结点），
 * node结点的关键字和子结点（如果有的话）向后移动一位,
 * 在第一个位置增加一个关键字，其值为父结点对应的关键字值，
 * 将父结点对应关键字值赋值为左子结点中的最后一个关键字。
 * 修改左子结点和该结点的n值
 */
void btree_right(BTreeNode parent, BTreeNode node, BTreeNode othernode, int location)
{
	int i = 0;

	for(i = node->n - 1; i >= 0; i--)
		othernode->key[i+1] = othernode->key[i];

	node->key[0] = parent->key[location];
	parent->key[location] = othernode->key[othernode->n];

	if(!node->leaf)
	{
		node->child[0] = othernode->child[othernode->n + 1];
		for(i = othernode->n; i >= 0; i--)
			othernode->child[i + 1] = othernode->child[i];

	}

	node->n = node->n + 1;
	othernode->n = othernode->n - 1;
}


int btree_merge_child(BTreeNode parent, int location)
{
	int i;
	BTreeNode	lnode = NULL;
	BTreeNode	rnode = NULL;

	if(location == parent->n)
		location--;

	lnode = parent->child[location];
	rnode = parent->child[location + 1];

	/* 将父结点对应的关键字以及右兄弟所有的关键字复制该结点，同时修改左子的n值 */
	lnode->key[lnode->n] = parent->key[location];
	for(i = 0; i < rnode->n; i++)
	{
		lnode->key[MIN_T + i] = rnode->key[i];
		lnode->n++;
	}

	/* 如果有子结点同样复制到该结点 */
	if(!rnode->leaf)
		for(i = 0; i <= rnode->n; i++)
			lnode->child[MIN_T + i] = rnode->child[i];

	rnode->n= 0;
	lnode->n = MAX_T - 1;

	/* 对父结点相应的关键字和子结点位置发生变化 */
	for(i = location; i < parent->n - 1; i++)
	{
		parent->key[i] = parent->key[i + 1];
		parent->child[i + 1] = parent->child[i + 2];
	}

	/* 调整父结点的n值 */
	parent->n = parent->n - 1;
	rnode = NULL;

	return location;
}


void btree_delete_leaf(BTreeNode node, int location)
{
	int i = 0;

	for(i = location; i < node->n - 1; i++)
		node->key[i] = node->key[i + 1];

	node->n = node->n - 1;
}

/*
 * 删除该层数组坐标为i的关键字
 */
int btree_delete_node_in(BTreeNode r_node, int i)
{

	BTreeNode lnode = r_node->child[i];
	BTreeNode rnode = r_node->child[i + 1];
	int temp = 0;

	/*
	* 当前于该位置的关键字的左子结点关键字个数大于等于T时，
	* 寻找该位置的关键的前驱（左子结点的最大关键字）
	*/
	if(lnode->n >= MIN_T)
	{
		BTreeNode newnode = btree_maximum(lnode);
		temp = r_node->key[i];
		r_node->key[i] = newnode->key[newnode->n - 1];
		newnode->key[newnode->n - 1] = temp;
	}
   /*
	* 相反的，若右子结点符合条件，则找寻后继（即右子结点的最小关键字）
	*/
	else if(rnode->n >= MIN_T)
	{
		BTreeNode newnode = btree_minimum(rnode);
		temp = r_node->key[i];
		r_node->key[i] = newnode->key[0];
		newnode->key[0] = temp;

		i++;
	}
   /*
	* 当左右子结点都不符合条件，则合并两个子结点
	*/
	else	i = btree_merge_child(r_node, i);

	return i;
}


void btree_delete_node(BTreeNode r_node, int key)
{
	int i = 0;


	/* 寻找关键字位置，或者下降的子结点位置 */
	while(i < r_node->n && key > r_node->key[i])
		i++;

	/* 若再该层且为叶子结点删除结点，否则下降寻找结点删除 */
	if(i < r_node->n && key == r_node->key[i])
		if(r_node->leaf)
			btree_delete_leaf(r_node, i);
		else
		{
			i = btree_delete_node_in(r_node, i);

			btree_delete_node(r_node->child[i], key);
		}
	else
	{
		if(r_node->leaf)
			printf("there is no the key %d!!\n", key);
		else
		{
			if(r_node->child[i]->n >= MIN_T){
				btree_delete_node(r_node->child[i], key);}
			else
			{
				if(i > 0 && r_node->child[i - 1]->n >= MIN_T)
				{
					btree_right(r_node, r_node->child[i], r_node->child[i - 1], i);}
				else if(i < r_node->n && r_node->child[i + 1]->n >= MIN_T)
					btree_left(r_node, r_node->child[i], r_node->child[i + 1], i);
				else
					i = btree_merge_child(r_node, i);

				btree_delete_node(r_node->child[i], key);
			}
		}
	}
}

/*
 * 删除树内的关键字key，如果根结点为空，则替换根结点
 */
void btree_delete(BTree tree, int key)
{
 BTreeNode r_node = tree->root;
 btree_delete_node(r_node, key);
 if(tree->root->n == 0 && tree->root->leaf == 0)
   tree->root = tree->root->child[0];
}
typedef struct {
    int A;
    int B;
} TypeR;
typedef struct {
    int C;
    int D;
} TypeS;
typedef struct{
	int key;  //键
	int val;  //值
}DataType; //对基本数据类型进行封装，类似泛型
typedef struct{
	DataType data;
	struct HashNode *next;  //key冲突时，通过next指针进行连接
}HashNode;
typedef struct{
	int size;
	HashNode *table;
}HashMap,*hashmap;

DataType MapR[MapRSize];
DataType MapS[MapSSize];

typedef int KeyType;                //KeyType为关键字类型


void createMapR(Buffer *buf)
{
    int i,j;
    int val;
    TypeR *blk;
    DataType temp;
    int index =0;
    for(i=0;i<16;i++)
    {
        blk = readBlockFromDisk(Raddress+i*Block_size,buf);
        if(index==0)
        {
            temp.key = (blk)->A;
            temp.val = i;
            /*printf("%d,%d,%d\n",temp.key,temp.val,index); */
            MapR[index] = temp;
            index ++;
        }
        for(j=0;j<7;j++)
        {
            if((blk+j)->A!=temp.key||temp.val!=i)
            {

                temp.key = (blk+j)->A;
                temp.val = i;
                MapR[index] = temp;
                /* printf("%d,%d,%d\n",temp.key,temp.val,index); */
                index ++;
            }
        }
        freeBlockInBuffer(blk,buf);
    }
}
void createMapS(Buffer *buf)
{
    int i,j;
    int val;
    TypeS *blk;
    DataType temp;
    int index =0;
    for(i=0;i<32;i++)
    {
        blk = readBlockFromDisk(Saddress+i*Block_size,buf);
        if(index==0)
        {
            temp.key = (blk)->C;
            temp.val = i;
            /*printf("%d,%d,%d\n",temp.key,temp.val,index); */
            MapS[index] = temp;
            index ++;
        }
        for(j=0;j<7;j++)
        {
            if((blk+j)->C!=temp.key||temp.val!=i)
            {

                temp.key = (blk+j)->C;
                temp.val = i;
                MapS[index] = temp;
                /*printf("%d,%d,%d\n",temp.key,temp.val,index); */
                index ++;
            }
        }
        freeBlockInBuffer(blk,buf);
    }
}
void SelectRByIndex(Buffer *buf)
{
    int i,j,k;
    int address = 350;
    int index = 0;
    int block = 0;
    TypeR *blk;
    TypeR *result;
    result = getNewBlockInBuffer(buf);
    for(i=0;i<MapRSize;i++)
    {
        if(MapR[i].key==40)
        {
            blk = readBlockFromDisk(Raddress+MapR[i].val*Block_size,buf);
            for(j=0;j<7;j++)
            {
                if((blk+j)->A==40)
                {
                    (result+index)->A = (blk+j)->A;
                    (result+index)->B = (blk+j)->B;
                    printf("Find tuple:%d,%d\n",(blk+j)->A,(blk+j)->B);
                }
                if(index >6)
                {
                    writeBlockToDisk(result,address+block*Block_size,buf);
                    block++;
                    index = 0;
                    result = getNewBlockInBuffer(buf);
                }
            }
        }
    }
    writeBlockToDisk(result,address+block*Block_size,buf);
}
void SelectSByIndex(Buffer *buf)
{
    int i,j,k;
    int address = 400;
    int index = 0;
    int block = 0;
    TypeS *blk;
    TypeS *result;
    result = getNewBlockInBuffer(buf);
    for(i=0;i<MapSSize;i++)
    {
        if(MapS[i].key==60)
        {
            blk = readBlockFromDisk(Saddress+MapS[i].val*Block_size,buf);
            for(j=0;j<7;j++)
            {
                if((blk+j)->C==60)
                {
                    (result+index)->C = (blk+j)->C;
                    (result+index)->D = (blk+j)->D;
                    printf("Find tuple:%d,%d\n",(blk+j)->C,(blk+j)->D);
                }
                if(index >6)
                {
                    writeBlockToDisk(result,address+block*Block_size,buf);
                    block++;
                    index = 0;
                    result = getNewBlockInBuffer(buf);
                }
            }
        }
    }
    writeBlockToDisk(result,address+block*Block_size,buf);
}
TypeR *getRandomR(Buffer *buf)
{
    int i=0;
    int n=0;
    TypeR *blk; /* A pointer to a block */
    /*
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    } */
    srand((unsigned)time( NULL ) );
    for(n=0;n<16;n++)
    {
        if((blk = getNewBlockInBuffer(buf))==NULL)
        {
            printf("failed to get Block!\n");
        }
        for(i=0;i<7;i++)
        {
            (blk+i)->A=rand()%40+1;
            (blk+i)->B=rand()%1000+1;
        }
        int *next = (blk+7);

        *next = Raddress+Block_size*(n+1);
        if (writeBlockToDisk(blk, Raddress+Block_size*n, buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        /*
        if ((blk = readBlockFromDisk(*next-Block_size, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("\n");
        for (i = 0; i < 7; i++)
        {
            printf("%d,%d\n", (blk+i)->A,(blk+i)->B);
            freeBlockInBuffer(blk,&buf);
        }
        */
    }
}

TypeS *getRandomS(Buffer *buf)
{

    int i=0;
    int n=0;
    TypeS *blk; /* A pointer to a block */
    TypeS *head;
    /*
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    } */
    srand((unsigned)time( NULL ) );
    for(n=0;n<32;n++)
    {
        if((blk = getNewBlockInBuffer(buf))==NULL)
        {
            printf("failed to get Block!\n");
        }
        for(i=0;i<7;i++)
        {
            (blk+i)->C=rand()%41+20;
            (blk+i)->D=rand()%1000+1;
        }
        int *next = (blk+7);
        *next = Raddress+Block_size*(n+1);

        if (writeBlockToDisk(blk, Saddress+Block_size*n, buf) != 0)
        {
            perror("Writing Block Failed!\n");
            return -1;
        }
        /*
        if ((blk = readBlockFromDisk(*next-Block_size, &buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        printf("\n");
        for (i = 0; i < 7; i++)
        {
            printf("%d,%d,%d\n", n,(blk+i)->C,(blk+i)->D);
            freeBlockInBuffer(blk,&buf);
        }
        */
    }
}
void selectByOrder(Buffer *buf)
{
    int Rresult = 200;
    int Rindex = 0;
    int Sresult = 250;
    int Sindex = 0;
    TypeR *tempR;
    TypeR *blkR;
    TypeS *blkS;
    TypeS *tempS;
    int index = 0;
    int n=0;
    /*
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    } */
    if((tempR = getNewBlockInBuffer(buf))==NULL)
    {
        printf("failed to get Block!\n");
    }
    for(n=0;n<16;n++)
    {
        if ((blkR = readBlockFromDisk(Raddress+n*Block_size, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        int i=0;
        for (i = 0; i < 7; i++)
        {
            if((blkR+i)->A==40)
            {
                printf("Find tuple R(A,B): %d,%d\n",(blkR+i)->A,(blkR+i)->B);
                (tempR+index)->A = (blkR+i)->A;
                (tempR+index)->B = (blkR+i)->B;
                index ++;
                if(index>=7)
                {
                    if (writeBlockToDisk(tempR, Rresult+Block_size*Rindex, &buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    int *next = (blkR+7);
                    *next = Rresult+Block_size*(Rindex+1);
                    if((tempR = getNewBlockInBuffer(&buf))==NULL)
                    {
                        printf("failed to get Block!\n");
                    }
                    Rindex ++;
                    index = 0;
                }
            }
        }

        freeBlockInBuffer(blkR,buf);
    }
    int i=0;
    for(i=index;i<7;i++)
    {
        (tempR+index)->A = 0;
        (tempR+index)->B = 0;
    }
    if (writeBlockToDisk(tempR, Rresult+Block_size*Rindex, buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    if((tempS = getNewBlockInBuffer(buf))==NULL)
    {
        printf("failed to get Block!\n");
    }
    index = 0;
    for(n=0;n<32;n++)
    {
        if ((blkS = readBlockFromDisk(Saddress+n*Block_size, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (i = 0; i < 7; i++)
        {
            if((blkS+i)->C==60)
            {
                printf("Find tuple S(C,D): %d,%d\n",(blkS+i)->C,(blkS+i)->D);
                (tempS+index)->C = (blkS+i)->C;
                (tempS+index)->D = (blkS+i)->D;
                index ++;
                if(index>=7)
                {
                    if (writeBlockToDisk(tempS, Sresult+Block_size*Sindex, buf) != 0)
                    {
                        perror("Writing Block Failed!\n");
                        return -1;
                    }
                    int *next = (blkS+7);
                    *next = Sresult+Block_size*(Sindex+1);
                    if((tempS = getNewBlockInBuffer(buf))==NULL)
                    {
                        printf("failed to get Block!\n");
                    }
                    Sindex ++;
                    index = 0;
                }
            }
        }
        freeBlockInBuffer(blkS,buf);
    }

    for(i=index;i<7;i++)
    {
        (tempS+index)->C = 0;
        (tempS+index)->D = 0;
    }
    if (writeBlockToDisk(tempS, Sresult+Block_size*Sindex, buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
}
void sortS(TypeS *blk)
{
    int i=0;
    int j=0;
    TypeS temp;
    for(i=0;i<7;i++)
    {
        for(j=i;j<7;j++)
        {
            if((blk+i)->C>(blk+j)->C)
            {
                temp.C = (blk+i)->C;
                temp.D = (blk+i)->D;
                (blk+i)->C = (blk+j)->C;
                (blk+i)->D = (blk+j)->D;
                (blk+j)->C = temp.C;
                (blk+j)->D = temp.D;
            }
        }
    }
    return;
}
void sortR(TypeR *blk)
{
    int i=0;
    int j=0;
    TypeR temp;
    for(i=0;i<7;i++)
    {
        for(j=i;j<7;j++)
        {
            if((blk+i)->A>(blk+j)->A)
            {
                temp.A = (blk+i)->A;
                temp.B = (blk+i)->B;
                (blk+i)->A = (blk+j)->A;
                (blk+i)->B = (blk+j)->B;
                (blk+j)->A = temp.A;
                (blk+j)->B = temp.B;
            }
        }
    }
    return;
}
void MyMergeSortR(Buffer *buf)
{
    int m=0;
    int l=0;
    int k=0;
    int i=0;
    int j=0;
    int n=0;
    int Rresult =300;
    TypeR *blk1;
    TypeR *blk2;
    TypeR *temp1;
    TypeR *temp2;
    TypeR *tempBlock;

    TypeS *blkS1;
    TypeS *blkS2;
    TypeS *tempS1;
    TypeS *tempS2;
    TypeS *tempBlockS;
    int index = 0;
    tempBlock = getNewBlockInBuffer(buf);

    for(m=1;m<=1;m++)
    {

        for(n=0;n<16;n=n+(int)pow(2.0,m))
        {
                printf("%d\n\n",n);
                int round = (int)(pow(2.0,m)/2);

                blk1 = readBlockFromDisk(Raddress+n*Block_size,buf);
                blk2 = readBlockFromDisk(Raddress+(n+round)*Block_size,buf);
                i=0;
                j=0;
                temp1 = blk1 ;
                temp2 = blk2 ;
                int index1 = 0;
                int index2 = 0;
                while(index1<round&&index2<round)
                {

                    if(temp1->A > temp2->A)
                    {
                        (tempBlock+index)->A = temp2->A;
                        (tempBlock+index)->B = temp2->B;
                        j++;
                        index ++;
                        temp2 = blk2+j;
                    }
                    else
                    {
                        (tempBlock+index)->A = temp1->A;
                        (tempBlock+index)->B = temp1->B;
                        i++;
                        index++;
                        temp1 = blk1+i;
                    }

                    if(j>6)
                    {
                        freeBlockInBuffer(blk2,buf);
                        index2 ++;
                        if(index2<round)
                        {
                            blk2 = readBlockFromDisk(Raddress+(n+round+index2)*Block_size,buf);
                            j = 0;
                            temp2 = blk2;
                        }
                        else break;
                    }

                    if(i>6)
                    {
                        freeBlockInBuffer(blk1,buf);
                        index1 ++;
                        if(index1<round)
                        {
                            blk1 = readBlockFromDisk(Raddress+(n+index1)*Block_size,buf);
                            i = 0;
                            temp1 = blk1;
                        }
                        else break;
                    }
                    if(index>6)
                    {
                        for(k=0;k<7;k++)
                        {
                            printf("%d\n",(tempBlock+k)->A);

                        }
                        printf("\n");
                        writeBlockToDisk(tempBlock,Rresult,buf);
                        tempBlock = getNewBlockInBuffer(buf);
                        index = 0;

                    }
                }
                for(index2=index2;index2<round;index2++)
                {
                    for(j=j;j<7;j++)
                    {
                        (tempBlock+index)->A = (blk2+j)->A;
                        (tempBlock+index)->B = (blk2+j)->B;
                        index ++;
                        if(index>6)
                        {
                            for(k=0;k<7;k++)
                            {
                                printf("%d\n",(tempBlock+k)->A);

                            }
                            printf("\n");
                            writeBlockToDisk(tempBlock,Rresult,buf);
                            tempBlock = getNewBlockInBuffer(buf);
                            index =0;
                        }
                    }
                    j = 0;
                    if(index2+1<round)
                    {
                        freeBlockInBuffer(blk2,buf);
                        blk2 =readBlockFromDisk(Raddress+n+round+index2+1,buf);
                    }
                    else break;
                }
                for(index1=index1;index1<round;index1++)
                {
                    for(i=i;i<7;i++)
                    {
                        (tempBlock+index)->A = (blk1+i)->A;
                        (tempBlock+index)->B = (blk1+i)->B;
                        index ++;
                        if(index>6)
                        {
                            for(k=0;k<7;k++)
                            {
                                printf("%d\n",(tempBlock+k)->A);

                            }
                            printf("\n");
                            writeBlockToDisk(tempBlock,Rresult,buf);
                            tempBlock = getNewBlockInBuffer(buf);
                            index =0;
                        }
                    }
                    i = 0;
                    if(index1+1<round)
                    {
                        freeBlockInBuffer(blk1,buf);
                        blk2 =readBlockFromDisk(Raddress+n+index1+1,buf);
                    }
                    else break;
                }

                freeBlockInBuffer(temp1,buf);
                freeBlockInBuffer(temp2,buf);
            }

    }
}
void MergeSortR(Buffer *buf)
{
    int m=0;
    int l=0;
    int k=0;
    int i=0;
    int j=0;
    int n=0;
    int Rresult =300;
    TypeR *blk1;
    TypeR *blk2;
    TypeR *temp1;
    TypeR *temp2;
    TypeR *tempBlock;
    TypeS *blk3;
    TypeS *blk4;
    int index = 0;
    tempBlock = getNewBlockInBuffer(buf);
    int arr1[400];
    int arr2[400];
    int t =0;
    int temp;
    for(i=0;i<16;i++)
    {
        blk1 = readBlockFromDisk(Raddress+i*Block_size,buf);
        for(j=0;j<7;j++)
        {

            arr1[t] = (blk1+j)->A;
            arr2[t] = (blk1+j)->B;
            t++;
        }
        freeBlockInBuffer(blk1,buf);
    }
    for(i=0;i<16*7;i++)
    {
        for(j=i;j<16*7;j++)
        {
            if(arr1[i]>arr1[j])
            {
                temp = arr1[i];
                arr1[i] = arr1[j];
                arr1[j]=temp;
                temp = arr2[i];
                arr2[i] = arr2[j];
                arr2[j]=temp;
            }
        }
    }
    t=0;
    for(i=0;i<16;i++)
    {
        blk1 = getNewBlockInBuffer(buf);
        for(j=0;j<7;j++)
        {

            (blk1+j)->A=arr1[t];
            (blk1+j)->B=arr2[t];
            printf("%d,%d\n",(blk1+j)->A,(blk1+j)->B);
            t++;
        }
        writeBlockToDisk(blk1,Raddress+i*Block_size,buf);
        freeBlockInBuffer(blk1,buf);
    }

    blk1 = getNewBlockInBuffer(buf);
    freeBlockInBuffer(blk1,buf);

    t=0;
    for(i=0;i<32;i++)
    {
        blk3 = readBlockFromDisk(Saddress+i*Block_size,buf);
        for(j=0;j<7;j++)
        {

            arr1[t] = (blk3+j)->C;
            arr2[t] = (blk3+j)->D;
            t++;
        }
        freeBlockInBuffer(blk3,buf);
    }
    for(i=0;i<32*7;i++)
    {
        for(j=i;j<32*7;j++)
        {
            if(arr1[i]>arr1[j])
            {
                temp = arr1[i];
                arr1[i] = arr1[j];
                arr1[j]=temp;
                temp = arr2[i];
                arr2[i] = arr2[j];
                arr2[j]=temp;
            }
        }
    }
    t=0;
    printf("-----------------------\n");
    for(i=0;i<32;i++)
    {
        blk3 = getNewBlockInBuffer(buf);
        for(j=0;j<7;j++)
        {

            (blk3+j)->C=arr1[t];
            (blk3+j)->D=arr2[t];
            printf("%d,%d\n",(blk3+j)->C,(blk3+j)->D);
            t++;
        }
        writeBlockToDisk(blk3,Saddress+i*Block_size,buf);
        freeBlockInBuffer(blk3,buf);
    }
    /*    for(n=0;n<16;n=n+2)
        {
                printf("%d\n\n",n);
                blk1 = readBlockFromDisk(Raddress+n*Block_size,buf);
                blk2 = readBlockFromDisk(Raddress+(n+1)*Block_size,buf);
                i=0;
                j=0;
                temp1 = blk1 ;
                temp2 = blk2 ;
                int index1 = 0;
                int index2 = 0;
                while(i<7&&j<7)
                {

                    if(temp1->A > temp2->A)
                    {
                        (tempBlock+index)->A = temp2->A;
                        (tempBlock+index)->B = temp2->B;
                        j++;
                        index ++;
                        temp2 = blk2+j;
                    }
                    else
                    {
                        (tempBlock+index)->A = temp1->A;
                        (tempBlock+index)->B = temp1->B;
                        i++;
                        index++;
                        temp1 = blk1+i;
                    }
                    if(index>6)
                    {
                        for(k=0;k<7;k++)
                        {
                            printf("%d\n",(tempBlock+k)->A);

                        }
                        printf("\n");
                        writeBlockToDisk(tempBlock,Rresult,buf);
                        freeBlockInBuffer(tempBlock,buf);
                        tempBlock = getNewBlockInBuffer(buf);
                        index = 0;

                    }
                }
                for(j=j;j<7;j++)
                {
                        (tempBlock+index)->A = (blk2+j)->A;
                        (tempBlock+index)->B = (blk2+j)->B;
                        index ++;
                        if(index>6)
                        {
                            for(k=0;k<7;k++)
                            {
                                printf("%d\n",(tempBlock+k)->A);

                            }
                            printf("\n");
                            writeBlockToDisk(tempBlock,Rresult,buf);
                            freeBlockInBuffer(tempBlock,buf);
                            tempBlock = getNewBlockInBuffer(buf);
                            index =0;
                        }
                    }

                for(i=i;i<7;i++)
                {
                    (tempBlock+index)->A = (blk1+i)->A;
                    (tempBlock+index)->B = (blk1+i)->B;
                    index++;
                    if(index>6)
                    {
                        for(k=0;k<7;k++)
                        {
                            printf("%d\n",(tempBlock+k)->A);

                        }
                        printf("\n");
                        writeBlockToDisk(tempBlock,Rresult,buf);
                        freeBlockInBuffer(tempBlock,buf);
                        tempBlock = getNewBlockInBuffer(buf);
                        index = 0;
                    }
                }
                freeBlockInBuffer(temp1,buf);
                freeBlockInBuffer(temp2,buf);
        }
         */
}

int Binary_SearchR(Buffer *buf, int N, int keyword)
{
    int low = 0, high = N-1,mid;
    TypeR *blk;
    int blockN =0;
    int index =0;
    while(low <= high)
    {
        mid = (low + high)/2;
        blockN =(int) mid/7;
        index = mid % 7;
        blk = readBlockFromDisk(Raddress+blockN*Block_size,buf);
        if((blk+index)->A == keyword)
        {
            printf("Successful find tuple:%d,%d\n",(blk+index)->A,(blk+index)->B);
            freeBlockInBuffer(blk,buf);
            return mid;
        }
        if((blk+index)->A < keyword)
        {
            low = mid + 1;
        }
        else
        {
            high = mid -1;
        }
        freeBlockInBuffer(blk,buf);
    }
    printf("No suitable tuple!\n");
    return -1;
}
int Binary_SearchS(Buffer *buf, int N, int keyword)
{
    int low = 0, high = N-1,mid;
    TypeS *blk;
    int blockN =0;
    int index =0;
    while(low <= high)
    {
        mid = (low + high)/2;
        blockN =(int) mid/7;
        index = mid % 7;
        blk = readBlockFromDisk(Saddress+blockN*Block_size,buf);
        if((blk+index)->C == keyword)
        {
            printf("Successful find tuple:%d,%d\n",(blk+index)->C,(blk+index)->D);
            freeBlockInBuffer(blk,buf);
            return mid;
        }
        if((blk+index)->C < keyword)
        {
            low = mid + 1;
        }
        else
        {
            high = mid -1;
        }
        freeBlockInBuffer(blk,buf);
    }
    printf("No suitable tuple!\n");
    return -1;
}
void findAllR(Buffer *buf,int index,int keyword)
{
    int outputR = 300;
    int outSize = 0;
    int i,j,k,t;
    TypeR *blk;
    TypeR *output;
    int blockN =0;
    int index_temp =0;
    int Tindex = 0;
    blockN =(int) index/7;
    index_temp = index % 7;
    t = index_temp;
    int temp_block = blockN;
    int flag1 = 1;
    int flag2 = 1;
    blk = readBlockFromDisk(Raddress+blockN*Block_size,buf);
    output = getNewBlockInBuffer(buf);
    while(flag1)
    {
        for(i=t;i>=0;i--)
        {
            if((blk+i)->A==keyword)
            {
                (output+Tindex)->A = (blk+i)->A;
                (output+Tindex)->B = (blk+i)->B;
                printf("find tuple:%d,%d\n",(blk+i)->A,(blk+i)->B);
                Tindex ++;
            }
            else
            {
                flag1 = 0;
                break;
            }
            if(Tindex>6)
            {
                writeBlockToDisk(output,outputR+outSize,buf);
                outSize++;
                Tindex = 0;
                output = getNewBlockInBuffer(buf);
            }
        }
        temp_block --;
        t = 7;
        freeBlockInBuffer(blk,buf);
        if(temp_block>=0)
        blk = readBlockFromDisk(Raddress+temp_block*Block_size,buf);
        else
        {
            break;
        }
    }
    writeBlockToDisk(output,outputR+outSize,buf);
    outSize++;

    blk = readBlockFromDisk(Raddress+blockN*Block_size,buf);
    temp_block = blockN;
    t = index_temp;
    if(t+1<7)
    {
        t++;
    }
    else
    {
        t = 0;
        temp_block ++;
        freeBlockInBuffer(blk,buf);
        if(temp_block<16)
        blk = readBlockFromDisk(Raddress+temp_block*Block_size,buf);
        else
        {
            return ;
        }
    }
    while(flag2)
    {
        for(i=t;i<7;i++)
        {
            if((blk+i)->A==keyword)
            {
                (output+Tindex)->A = (blk+i)->A;
                (output+Tindex)->B = (blk+i)->B;
                printf("find tuple:%d,%d\n",(blk+i)->A,(blk+i)->B);
                Tindex ++;
            }
            else
            {
                flag2 = 0;
                break;
            }
            if(Tindex>6)
            {
                writeBlockToDisk(output,outputR+outSize,buf);
                outSize++;
                Tindex = 0;
                output = getNewBlockInBuffer(buf);
            }
        }
        temp_block ++;
        t = 0;
        freeBlockInBuffer(blk,buf);
        if(temp_block<16)
        blk = readBlockFromDisk(Raddress+temp_block*Block_size,buf);
        else
        {
            break;
        }
    }
    writeBlockToDisk(output,outputR+outSize,buf);
    outSize++;
}
void findAllS(Buffer *buf,int index,int keyword)
{
    int outputS = 350;
    int outSize = 0;
    int i,j,k,t;
    TypeS *blk;
    TypeS *output;
    int blockN =0;
    int index_temp =0;
    int Tindex = 0;
    blockN =(int) index/7;
    index_temp = index % 7;
    t = index_temp;
    int temp_block = blockN;
    int flag1 = 1;
    int flag2 = 1;
    blk = readBlockFromDisk(Saddress+blockN*Block_size,buf);
    output = getNewBlockInBuffer(buf);
    while(flag1)
    {
        for(i=t;i>=0;i--)
        {
            if((blk+i)->C==keyword)
            {
                (output+Tindex)->C = (blk+i)->C;
                (output+Tindex)->D = (blk+i)->D;
                printf("find tuple:%d,%d\n",(blk+i)->C,(blk+i)->D);
                Tindex ++;
            }
            else
            {
                flag1 = 0;
                break;
            }
            if(Tindex>6)
            {
                writeBlockToDisk(output,outputS+outSize,buf);
                outSize++;
                Tindex = 0;
                output = getNewBlockInBuffer(buf);
            }
        }
        temp_block --;
        t = 7;
        freeBlockInBuffer(blk,buf);
        if(temp_block>=0)
        blk = readBlockFromDisk(Saddress+temp_block*Block_size,buf);
        else
        {
            break;
        }
    }
    writeBlockToDisk(output,outputS+outSize,buf);
    outSize++;

    blk = readBlockFromDisk(Saddress+blockN*Block_size,buf);
    temp_block = blockN;
    t = index_temp;
    if(t+1<7)
    {
        t++;
    }
    else
    {
        t = 0;
        temp_block ++;
        freeBlockInBuffer(blk,buf);
        if(temp_block<32)
        blk = readBlockFromDisk(Raddress+temp_block*Block_size,buf);
        else
        {
            return ;
        }
    }
    while(flag2)
    {
        for(i=t;i<7;i++)
        {
            if((blk+i)->C==keyword)
            {
                (output+Tindex)->C = (blk+i)->C;
                (output+Tindex)->D = (blk+i)->D;
                printf("find tuple:%d,%d\n",(blk+i)->C,(blk+i)->D);
                Tindex ++;
            }
            else
            {
                flag2 = 0;
                break;
            }
            if(Tindex>6)
            {
                writeBlockToDisk(output,outputS+outSize,buf);
                outSize++;
                Tindex = 0;
                output = getNewBlockInBuffer(buf);
            }
        }
        temp_block ++;
        t = 0;
        freeBlockInBuffer(blk,buf);
        if(temp_block<32)
        blk = readBlockFromDisk(Saddress+temp_block*Block_size,buf);
        else
        {
            break;
        }
    }
    writeBlockToDisk(output,outputS+outSize,buf);
    outSize++;
}
BTree  createBtreeR(Buffer *buf)
{
    BTree tree = (BTree) malloc (BTREE_SIZE);
	tree->root	= (BTreeNode) malloc (BTREE_NODE_SIZE);
	int keys[40] ;
	int i = 0;
	int index =0;
	int j=0;
	TypeR *blk;
	for(i=0;i<16;i++)
    {
        blk = readBlockFromDisk(Raddress+i*Block_size,buf);
        if(index==0)
        {
            keys[0]=blk->A;
        }
        for(j=0;j<7;j++)
        {

            if(keys[index]!=(blk+j)->A)
            {
                index ++;
                keys[index] = (blk+j)->A;
            }
        }
        freeBlockInBuffer(blk,buf);
    }
	btree_create(tree);
	for(i = 0; i <= index; i++){
		btree_insert(tree, keys[i]);
		//display_btree(&tree);
		}
	btree_max(tree);
	btree_min(tree);
	return tree;
}
BTree  createBtreeS(Buffer *buf)
{
    BTree tree = (BTree) malloc (BTREE_SIZE);
	tree->root	= (BTreeNode) malloc (BTREE_NODE_SIZE);
	int keys[40] ;
	int i = 0;
	int index =0;
	int j=0;
	TypeS *blk;
	for(i=0;i<32;i++)
    {
        blk = readBlockFromDisk(Saddress+i*Block_size,buf);
        if(index==0)
        {
            keys[0]=blk->C;
        }
        for(j=0;j<7;j++)
        {

            if(keys[index]!=(blk+j)->C)
            {
                index ++;
                keys[index] = (blk+j)->C;
            }
        }
        freeBlockInBuffer(blk,buf);
    }
	btree_create(tree);
	for(i = 0; i <= index; i++)
    {
		btree_insert(tree, keys[i]);
		//display_btree(&tree);
    }
	btree_max(tree);
	btree_min(tree);
	return tree;
}
void SelectByBtree(Buffer *buf)
{
    createMapR(buf);
    createMapS(buf);
    BTree treeR = createBtreeR(buf);
	display_btree(treeR);
	btree_search(treeR->root,40);
	SelectRByIndex(buf);
    BTree treeS = createBtreeS(buf);
	display_btree(treeS);
	btree_search(treeS->root,60);
	SelectSByIndex(buf);
}
void ProjectionR(Buffer *buf)
{
    int address = 450;
    int i,j;
    int index=0;
    int block =0;
    TypeR *blk;
    TypeR *output;
    output = getNewBlockInBuffer(buf);
    for(i=0;i<16;i++)
    {
        blk = readBlockFromDisk(Raddress+i*Block_size,buf);
        for(j=0;j<7;j++)
        {
            (output+index)->A = (blk+j)->A;
            (output+index)->B = 0;
            index++;
        }
        if(index>6)
        {
            writeBlockToDisk(output,address+block*Block_size,buf);
            block++;
            index=0;
            output = getNewBlockInBuffer(buf);
        }
        freeBlockInBuffer(blk,buf);
    }
}
void SelectByBinary(Buffer *buf)
{
    int i=0;
    int j;
    TypeR *blkR;
    TypeS *blkS;
    for(i=0;i<16;i++)
    {
        blkR = readBlockFromDisk(Raddress+i*Block_size,buf);
        sortR(blkR);
        writeBlockToDisk(blkR,Raddress+i*Block_size,buf);
        freeBlockInBuffer(blkR,buf);
    }


    for(i=0;i<32;i++)
    {
        blkS = readBlockFromDisk(Saddress+i*Block_size,buf);
        sortS(blkS);
        writeBlockToDisk(blkS,Saddress+i*Block_size,buf);
        freeBlockInBuffer(blkS,buf);
    }
    MergeSortR(buf);
    int index = Binary_SearchR(buf,7*16,40);
    findAllR(buf,index,40);
    index = Binary_SearchS(buf,32*7,60);
    findAllS(buf,index,60);
}
void NextLoopJion(Buffer *buf)
{
    int address =500;
    int block = 0;
    int index = 0;
    int i,j,k,m,n;
    TypeR *blkR;
    TypeS *blkS;
    TypeT *blkT;
    blkT = getNewBlockInBuffer(buf);
    for(i=0;i<16;i++)
    {
        blkR = readBlockFromDisk(Raddress+i*Block_size,buf);
        for(j=0;j<32;j++)
        {
            blkS = readBlockFromDisk(Saddress+j*Block_size,buf);
            for(m=0;m<7;m++)
            {
                for(n=0;n<7;n++)
                {
                    if((blkR+m)->A==(blkS+n)->C)
                    {
                        (blkT+index)->A = (blkR+m)->A;
                        (blkT+index)->B = (blkR+m)->B;
                        (blkT+index)->D = (blkS+n)->D;
                        index++;
                        printf("连接(%d,%d)  (%d,%d)为(%d,%d,%d)\n",(blkR+m)->A,(blkR+m)->B,(blkS+n)->C,(blkS+n)->D,(blkR+m)->A,(blkR+m)->B,(blkS+n)->D);
                    }
                    if(index>4)
                    {
                        writeBlockToDisk(blkT,address+block*Block_size,buf);
                        block++;
                        index = 0;
                        blkT = getNewBlockInBuffer(buf);
                    }
                }
            }
            freeBlockInBuffer(blkS,buf);
        }
        freeBlockInBuffer(blkR,buf);
    }
    writeBlockToDisk(blkT,address+block*Block_size,buf);
}
void SortMergeJoin(Buffer *buf)
{
    int address =550;
    int block = 0;
    int index = 0;
    int indexR=0;
    int indexS=0;
    int i,j,k,m,n;
    i=0;
    j=0;
    TypeR *blkR;
    TypeS *blkS;
    TypeT *blkT;
    blkT = getNewBlockInBuffer(buf);
    blkR = readBlockFromDisk(Raddress,buf);
    blkS = readBlockFromDisk(Saddress,buf);
    while(i<16)
    {
        if((blkR+indexR)->A==(blkS+indexS)->C)
        {
            (blkT+index)->A = (blkR+indexR)->A;
            (blkT+index)->B = (blkR+indexR)->B;
            (blkT+index)->D = (blkS+indexS)->D;
            printf("连接(%d,%d) (%d,%d)为(%d,%d,%d)\n",(blkT+index)->A,(blkT+index)->B,(blkS+indexS)->C,(blkS+indexS)->D,(blkT+index)->A,(blkT+index)->B,(blkS+indexS)->D);
            index++;
            indexS++;
        }
        else if((blkR+indexR)->A < (blkS+indexS)->C)
        {
            indexR++;
            indexS = 0;
        }
        else
        {
            indexS++;
        }
        if(index>4)
        {
            writeBlockToDisk(blkT,address+block*Block_size,buf);
            block++;
            index = 0;
            blkT = getNewBlockInBuffer(buf);
        }
        if(indexR>6)
        {
            i++;
            if(i<16)
            {
                freeBlockInBuffer(blkR,buf);
                blkR = readBlockFromDisk(Raddress+i*Block_size,buf);
            }
            indexR = 0;
            freeBlockInBuffer(blkS,buf);
            blkS = readBlockFromDisk(Saddress,buf);
            j=0;
            indexS=0;
        }
        if(indexS>6)
        {
            if(j<31)
            {
                j++;
                freeBlockInBuffer(blkS,buf);
                blkS = readBlockFromDisk(Saddress+j*Block_size,buf);
            }
            else
            {
                j = 0;
                indexR++;
                indexS = 0;
                freeBlockInBuffer(blkS,buf);
                blkS = readBlockFromDisk(Saddress+j*Block_size,buf);
            }
            indexS = 0;
        }
    }
    writeBlockToDisk(blkT,address+block*Block_size,buf);
}
void HashJoin(Buffer *buf)
{
    int address[4] = {600,650,700,750};
    int address2[4] = {800,850,900,1000};
    int address3 = 2000;
    int block = 0;
    int index = 0;
    int i,j,k,m,n;
    i=0;
    j=0;
    TypeR *blkR;
    TypeS *blkS;

    TypeR *R1;
    TypeR *R2;
    TypeR *R3;
    TypeR *R0;

    TypeS *S1;
    TypeS *S2;
    TypeS *S3;

    TypeT *result;
    TypeS *S0;
    int HashR[4]={0};
    int indexR[4] = {0};
    int indexS[4] = {0};
    int HashS[4]={0};

    R0 = getNewBlockInBuffer(buf);
    R1 = getNewBlockInBuffer(buf);
    R2 = getNewBlockInBuffer(buf);
    R3 = getNewBlockInBuffer(buf);
    for(i=0;i<16;i++)
    {
        blkR = readBlockFromDisk(Raddress+i*Block_size,buf);
        for(j=0;j<7;j++)
        {
            switch((blkR+j)->A%4)
            {
            case 0:
                (R0+indexR[0])->A = (blkR+j)->A;
                (R0+indexR[0])->B = (blkR+j)->B;
                indexR[0]++;

                if(indexR[0]>6)
                {
                    writeBlockToDisk(R0,address[0]+HashR[0]*Block_size,buf);
                    HashR[0]++;
                    indexR[0] = 0;
                    R0 = getNewBlockInBuffer(buf);
                }
                break;
            case 1:
                (R1+indexR[1])->A = (blkR+j)->A;
                (R1+indexR[1])->B = (blkR+j)->B;
                indexR[1]++;

                if(indexR[1]>6)
                {
                    writeBlockToDisk(R1,address[1]+HashR[1]*Block_size,buf);
                    HashR[1]++;
                    indexR[1] = 0;
                    R1 = getNewBlockInBuffer(buf);
                }
                break;
            case 2:
                (R2+indexR[2])->A = (blkR+j)->A;
                (R2+indexR[2])->B = (blkR+j)->B;

                indexR[2]++;
                if(indexR[2]>6)
                {
                    writeBlockToDisk(R2,address[2]+HashR[2]*Block_size,buf);
                    HashR[2]++;
                    indexR[2] = 0;
                    R2 = getNewBlockInBuffer(buf);
                }
                break;
            case 3:
                (R3+indexR[3])->A = (blkR+j)->A;
                (R3+indexR[3])->B = (blkR+j)->B;
                indexR[3]++;
                if(indexR[3]>6)
                {
                    writeBlockToDisk(R3,address[3]+HashR[3]*Block_size,buf);
                    HashR[3]++;
                    indexR[3] = 0;
                    R3 = getNewBlockInBuffer(buf);
                }
                break;
            }
        }
        freeBlockInBuffer(blkR,buf);

    }
    printf("\n");
    writeBlockToDisk(R1,address[1]+HashR[1]*Block_size,buf);
    writeBlockToDisk(R2,address[2]+HashR[2]*Block_size,buf);
    writeBlockToDisk(R3,address[3]+HashR[3]*Block_size,buf);
    writeBlockToDisk(R0,address[0]+HashR[0]*Block_size,buf);

    block = 0;
    index = 0;
    S0 = getNewBlockInBuffer(buf);
    S1 = getNewBlockInBuffer(buf);
    S2 = getNewBlockInBuffer(buf);
    S3 = getNewBlockInBuffer(buf);
    int count = 0;
    for(i=0;i<32;i++)
    {
        blkS = readBlockFromDisk(Saddress+i*Block_size,buf);
        for(j=0;j<7;j++)
        {
            switch((blkS+j)->C%4)
            {
            case 0:
                (S0+indexS[0])->C = (blkS+j)->C;
                (S0+indexS[0])->D = (blkS+j)->D;
                indexS[0]++;

                if(indexS[0]>6)
                {
                    writeBlockToDisk(S0,address2[0]+HashS[0]*Block_size,buf);
                    HashS[0]++;
                    indexS[0]=0;
                    S0 = getNewBlockInBuffer(buf);
                }
                break;
            case 1:
                (S1+indexS[1])->C = (blkS+j)->C;
                (S1+indexS[1])->D = (blkS+j)->D;
                indexS[1]++;

                if(indexS[1]>6)
                {
                    writeBlockToDisk(S1,address2[1]+HashS[1]*Block_size,buf);
                    HashS[1]++;
                    indexS[1] = 0;
                    S1 = getNewBlockInBuffer(buf);
                }
                break;
            case 2:
                (S2+indexS[2])->C = (blkS+j)->C;
                (S2+indexS[2])->D = (blkS+j)->D;

                indexS[2]++;
                if(indexS[2]>6)
                {
                    writeBlockToDisk(S2,address2[2]+HashS[2]*Block_size,buf);
                    HashS[2]++;
                    indexS[2] = 0;
                    S2 = getNewBlockInBuffer(buf);
                }
                break;
            case 3:
                (S3+indexS[3])->C = (blkS+j)->C;
                (S3+indexS[3])->D = (blkS+j)->D;
                indexS[3]++;
                if(indexS[3]>6)
                {
                    writeBlockToDisk(S3,address2[3]+HashS[3]*Block_size,buf);
                    HashS[3]++;
                    indexS[3] = 0;
                    R3 = getNewBlockInBuffer(buf);
                }
                break;
            }
        }
        freeBlockInBuffer(blkS,buf);

    }
    printf("\n");
    index = 0;
    block = 0;
    writeBlockToDisk(S1,address2[1]+HashS[1]*Block_size,buf);
    writeBlockToDisk(S2,address2[2]+HashS[2]*Block_size,buf);
    writeBlockToDisk(S3,address2[3]+HashS[3]*Block_size,buf);
    writeBlockToDisk(S0,address2[0]+HashS[0]*Block_size,buf);
    result = getNewBlockInBuffer(buf);
    for(i=0;i<4;i++)
    {
        for(j=0;j<HashR[i];j++)
        {
            blkR = readBlockFromDisk(address[i]+j*Block_size,buf);
            for(k=0;k<7;k++)
            {
                for(n=0;n<HashS[i];n++)
                {
                    blkS = readBlockFromDisk(address2[i]+n*Block_size,buf);
                    for(m=0;m<7;m++)
                    {
                        if((blkS+m)->C==(blkR+k)->A)
                        {
                            (result+index)->A = (blkR+k)->A;
                            (result+index)->B = (blkR+k)->B;
                            (result+index)->D = (blkS+m)->D;
                            printf("(%d,%d,%d)\n",(blkR+k)->A,(blkR+k)->B,(blkS+m)->D);
                            index ++;
                        }
                        if(index>6)
                        {
                            writeBlockToDisk(result,address3+block*Block_size,buf);
                            block ++;
                            index = 0;
                            result = getNewBlockInBuffer(buf);
                        }
                    }
                    freeBlockInBuffer(blkS,buf);
                }
            }
            freeBlockInBuffer(blkR,buf);
        }
        printf("\n");
    }
}
int main(int argc, char **argv)
{
    Buffer buf; /* A buffer */
    TypeR R;
    TypeS S;
    TypeS *blk; /* A pointer to a block */
    int i = 0;
    int j=0;
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    }
    /*
    getRandomS(&buf);
    getRandomR(&buf);
    SelectByBinary(&buf); */
    /*createMapS(&buf);
    createMapR(&buf); */
    /*
    for(i=0;i<MapRSize;i++)
    {
        printf("%d,%d\n",MapR[i].key,MapR[i].val);
    } */
    /*
    SelectRByIndex(&buf);
    SelectSByIndex(&buf); */
    /*SelectByBtree(&buf); */
    /*ProjectionR(&buf); */

    /*NextLoopJion(&buf); */

    SortMergeJoin(&buf);
    HashJoin(&buf);
    freeBuffer(&buf);
    SelectByBtree(&buf);
    /*
    selectByOrder(&buf); */
    /*
    blk = readBlockFromDisk(Saddress,&buf);
    for(i=0;i<7;i++)
    {
        printf("%d\n",(blk+i)->A);
    }
    printf("\n");
    sortR(blk);
    for(i=0;i<7;i++)
    {
        printf("%d\n",(blk+i)->A);
    }  */
    /* Initialize the buffer */

    /*
    if (!initBuffer(20, 8, &buf))
    {
        perror("Buffer Initialization Failed!\n");
        return -1;
    } */

    /* Get a new block in the buffer */
    /* blk = getNewBlockInBuffer(&buf); */

    /* Fill data into the block */
    /*for (i = 0; i < 8; i++)
        *(blk + i) = 'a' + i; */

    /* Write the block to the hard disk */
    /*if (writeBlockToDisk(blk, 31415926, &buf) != 0)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    */
    /* Read the block from the hard disk */
    /*
    if ((blk = readBlockFromDisk(31415926, &buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }
    */
    /* Process the data in the block */
    /*
    for (i = 0; i < 8; i++)
        printf("%c", *(blk + i));

    printf("\n");
    printf("# of IO's is %d\n", buf.numIO); /* Check the number of IO's */

    return 0;
}

