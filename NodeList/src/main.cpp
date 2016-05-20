#include <iostream>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include "../include/ppc_tree.h"

struct dictionary
{
    int key;
    int num;
    bool operator<(const dictionary &c) const { return num < c.num; }
    bool operator>(const dictionary &c) const { return num > c.num; }
};
int PPC_tree::freqcount;
void PPC_tree::buildTree(const char* fileName, int freqdict[]){
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
            while(line[i] != ' ' && line[i] != '\n'){
                num = num * 10 + line[i] -'0';
                i++;
            }
            database[linecount][++count] = num;
        }
        database[linecount++][0] = count + 1;
    }
    
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
    
    int tfreqcount = 0;
    for (int i=0; dict[i].num > linecount*0.001; i++) {
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
void PPC_tree::buildList(int length, int freqdict[]){
    PP_code* Node_List = new PP_code[length * length];
    
}
void PPC_tree::traverseWithMark(int &pre, int &post){
    node.order.pre = pre++;
    for (int i = 0; i < freqcount; i++) {
        if(child[i] != NULL)
            child[i] -> traverseWithMark(pre, post);
    }
    node.order.post = post++;
}
int main(int argc, char *argv[])
{
    int *freqdict = new int [1000];
    PPC_tree *root = new PPC_tree();
    root->buildTree("../data/T10I4D100K.dat", freqdict);
    /*int length=0;
    for (int i = 0; i < 1000; i++) {
        if(freqdict[i] !=-1){
            length ++;
        }
        }*/
    //int listlength = PPC_tree::freqcount *PPC_tree::freqcount/2;
    /*
    PP_code **Node_List = new PP_code*[listlength];
    for (int i =0; i < listlength; i++) {
        Node_List[i] = new PP_code[2100];
    }*/
    //PP_code *Node_list = new PP_code[listlength*2100];
    
    //root->buildList(length, freqdict);
    //root->printTree();

    printf("ram enough!\n");
    /*
    for (int i =0; i < listlength; i++) {
        delete[] Node_List[i];
    }*/

    //delete[] Node_List;
    delete[] freqdict;
    return 0;
}
