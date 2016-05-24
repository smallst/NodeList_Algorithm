#include <iostream>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <stdlib.h>
#include "../include/ppc_tree.hpp"


const int LISTSIZE = 800;
int ORIGSIZE = 2800;
int threshold = 0; 
bool LLOG = false;
int totalcount = 0;
int freqcount;

void printNode_List(PP_code *list, int i);
int mergeNList(PP_code *nlist, long begina, long beginb, int dest, long addrdict[]){
    long enda = begina + nlist[begina].count ;
    long endb = beginb + nlist[beginb].count ;
    
    long j = beginb + 1;
    long begind = addrdict[dest];
    nlist[begind] = PP_code(1);
    for (long i = begina+1; i < enda; i ++) {
        while(j < endb){
            if(nlist[i].order_pre < nlist[j].order_pre)
            {
                if(nlist[i].order_post > nlist[j].order_post)
                {
                    int index = begind + nlist[begind].count;
                    if(nlist[index-1].order_pre == nlist[i].order_pre
                       && nlist[index-1].order_post == nlist[i].order_post)
                        nlist[index-1].count += nlist[j].count;
                    else
                    {
                        nlist[index].order_post = nlist[i].order_post;
                        nlist[index].order_pre = nlist[i].order_pre;
                        nlist[index].count = nlist[j].count;
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
        //      printNode_List(nlist, begind);
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
void Mining(PP_code *nlist, long addrdict[], int bbegin, int eend){
    int begin = bbegin, end = eend;
    int *mystack = new int [eend - bbegin+20];
    int len = 1;
    mystack[0] = begin;
    for (int i = eend - 1; i > bbegin ; i --) {
        mystack[len ++] = end;
        while(len > 1){
            int jb = begin, je = end -1;
            int dest  =end;
            while (je - jb > 0) {
                for (int j = jb; j < je; j ++) {
                    dest = mergeNList(nlist, addrdict[j], addrdict[je], dest, addrdict);
                }
                if(nlist[addrdict[je]].count == 2){
                    int n = dest - end;
                    totalcount += (1<<n) - n - 1;
                    len ++;
                    break;
                }
                else
                {
                    begin = end;
                    end = dest;
                    jb = begin;
                    je = end -1;
                    mystack[len ++] =end;
                }
            }
            len --;
            if(mystack[len-1] - mystack[len-2] > 1){
                begin = mystack[len-2];
                end = -- mystack[len-1];
            }
            else if(--len > 1){
                begin = mystack[len -2];
                end = mystack[len-1];
            }
        }
    }
    delete[] mystack;
}
void printNode_List(PP_code *list, int i){
    int index = i;
    for (int j = 1; j < list[index].count; j++) {
        printf("<%d,%d>:%d, ",list[index + j].order_pre,
               list[index + j].order_post,
               list[index+j].count);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("usage: %s filepath threshold\n",argv[0]);
        return 0;
    }

    const char * filepath = argv[1];
    double thresh = atof(argv[2]);
    
    PPC_Tree *root = new PPC_Tree();
    int freqdict[1000];
    root->buildTree(filepath, thresh, freqdict);
    
    PP_code *NList = new PP_code[freqcount*ORIGSIZE];	
    int listlength = freqcount * (freqcount-1) / 2;
    for (long long i = 0; i < freqcount; i++) {
        NList[i*ORIGSIZE] = PP_code(1);
    }
    root->buildList(NList, freqdict);
    
    long *listaddr = new long[listlength];
     
    PP_code *Node_list = new PP_code[freqcount*ORIGSIZE + listlength*LISTSIZE];
    memcpy(Node_list, NList, freqcount*ORIGSIZE*sizeof(PP_code));
    /*
    int mxlen = 0;
    for (int i = 0; i < freqcount; i++) {
        if(Node_list[i*ORIGSIZE].count > mxlen)
            mxlen = Node_list[i*ORIGSIZE].count;
    }
    printf("mxlen=%d\n",mxlen);
    */
    totalcount = freqcount;
    //printf("freqcount=%d\n",freqcount);
    
    listaddr[0] = freqcount * ORIGSIZE;
    for (int i = freqcount - 1; i > 0; i--) {
        int dest = 0;
        for (int j = 0 ; j < i; j++) {
            dest = mergeNList(Node_list, j*ORIGSIZE, i*ORIGSIZE, dest, listaddr);
        }
        Mining(Node_list, listaddr, 0, dest);
    }
    printf("total:%d\n", totalcount);
    delete[] Node_list;
    delete[] listaddr;
    delete[] NList;
    delete root;
    
    return 0;
}
