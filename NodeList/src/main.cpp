#include <iostream>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include "../include/ppc_tree.h"

const int LISTSIZE = sizeof(PP_code)*2100;
const int CODESIZE = sizeof(PP_code);
int threshold = 0; 

struct dictionary
{
    int key;
    int num;
    bool operator<(const dictionary &c) const { return num < c.num; }
    bool operator>(const dictionary &c) const { return num > c.num; }
};
void printNode_List(PP_code *list, int i);

int PPC_tree::freqcount;
void PPC_tree::buildTree(const char* fileName,double thresh, int freqdict[]){
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
            int index = i * LISTSIZE;
            nlist[index + nlist[index].count*CODESIZE] = child[i]->node;
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
int mergeNList(PP_code *nlist, int begina, int beginb, int dest){
    int lengtha = nlist[(long long)begina*LISTSIZE].count * CODESIZE;
    int lengthb = nlist[(long long)beginb*LISTSIZE].count * CODESIZE;
    long long enda = lengtha +(long long) begina*LISTSIZE;
    long long endb = lengthb +(long long) beginb*LISTSIZE;
    long long j = beginb*LISTSIZE + CODESIZE;
    long long begind = dest*LISTSIZE;
    nlist[begind] = PP_code(1);
    for (long long i = begina*LISTSIZE+CODESIZE; i < enda; i += CODESIZE) {
        while(j < endb){
            if(nlist[i].order.pre < nlist[j].order.pre)
            {
                if(nlist[i].order.post > nlist[j].order.post)
                {
                    nlist[begind + nlist[begind].count*CODESIZE] = nlist[i];
                    nlist[begind + nlist[begind].count*CODESIZE].count = nlist[j].count;
                    nlist[begind].count++;
                    j+= CODESIZE;
                }
                else
                    break;
            }
            else{
                j+= CODESIZE;
            }
        }
    }
    int count = 0;
    
    if(nlist[begind].count != 1){
        //printNode_List(nlist, dest);
        long long endd = begind + nlist[begind].count * CODESIZE;
        pair temp = nlist[begind+CODESIZE].order;
        int newcount = 1;
        for (long long i = begind+CODESIZE*2; i < endd; i+=CODESIZE) {
            if(nlist[i].order == temp){
                nlist[begind+newcount*CODESIZE].count += nlist[i].count;
            }
            else{
                temp = nlist[i].order;
                nlist[begind+(++newcount)*CODESIZE] = nlist[i];
            }
        }
        newcount ++;
        
        nlist[begind].count = newcount;
        endd = begind + newcount*CODESIZE;
        for (long long i = begind+CODESIZE; i < endd; i+= CODESIZE) {
            count += nlist[i].count;
        }
        
        // printNode_List(nlist, dest);
        // printf("===================\n");
 }
    if(count < threshold)
        return dest;
    else
    {
        totalcount ++;
        return dest+1;
    }
    
}
int mergeNodeList(PP_code *nlist, int begina, int beginb, int dest){
    int lengtha = nlist[begina*LISTSIZE].count * CODESIZE;
    int lengthb = nlist[beginb*LISTSIZE].count * CODESIZE;
    int enda = lengtha + begina*LISTSIZE;
    int endb = lengthb + beginb*LISTSIZE;
    int j = beginb*LISTSIZE + CODESIZE;
    int begind = dest*LISTSIZE;
    nlist[begind] = PP_code(1);
    for (int i = begina*LISTSIZE+CODESIZE; i < enda; i += CODESIZE) {
        while(j < endb){
            if(nlist[i].order.pre < nlist[j].order.pre
               && nlist[i].order.post > nlist[j].order.post)
            {
                nlist[begind + nlist[begind].count*CODESIZE] = nlist[j];
//                nlist[begind + nlist[begind].count*CODESIZE].count = nlist[j].count;
                nlist[begind].count++;
                if(j == endb-CODESIZE)
                    break;
                else
                    j+= CODESIZE;
            }
            else{
                break;
            }
        }
    }
    int endd = begind + nlist[begind].count * CODESIZE;
    int count = 0;
    for (int i = begind+CODESIZE; i < endd; i+= CODESIZE) {
        count += nlist[i].count;
    }
    nlist[begind].order.pre = count;
    if(count < threshold)
        return dest;
    else
    {
        totalcount ++;
        return dest+1;
    }
    
}
void DataMining(PP_code *nlist, int begin, int end){
    // NList
    for (int i = end - 1; i > begin; i--) {
        int dest = end;
        for (int j = begin ; j < i; j++) {
            //   printf("%d%d:\n",j,i) ;
            
            dest = mergeNList(nlist, j, i, dest);
        }
        DataMining(nlist, end, dest);
    }
    //NodeList
    /*
      for (int i = begin; i < end -1 ; i++) {
        int dest = end;
        for (int j = i+1; j < end; j++) {
            dest = mergeNodeList(nlist, i, j,dest);
        }
        DataMining(nlist, end, dest);
      }
    */
}
void printNode_List(PP_code *list, int i){
    int index = i * LISTSIZE;
    for (int j = 1; j < list[index].count; j++) {
        printf("<%d,%d>:%d, ",list[index + j*CODESIZE].order.pre,
               list[index + j*CODESIZE].order.post,
               list[index+j*CODESIZE].count);
    }
    printf("\n");
}
int main(int argc, char *argv[])
{
    int *freqdict = new int [1000];
    PPC_tree *root = new PPC_tree();
    double thresh = 0.6;
    root->buildTree("../data/accidents.dat", thresh, freqdict);
    delete[] freqdict;
    int listlength = PPC_tree::freqcount * (PPC_tree::freqcount+1) / 2;
    /*
    PP_code **Node_List = new PP_code*[listlength];
    for (int i =0; i < listlength; i++) {
        Node_List[i] = new PP_code[2100];
    }*/
    PP_code *Node_list = new PP_code[(long long)listlength * LISTSIZE];
    for (int i = 0; i < PPC_tree::freqcount; i++) {
        Node_list[i*LISTSIZE] = PP_code(1);
    }
    root->buildList(Node_list);
    
    for (int i = 0; i < PPC_tree::freqcount; i++) {
        //printNode_List(Node_list,i);
    }
    
    totalcount = PPC_tree::freqcount;
    printf("freqcount:%d\n",PPC_tree::freqcount);
    
    DataMining(Node_list, 0, PPC_tree::freqcount);
    
    //root->printTree();

    /*
    for (int i =0; i < listlength; i++) {
        delete[] Node_List[i];
    }*/
    printf("total:%d\n", totalcount);
    
    delete[] Node_list;
    return 0;
}
