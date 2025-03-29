#include <cstdint>
#include <new>
#include <stddef.h>  
#include <vector>



// #define MAX_SLOT_SIZE 512#这个没有规定类型，在后面比较的时候可能类型不匹配（此处是默认为int，后面与size_t需要转换）
// #define MIN_SLOT_SIZE 8
// #define MEMORY_POOL_NUM 64
constexpr size_t MAX_SLOT_SIZE = 512;  // 类型安全的常量。并且使用这个，能方便代码的维护
constexpr size_t MIN_SLOT_SIZE = 8;
constexpr size_t MEMORY_POOL_NUM = 64;


//对于自定义的类：SlotHead和BlockHead，其指针：Slot*和Block*的大小和普通指针一样；
//而对于这两个类本身，其类实例的大小（sizeof(Slot) 和 sizeof(Block)）取决于其成员变量和内存对齐规则
struct SlotHead {
    SlotHead* next;
};

struct BlockHead
{
    BlockHead* next;
};

class MemoryPool {

public:
    MemoryPool(size_t SlotSize) :
        SlotSize_(SlotSize),
        BlockSize_(4096),
        ptr_to_freeList(nullptr),
        ptr_to_firstBlock(nullptr),
        ptr_to_curSlot(nullptr),
        ptr_to_lastSlot(nullptr)
    {
    }

    ~MemoryPool() {
        //注意，此处只是把内存回收了，但是没有完成对象的析构
        //区分内存回收与对象析构：  内存回收：标记这块内存可以重新使用了
        //对象析构：比如我们在SlotHead后面跟着保存了一个string，其实只能保存一个指针，真实的数据是这个指针指向的heap位置，如果没有析构，那么就会造成堆上的那块内存泄漏
        BlockHead* cur = ptr_to_firstBlock;
        while (cur != nullptr) {
            BlockHead* next = cur->next;
            operator delete(reinterpret_cast<void*>(cur));//转化为void*，避免析构（内存池项目中，我们手动控制内存的释放和对象的析构，他们两个是分离的）
            cur = next;
        }

    }

public:
    size_t SlotSize_;
    size_t BlockSize_;
    SlotHead* ptr_to_freeList;//存的保存实际数据的地址,而不是SlotHead
    BlockHead* ptr_to_firstBlock;
    SlotHead* ptr_to_curSlot;//我们都是逐个往后遍历的
    SlotHead* ptr_to_lastSlot;//最后一个有效槽位的下一个地址,类似与迭代器end


    void* allocate() {//返回实际数据地址，void*能转化成任何指针类型
        if (ptr_to_freeList != nullptr) {
            SlotHead* tmp = ptr_to_freeList;
            ptr_to_freeList = ptr_to_freeList->next;
            return tmp + 1;
        }
        else {
            if (ptr_to_curSlot >= ptr_to_lastSlot) {//虽然我们书写代码的时候，还没弄清楚ptr_to_lastSlot怎么变化的，但是我们可以先这样直接使用着
                allocateNewBlock();
                //我们在开发的时候，一开始想到这里需要一个分配新的块的操作，但是不知道其具体行为，从这里的逻辑我们就可以去建构分配新块这个操作的逻辑和返回值了
                //我们在此处只需要想我们需要什么，那么就让这个分配新块的函数拥有什么能力，后面再实现它即可
                //此处我们想要的是：使用ptr_to_curSlot,那么我们就知道了，这个allocateNewBlock的函数需要实现的是把ptr_to_curSlot进行正确移动 
            }
            SlotHead* tmp = ptr_to_curSlot;
            ptr_to_curSlot = tmp + SlotSize_ / sizeof(SlotHead); //这是挪到下一个存储区的头那里（注意与+1操作（挪到自己的实际数据区那里相区分））
            return tmp + 1;
        }
    }


    template<typename T>
    void deallocate(T* p) {//p为实际数据的地址
        SlotHead* SlotHead_for_p = reinterpret_cast<SlotHead*>(p) - 1;//把p转化为SlotHead类型，方便指针回退(-1能回退sizeof(SlotHead*)的大小，刚好到头那里)
        SlotHead_for_p->next = ptr_to_freeList;
        ptr_to_freeList = SlotHead_for_p;//造就了freelist每个节点的值都是SlotHead的地址，而不是实际数据的地址
    }


