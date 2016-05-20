struct pair
{
    int pre, post;
public:
    pair():pre(0),post(0) {}
};
struct PP_code
{
    pair order;
    int count;
public:
    PP_code():count(0){}
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
    void buildTree(const char* fileName, int freqdict[]);
    void printTree();
    void traverseWithMark(int &pre, int &post);
    void buildList(int length, int freqdict[]);
};
