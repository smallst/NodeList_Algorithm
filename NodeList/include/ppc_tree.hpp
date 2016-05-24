#ifndef _PPC_TREE_H_
#define _PPC_TREE_H_
struct PP_code
{
    int count;
    int order_pre=0;
    int order_post=0;
    
    PP_code():count(0),order_pre(0),order_post(0){}
    PP_code(int c):count(c),order_pre(0),order_post(0){}
};
class PPC_Tree{
private:
    PP_code node;
    int value;
    PPC_Tree *child, *next, *father;
public:
    
    PPC_Tree(){
        child = NULL;
        next = NULL;
        father = NULL;
        value = -1;
    }
    PPC_Tree(int v){
        value = v;
        child  =NULL;
        next = NULL;
        father = NULL;
    }
    ~PPC_Tree();
    void buildTree(const char* fileName, double thresh, int freqdict[]);
    void traverseWithMark();
    void buildList(PP_code *nlist, int freqdict[]);
    void printNode();
    void printTree();
};
#endif