    void allocateNewBlock() {//最开始的时候需要调用一下
        void* rawMemory = ::operator new(BlockSize_);//new会分配内存+调用构造函数；::operator new就是原始的手动分配内存
        BlockHead* ptr_to_newBlock = reinterpret_cast<BlockHead*>(rawMemory);
        ptr_to_newBlock->next = ptr_to_firstBlock;//头插法插入新的Block
        ptr_to_firstBlock = ptr_to_newBlock;
        char* body = reinterpret_cast<char*>(ptr_to_newBlock) + sizeof(BlockHead*);
        //然后我们需要通过pad操作，使得BlockHead进行填充对齐，让cur指针指向对齐之后的下一个位置
        //那么我们根据这个想法，我们需要引入一个填充函数，同样是我们此处想直接用，写完这个allocateNewBlock之后再去写那个填充函数
        size_t padding = pad(body, SlotSize_);
        ptr_to_curSlot = reinterpret_cast<SlotHead*>(body + padding);
        ptr_to_lastSlot = reinterpret_cast<SlotHead*>(reinterpret_cast<char*>(ptr_to_newBlock) + BlockSize_ - SlotSize_ + 1);
        //最后一个 Slot 的起始地址newBlock + BlockSize_ - SlotSize_ 即从内存块末尾回退一个Slot的大小，然后再+1即为最后一个SlotHead的下一个地址
    }//在写每一个功能的时候，需要思考对于各个实例变量的影响，比如这个allocateNewBlock我们本来是由于allocate中发现需要的，但是写到后面发现lastSlot那个也需要修改


    size_t pad(void* p, size_t require) {//void*表示允许函数接受任何指针类型的实参
        char* p_charType = reinterpret_cast<char*>(p);//转化为char类型的指针，方便后续进行计算（因为这样指针的+1，-1都是直接对应于1B，-1B）
        size_t offset = reinterpret_cast<uintptr_t>(p_charType) % require;//uintptr_t将指针转换为整数类型，避免直接对指针的值取模（C++标准未定义）
        return (offset == 0) ? 0 : require - offset;
    }


};





class HashBucket {

public:
    //此数据结构的意义：用于在useMemory_in_MemoryPool中快速查找到对应的内存池
    //此处我们一开始使用了静态数组：static MemoryPool pools[MEMORY_POOL_NUM];
    //但是有一个很麻烦的问题是：静态的变量要在类外面重新写一遍，但是如果我们写了，这个静态数组构建完毕之后，会自动尝试给每个要保存的成员（此处是MemoryPool）调用默认构造函数，这不是我们想要的
    //此处我们换成vector，它不会尝试给要保存的成员调用默认构造函数
    inline static std::vector<MemoryPool> pools;

public:
    static void initMemoryPool() {
        pools.reserve(MEMORY_POOL_NUM);  // 预分配空间（避免扩容）,性能与静态数组相当。此处只是分配空间，但不做任何的尝试去构造对象
        for (int i = 0;i < MEMORY_POOL_NUM;i++) {
            // pools[i]=MemoryPool((i+1)*MIN_SLOT_SIZE);  不能这样写，因为这样就是先构造，然后再使用拷贝/移动赋值运算符，如果MemoryPool没有定义这俩就会出错
            // 此处是最开始针对静态数组的写法：new(&pools[i]) MemoryPool((i+1)*MIN_SLOT_SIZE);这样子就是在已分配的内存上面直接构造，&pools[i]是取地址操作
            pools.emplace_back((i + 1) * MIN_SLOT_SIZE);//vector这样子能直接原地构造对象
        }
    }

    static void* findMemory_in_MemoryPool(size_t size) {//设为静态成员函数，这样可以直接通过类名调用，但是就无法访问非静态成员，因为没有this指针
        //找到size对应的MemoryPool: 1~8Byte：0号内存池，9~16Byte：1号内存池……，然后分配实际数据存储的位置
        int rest = size % 8;
        int idx;
        if (rest == 0) {
            idx = size / 8 - 1;
        }
        else {
            idx = size / 8;
        }
        return pools[idx].allocate();
    }



    static void freeMemory_in_MemoryPool(void* ptr, size_t size) {//由于deleteElement的设计，此处我们只需要完成内存的释放即可
        int rest = size % 8;
        int idx;
        if (rest == 0) {
            idx = size / 8 - 1;
        }
        else {
            idx = size / 8;
        }
        pools[idx].deallocate(ptr);
    }

    //这两个我们想作为全局的函数，而不是放在HashBucket内部，这个不大符合语义
    template<typename T, typename ...Args>
    friend T* newElement(Args... args);

    template<typename T>
    friend void deleteElement(T* p);

};


template<typename T, typename ...Args>
T* newElement(Args... args) {//我们要找到一个位置构造好这个对象，并返回这个位置的指针

    size_t theSize = sizeof(T);
    if (theSize > MAX_SLOT_SIZE) {
        return new T(args...);
    }
    else {
        T* p = reinterpret_cast<T*>(HashBucket::findMemory_in_MemoryPool(theSize));//我们在书写的时候，发现要把useMemory设为静态变量比较好，因为我们只想有一个实例
        new(p) T(args...);//findMemory只是找到内存池中的能存数据的位置，此处才是真正地构造对象
        return p;
    }

}

template<typename T>
void deleteElement(T* p) {//需要完成对象的析构+内存的释放
    p->~T();//完成对象的析构
    size_t theSize = sizeof(T);
    if (theSize <= MAX_SLOT_SIZE) {//size在这个范围的对象，我们才会分配到内存池
        HashBucket::freeMemory_in_MemoryPool(p, theSize);//freeMemory是完成内存池中的内存的释放，不涉及对象的析构
    }
    else {
        operator delete(p);
    }
}
