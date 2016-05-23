#ifndef _PPC_TREE_H_
#define _PPC_TREE_H_
struct pair
{
    int pre, post;
public:
    pair():pre(0),post(0) {}
    bool operator==(pair& b){
        return pre==b.pre && post == b.post;
    }
};
struct PP_code
{
    //pair order;
    int order_pre;
    int order_post;
    int count;
public:
    PP_code():count(0){}
    PP_code(int c):count(c){}
};
class PPC_tree
{
private:
    PP_code node;
    int value;
    PPC_tree** child;
public:
    static int freqcount;
    PPC_tree(){
        child = NULL;
        value = -1;
    }
    PPC_tree(int num){
        child = new PPC_tree*[num];
        for (int i = 0; i < num; i++) {
            child[i] = NULL;
        }
    }
    ~PPC_tree();
    void buildTree(const char* fileName,double thresh);
    void printTree();
    void traverseWithMark(int &pre, int &post);
    void buildList(PP_code *nlist);
    void deleteTree();
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