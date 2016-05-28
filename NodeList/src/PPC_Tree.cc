#include <cstring>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include "../include/ppc_tree.hpp"


extern int threshold;
extern int freqcount;
extern int* FreqDict;
extern bool LLOG;
extern int ORIGSIZE;

// for sort
struct dictionary{
    int key;
    int num;
    bool operator<(const dictionary &c) const {return num < c.num;}
    bool operator>(const dictionary &c) const {return num > c.num;}
};
PPC_Tree::~PPC_Tree(){
    if(child != NULL){
        delete child;
    }
    if(next != NULL)
        delete next;
}
// build tree with database
void PPC_Tree::buildTree(const char* fileName, double thresh, int freqdict[]){
    FILE* stream;
    char line[200];
    int linecount = 0;
    dictionary *dict = new dictionary[1000];

    for (int i = 0; i < 1000; i++) {
        dict[i].key = i;
        dict[i].num = 0;
        freqdict[i] = -1;
    }
    // first, count freq
    stream = fopen(fileName,"r");
    while(fgets(line, 200, stream)){
        for (int i = 0; line[i] != '\0'; i++) {
            int num = 0;
            while(line[i] != ' ' && line[i]!= '\n' &&line[i] != '\0'){
                num = num * 10 + line[i] - '0';
                i ++;
            }
            if(num != 0)
                dict[num].num ++;
        }
        linecount++;
    }
    fclose(stream);
    // sort freq
    std::sort(dict, dict+1000, std::greater<dictionary>());

    int ffreqcount = 0;
    threshold = linecount * thresh;
    for (int i = 0; dict[i].num >= threshold; i++) {
        freqdict[dict[i].key] = i;
        ffreqcount ++;
    }
    freqcount = ffreqcount;
    if(LLOG){
        FreqDict = new int [(freqcount+1)*freqcount];
        for (int i = 0; i < freqcount; i++) {
            FreqDict[(i<<1)+1] = dict[i].key;
        }
    }
    // second, build lcrs tree
    stream = fopen(fileName, "r");
    int sortline[100];
    while(fgets(line, 200, stream)){
        int len = 0;
        for (int i =0; line[i] != '\0'; i++) {
            int num = 0;
            while(line[i] !=' ' && line[i] != '\n' && line[i] !='\0'){
                num = num * 10 + line[i] -'0';
                i++;
            }
            if(freqdict[num] > -1){
                sortline[len ++] = num;
            }
        }
        std::sort(sortline, sortline+len,[freqdict](int a,int b){return freqdict[a] < freqdict[b];});
        PPC_Tree* temp = this;
        for (int i = 0; i < len; i++) {
            if(temp -> child == NULL){
                temp->child = new PPC_Tree(sortline[i]);
                temp->child ->father = temp;
                temp = temp -> child;
                temp->node.count++;
            }
            else{
                temp = temp->child;
                while(temp->value != sortline[i])
                {
                    if(temp->next != NULL)
                        temp = temp->next;
                    else
                    {
                        temp->next = new PPC_Tree(sortline[i]);
                        temp->next->father = temp->father;
                        temp = temp->next;
                    }
                }
                temp->node.count++;
            }
        }
    }
    // mark pre & post order
    traverseWithMark();
}
// non-recursion traverse
void PPC_Tree::traverseWithMark(){
    int pre = 1,post = 1;
    PPC_Tree *temp = this;
    while(true){
        temp->node.order_pre = pre ++;
        while(temp->child !=NULL){
            temp = temp->child;
            temp->node.order_pre = pre ++;
        }
        temp->node.order_post = post++;
        if(temp->next != NULL){
            temp = temp->next;
        }else if(temp->father !=NULL){
            while(temp->father != NULL && temp->father->next == NULL)
            {
                temp = temp->father;
                temp ->node.order_post = post++;
            }
            if(temp->father == NULL)
                break;
            else{
                temp->father->node.order_post = post++;
                temp = temp->father->next;
            }
        }else
            break;
    }
}
// non-recursion print tree, for test
void PPC_Tree::printTree(){
    PPC_Tree *temp = this;
    while(true){
        temp->printNode();
        while(temp->child !=NULL){
            temp = temp->child;
            temp->printNode();
        }
        if(temp->next != NULL){
            temp = temp->next;
        }else if(temp->father !=NULL){
            while(temp->father != NULL && temp->father->next == NULL)
            {
                temp = temp->father;
            }
            if(temp->father == NULL)
                break;
            else{
                temp = temp->father->next;
            }
        }else
            break;
    }
    printf("\n");
}
// build 1-itemset Nlist
void PPC_Tree::buildList(PP_code *nlist, int freqdict[]){
    PPC_Tree *temp = this->child;
    int index = 0;
    while(true){
        index = freqdict[temp->value]*ORIGSIZE;
        nlist[index + nlist[index].count] = temp->node;
        nlist[index].count ++;
        
        while(temp->child !=NULL){
            temp = temp->child;
            index = freqdict[temp->value]*ORIGSIZE;
            nlist[index + nlist[index].count] = temp->node;
            nlist[index].count ++;
        }
        if(temp->next != NULL){
            temp = temp->next;
        }else if(temp->father !=NULL){
            while(temp->father != NULL && temp->father->next == NULL)
            {
                temp = temp->father;
            }
            if(temp->father == NULL)
                break;
            else{
                temp = temp->father->next;
            }
        }else
            break;
    }
}
void PPC_Tree::printNode(){
    printf("<%d,%d>:%d,",node.order_pre,node.order_post,node.count);
}