#include <iostream>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <regex.h>
#include "../include/ppc_tree.hpp"


const int LISTSIZE = 800;
int ORIGSIZE = 2800;
int threshold = 0; 
bool LLOG = false;
int totalcount = 0;
int freqcount;
int *FreqDict = NULL;
FILE *outputlog = NULL;

// LOG to FILE
void printRESrec(int dest){
    int pre = FreqDict[dest<<1],post = FreqDict[dest*2+1];
    while(FreqDict[pre<<1] != -1){
        pre = FreqDict[pre << 1];
    }
    fprintf(outputlog,"%d-",FreqDict[pre*2+1]);
    if(FreqDict[post<<1] != -1){
        printRESrec(post);
    }
    else
        fprintf(outputlog,"%d-",FreqDict[post*2+1]);
}
void printRES(int begin, int end, int count){
    for (int i = end -1; i > begin; i--) {
        for (int j = begin; j < i; j++) {
            int pre = j;
            int index = i+1+j-begin;
            FreqDict[index<<1] = j;
            FreqDict[index*2+1] = i;
            while(FreqDict[pre*2] != -1){
                pre = FreqDict[pre*2];
            }
            fprintf(outputlog,"%d-",FreqDict[pre*2+1]);
            printRESrec(i);
            fseek(outputlog,-1,SEEK_CUR);
            fprintf(outputlog,":%d\n",count);
        }
        printRES(i+1, i *2 - begin +1,count);
    }
}

// LOG show Nlist
void printNList(PP_code *list, int i){
    int index = i;
    for (int j = 1; j < list[index].count; j++) {
        printf("<%d,%d>:%d, ",list[index + j].order_pre,
               list[index + j].order_post,
               list[index+j].count);
    }
    printf("\n");
}

// try merge NList, if new list is useful, return dest+1
int mergeNList(PP_code *nlist, int ba, int bb, int dest, long addrdict[]){
    long begina = addrdict[ba];
    long beginb = addrdict[bb];
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
                    // find father node
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
    // count = support count
    int count = 0;
    long endd = begind + nlist[begind].count ;
    for (long i = begind+1; i < endd; i++) {
        count += nlist[i].count;
    }
    if(count < threshold)
        // useless
        return dest;
    else
    {
        if(LLOG){
            FreqDict[dest << 1] = ba;
            FreqDict[(dest<<1)+1] = bb;
            printRESrec(dest);
            fseek(outputlog,-1,SEEK_CUR);
            fprintf(outputlog,":%d\n",count);
        }
        addrdict[dest+1] = endd;
        totalcount ++;
        return dest+1;
    }
}
// DataMining with PrePost (non-recursion)
void Mining(PP_code *nlist, long addrdict[], int bbegin, int eend){
    int begin = bbegin, end = eend;
    int *mystack = new int [eend - bbegin+20];
    int len = 1;
    // stack for emulate recursion
    mystack[0] = begin;
    for (int i = eend - 1; i > bbegin ; i --) {
        mystack[len ++] = end;
        while(len > 1){
            int jb = begin, je = end -1;
            int dest  =end;
            while (je - jb > 0) {
                // try merge NList
                for (int j = jb; j < je; j ++) {
                    dest = mergeNList(nlist, j, je, dest, addrdict);
                }
                // single path property
                if(nlist[addrdict[je]].count == 2){
                    int n = dest - end;
                    if(LLOG){
                        printRES(end, dest,nlist[addrdict[je]+1].count);
                    }
                    totalcount += (1<<n) - n - 1;
                    len ++;
                    break;
                }
                else
                {
                    // emulate recursion
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
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("usage: %s filepath threshold [LOG]\n    [LOG] 'T' for output to file\n",argv[0]);
        return 0;
    }else if(argc ==4){
        if(argv[3][0] == 'T')
        {
            LLOG = true;
            int len1 = 0;
            for (int i = 0; argv[1][i]!='\0'; i++) {
                len1 ++;
            }
            int len2 = 0;
            for (int i = 0; argv[2][i]!='\0'; i++) {
                len2 ++;
            }
            char filename[len1+len2];
            int i =0;
            while(argv[1][i] != '.'||argv[1][i+1] != 'd')
            {
                filename[i] = argv[1][i];
                i++;
            }
            for (int j =0 ; j < len2; j++) {
                filename[i++] = argv[2][j];
            }
            filename[i] = '.';
            filename[i+1] = 't';
            filename[i+2] = 'x';
            filename[i+3] = 't';
            filename[i+4] = '\0';
            printf("output log to %s\n",filename);
            outputlog = fopen(filename,"w");
        }
    }

    clock_t start, end;
    
    const char * filepath = argv[1];
    double thresh = atof(argv[2]);
    regex_t reg;
    regmatch_t pm[1];
    if(!regcomp(&reg, "accidents\.dat",REG_EXTENDED))
    {
        if(!regexec(&reg,filepath,1,pm,0)){
            ORIGSIZE = 80000;
            //printf("change ORIGSIZE\n");
        }
        regfree(&reg);
    }
    PPC_Tree *root = new PPC_Tree();
    int freqdict[1000];

    start = clock();
    // build PPC_Tree 
    root->buildTree(filepath, thresh, freqdict);
    // prepare NList
    PP_code *NList = new PP_code[freqcount*ORIGSIZE];	
    int listlength = freqcount * (freqcount-1) / 2;
    for (long i = 0; i < freqcount; i++) {
        NList[i*ORIGSIZE] = PP_code(1);
    }
    // generate 1-itemset
    root->buildList(NList, freqdict);
    // prepare actual NList(called Node_list is a mistake)
    long *listaddr = new long[freqcount + listlength];
    PP_code *Node_list = new PP_code[freqcount*ORIGSIZE + listlength*LISTSIZE];
    memcpy(Node_list, NList, freqcount*ORIGSIZE*sizeof(PP_code));

    for (int i = 0; i < freqcount; i++) {
        listaddr[i] = i * ORIGSIZE;
    }
    if(LLOG){
        for (int i = 0; i < freqcount; i++) {
            FreqDict[i<<1] = -1;
            int index = i*ORIGSIZE,count = 0;
            for (int j = 1; j < Node_list[index].count; j++) {
                count += Node_list[index+j].count;
            }
            fprintf(outputlog,"%d:%d\n",FreqDict[i*2+1],count);
        }
    }
    // 1-itemset nums
    totalcount = freqcount;
    listaddr[freqcount] = freqcount * ORIGSIZE;
    // DataMining
    Mining(Node_list, listaddr, 0,freqcount);
    // output result
    printf("total:%d\n", totalcount);
    
    end = clock();
    printf("time: %.3f s\n", (double)(end-start)/CLOCKS_PER_SEC);
    
    delete[] Node_list;
    delete[] listaddr;
    delete[] NList;
    delete root;
    
    return 0;
}
