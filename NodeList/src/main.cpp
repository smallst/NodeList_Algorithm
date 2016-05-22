#include <iostream>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include "../include/ppc_tree.h"

const int LISTSIZE = 800;
const int ORIGSIZE = 2100;
const int CODESIZE = 1;//sizeof(PP_code);
int threshold = 0; 
bool LLOG = false;

struct dictionary
{
    int key;
    int num;
    bool operator<(const dictionary &c) const { return num < c.num; }
    bool operator>(const dictionary &c) const { return num > c.num; }
};
void printNode_List(PP_code *list, int i);

int PPC_tree::freqcount;
PPC_tree::~PPC_tree(){
    if(child != NULL){
        for (int i =0; i < freqcount; i++) {
            if(child[i] != NULL)
                delete child[i];
            child[i] = NULL;
        }
        delete[] child;
    }
    
}

void PPC_tree::deleteTree(){
    for (int i = 0; i < freqcount; i++) {
        if(child[i] != NULL)
        {
            child[i]->deleteTree();
            delete child[i];
        }
    }
    if(child != NULL)
    {
        delete[] child;
        child =NULL;
    }
}
void PPC_tree::buildTree(const char* fileName,double thresh){

    int freqdict[1000];
    FILE *stream;
    char line[200];
    stream = fopen(fileName,"r");
    int **database = new int *[400000];
    for (int i = 0; i < 400000; i++) {
        database[i] = new int [100];
    }
    int linecount = 0;
    while(fgets(line, 200, stream)){
        int  count = 0;
        for (int i=0; line[i] != '\0'; i++) {
            int num = 0;
            while(line[i] != ' ' && line[i] != '\n' &&line[i] !='\0'){
                num = num * 10 + line[i] -'0';
                i++;
            }
            if(num != 0)
                database[linecount][++count] = num;
        }
        database[linecount++][0] = count + 1;
    }
    /*
    printf("=============================database===========\n");
    for (int i = 0; i< linecount; i++) {
        for (int j = 1; j < database[i][0]; j++) {
            printf("%d,",database[i][j]);
        }
        printf("\n");
        
    }
    printf("===============================================\n");
    */
    dictionary *dict = new dictionary[1000];
    for (int i = 0; i < 1000; i++) {
        dict[i].key = i;
        dict[i].num = 0;
        freqdict[i] = -1;
    }
    for (int i = 0; i < linecount; i++) {
        for (int j  = 1; j < database[i][0]; j ++) {
            dict[database[i][j]].num ++;
        }
    }
    std::sort(dict, dict+1000, std::greater<dictionary>());
    /*
    for (int i = 0; dict[i].num > 0; i++) {
        printf("%d:%d,",dict[i].key,dict[i].num);
        
    }
    printf("\n");
    */
    int tfreqcount = 0;
    threshold = linecount * thresh;
    printf("%d\n",threshold);
    
    for (int i = 0; dict[i].num >= threshold; i++) {
        freqdict[dict[i].key] = i;
        tfreqcount ++;
    }
    
    printf("%d\n",tfreqcount);
    freqcount = tfreqcount;
    for (int i = 0; i < linecount; i++) {
        int l = 0;
        for (int j  = 1; j < database[i][0]; j ++) {
            if(freqdict[database[i][j]] != -1)
                database[i][++l] = database[i][j];
        }
        database[i][0] = l+1;
    }
    for (int i = 0; i < linecount; i++) {
        std::sort(&database[i][1], &database[i][0]+database[i][0], [freqdict](int a,int b){return freqdict[a] < freqdict[b];});
    }

    child = new PPC_tree*[freqcount];
    for (int i = 0; i < linecount; i++) {
        PPC_tree * temp = this;
        for (int j = 1; j < database[i][0]; j++) {
            //printf(":%c",'A'-1+database[i][j]);
            if(temp->child[freqdict[database[i][j]]] == NULL)
                temp->child[freqdict[database[i][j]]] = new PPC_tree(freqcount);
            temp = temp->child[freqdict[database[i][j]]];
            temp->value = database[i][j];
            temp->node.count ++;
        }
    }
    int pre = 1, post = 1;
    traverseWithMark(pre, post);
    delete[] dict;
    for (int i = 0; i < 400000; i++) {
        delete[] database[i];
    }
    delete[] database;
}
void PPC_tree::printTree(){
    printf("%d<(%d,%d):%d>\n", value,node.order.pre,node.order.post,node.count);
    for (int i = 0; i < freqcount; i++) {
        if(child[i] != NULL){
            child[i]->printTree();
        }
    }
}
void PPC_tree::buildList(PP_code *nlist){
    for (int i = 0; i < freqcount; i++) {
        if(child[i] != NULL)
        {
            int index = i * ORIGSIZE;
            nlist[index + nlist[index].count] = child[i]->node;
            nlist[index].count++;
            child[i] -> buildList(nlist);
        }
    }
}
void PPC_tree::traverseWithMark(int &pre, int &post){
    node.order.pre = pre++;
    for (int i = 0; i < freqcount; i++) {
        if(child[i] != NULL)
            child[i] -> traverseWithMark(pre, post);
    }
    node.order.post = post++;
}
int totalcount = 0;
int mergeNList(PP_code *nlist, long begina, long beginb, int dest, long addrdict[]){
    long enda = begina + nlist[begina].count ;
    long endb = beginb + nlist[beginb].count ;
    
    long j = beginb + 1;
    long begind = addrdict[dest];
    nlist[begind] = PP_code(1);
    for (long i = begina+1; i < enda; i ++) {
        while(j < endb){
            if(nlist[i].order.pre < nlist[j].order.pre)
            {
                if(nlist[i].order.post > nlist[j].order.post)
                {
                    if(nlist[begind + (nlist[begind].count-1)].order == nlist[i].order)
                        nlist[begind+ (nlist[begind].count-1)].count += nlist[j].count;
                    else
                    {
                        nlist[begind + nlist[begind].count].order = nlist[i].order;
                        nlist[begind + nlist[begind].count].count = nlist[j].count;
                        nlist[begind].count++;
                     }
                    j++;
                }
                else
                    break;
            }
            else{
                j++;
            }
        }
    }
    int count = 0;
    long endd = begind + nlist[begind].count ;
    for (long i = begind+1; i < endd; i++) {
        count += nlist[i].count;
    }
    if(LLOG){
        printf("%d",nlist[begind].count);
        printf("::::::::::::::::::::%d\n",count);
        printNode_List(nlist, begind);
        printf("===================\n");
    }
    if(count < threshold)
        return dest;
    else
    {
        addrdict[dest+1] = endd;
        totalcount ++;
        return dest+1;
    }
}
void DataMining(PP_code *nlist, long addrdict[], int begin, int end){
    // NList
    for (int i = end - 1; i > begin; i--) {
        int dest = end;
        for (int j = begin ; j < i; j++) {
            dest = mergeNList(nlist, addrdict[j], addrdict[i], dest, addrdict);
        }
        if(nlist[addrdict[i]].count == 2){
            int n = dest - end;
            totalcount += (1<<n) -n - 1;
        }
        else
            DataMining(nlist, addrdict, end, dest);
    }
}
void printNode_List(PP_code *list, int i){
    int index = i;
    for (int j = 1; j < list[index].count; j++) {
        printf("<%d,%d>:%d, ",list[index + j].order.pre,
               list[index + j].order.post,
               list[index+j].count);
    }
    printf("\n");
}
int main(int argc, char *argv[])
{
    PPC_tree *root = new PPC_tree();
    //double thresh = 0.005;
    double thresh =0.001 * (argv[1][0]-'0');
    root->buildTree("../data/T10I4D100K.dat", thresh);
    int listlength = PPC_tree::freqcount * (PPC_tree::freqcount-1) / 2;
    /* 
    PP_code *Node_list = new PP_code[(PPC_tree::freqcount*ORIGSIZE + listlength*LISTSIZE)];
    long *listaddr = new long[listlength];
    
    for (int i = 0; i < PPC_tree::freqcount; i++) {
        Node_list[i*ORIGSIZE] = PP_code(1);
    }
    root->buildList(Node_list);
    root->deleteTree();*/
    	PP_code *Nde_list = new PP_code[PPC_tree::freqcount*ORIGSIZE/CODESIZE];	
    long *listaddr = new long[listlength];
    
printf("list build\n");
    for (long long i = 0; i < PPC_tree::freqcount; i++) {
        Nde_list[i*ORIGSIZE] = PP_code(1);
    }
    root->buildList(Nde_list);
    root->deleteTree();
    PP_code *Node_list = new PP_code[PPC_tree::freqcount*ORIGSIZE + listlength*LISTSIZE];
    memcpy(Node_list, Nde_list, PPC_tree::freqcount*ORIGSIZE*sizeof(PP_code));
 
    int mxlen = 0;
    for (int i = 0; i < PPC_tree::freqcount; i++) {
        if(Node_list[i*ORIGSIZE].count > mxlen)
           mxlen = Node_list[i*ORIGSIZE].count;
        if(LLOG)
            printNode_List(Node_list,i*ORIGSIZE);
    }
    printf("mxlen=%d\n",mxlen);
    
    totalcount = PPC_tree::freqcount;
    listaddr[0] = PPC_tree::freqcount * ORIGSIZE;
    for (int i = PPC_tree::freqcount - 1; i > 0; i--) {
        int dest = 0;
        for (int j = 0 ; j < i; j++) {
            dest = mergeNList(Node_list, j*ORIGSIZE, i*ORIGSIZE, dest, listaddr);
        }
        DataMining(Node_list, listaddr, 0, dest);
    }
    printf("total:%d\n", totalcount);
    
    delete[] Node_list;
    delete[] listaddr;
    return 0;
}
