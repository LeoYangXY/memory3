#include "memory3.h"
#include <cstdint>
#include <new>
#include <stddef.h>  
#include <vector>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdint>
#include <new>
#include <stddef.h>  
#include <vector>
#include <cstdint>
#include <new>
#include <vector>
#include <chrono>
#include <iostream>
#include <random>
#include<string.h>

#include <cstdint>
#include <new>
#include <stddef.h>  
#include <vector>
#include <cassert>
#include <cstdint>
#include <new>
#include <stddef.h>  
#include <vector>

// ========== 测试配置 ==========
const size_t TEST_TIMES = 1000000;  // 测试循环次数
const size_t SMALL_SIZE = 2;       // 小对象大小
const size_t LARGE_SIZE = 600;      // 大对象大小

// ========== 测试对象定义 ==========
struct SmallObj {
    char data[SMALL_SIZE];  // 64字节
    void fill() {
        for (size_t i = 0; i < SMALL_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

struct LargeObj {
    char data[LARGE_SIZE];  // 512字节
    void fill() {
        for (size_t i = 0; i < LARGE_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

// ========== 测试用例 ==========

// 测试1：连续分配/释放小对象
void TestSmallObject() {
    std::cout << "\n=== 测试1：小对象(" << SMALL_SIZE << "字节)连续分配/释放 ===\n";

    // 内存池测试
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        SmallObj* p = newElement<SmallObj>();
        p->fill();
        deleteElement(p);
    }
    auto pool_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    // 系统默认测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        SmallObj* p = new SmallObj;
        p->fill();
        delete p;
    }
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "内存池耗时: " << pool_time.count() << " ms\n";
    std::cout << "系统默认: " << sys_time.count() << " ms\n";
    std::cout << "性能提升: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

// 测试2：连续分配/释放大对象
void TestLargeObject() {
    std::cout << "\n=== 测试2：大对象(" << LARGE_SIZE << "字节)连续分配/释放 ===\n";

    // 内存池测试
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        LargeObj* p = newElement<LargeObj>();
        p->fill();
        deleteElement(p);
    }
    auto pool_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    // 系统默认测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        LargeObj* p = new LargeObj;
        p->fill();
        delete p;
    }
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "内存池耗时: " << pool_time.count() << " ms\n";
    std::cout << "系统默认: " << sys_time.count() << " ms\n";
    std::cout << "性能提升: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

// 测试3：混合大小对象分配
void TestMixedObjects() {
    std::cout << "\n=== 测试3：混合大小对象分配 ===\n";

    std::mt19937 gen(42);
    std::uniform_int_distribution<> size_dist(1, 1024);

    // 内存池测试
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        size_t size = size_dist(gen);
        if (size <= 256) {
            SmallObj* p = newElement<SmallObj>();
            p->fill();
            deleteElement(p);
        }
        else {
            LargeObj* p = newElement<LargeObj>();
            p->fill();
            deleteElement(p);
        }
    }
    auto pool_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    // 系统默认测试
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        size_t size = size_dist(gen);
        if (size <= 256) {
            SmallObj* p = new SmallObj;
            p->fill();
            delete p;
        }
        else {
            LargeObj* p = new LargeObj;
            p->fill();
            delete p;
        }
    }
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "内存池耗时: " << pool_time.count() << " ms\n";
    std::cout << "系统默认: " << sys_time.count() << " ms\n";
    std::cout << "性能提升: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

int main() {
    // 初始化内存池
    HashBucket::initMemoryPool();
    // 运行测试用例
    TestSmallObject();
    TestLargeObject();
    TestMixedObjects();

    return 0;
}





// // 测试类（确保实际内存访问）
// struct P1 { int data[16]; };  // 增大内存占用
// struct P2 { int data[32]; };
// struct P3 { int data[64]; };
// struct P4 { int data[128]; };

// // 防止编译器优化的全局变量
// volatile int anti_optimize;

// // 精准测试函数
// void BenchmarkMemoryPool(size_t ntimes) {
//     std::mt19937 gen(42);
//     std::uniform_int_distribution<> dis(1, 100);

//     auto start = std::chrono::high_resolution_clock::now();
//     for (size_t i = 0; i < ntimes; i++) {
//         P1* p1 = newElement<P1>();
//         P2* p2 = newElement<P2>();
//         P3* p3 = newElement<P3>();
//         P4* p4 = newElement<P4>();

//         // 实际内存写入（防止优化）
//         p1->data[0] = dis(gen);
//         p2->data[0] = dis(gen);
//         p3->data[0] = dis(gen);
//         p4->data[0] = dis(gen);

//         // 内存屏障
//         asm volatile("" : : : "memory");

//         anti_optimize = p1->data[0] + p2->data[0] + p3->data[0] + p4->data[0];

//         deleteElement(p1);
//         deleteElement(p2);
//         deleteElement(p3);
//         deleteElement(p4);
//     }
//     auto dur = std::chrono::high_resolution_clock::now() - start;
//     std::cout << "内存池耗时: " 
//               << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() 
//               << " ms\n";
// }

// void BenchmarkNew(size_t ntimes) {
//     std::mt19937 gen(42);
//     std::uniform_int_distribution<> dis(1, 100);

//     auto start = std::chrono::high_resolution_clock::now();
//     for (size_t i = 0; i < ntimes; i++) {
//         P1* p1 = new P1;
//         P2* p2 = new P2;
//         P3* p3 = new P3;
//         P4* p4 = new P4;

//         p1->data[0] = dis(gen);
//         p2->data[0] = dis(gen);
//         p3->data[0] = dis(gen);
//         p4->data[0] = dis(gen);

//         asm volatile("" : : : "memory");

//         anti_optimize = p1->data[0] + p2->data[0] + p3->data[0] + p4->data[0];

//         delete p1;
//         delete p2;
//         delete p3;
//         delete p4;
//     }
//     auto dur = std::chrono::high_resolution_clock::now() - start;
//     std::cout << "new/delete 耗时: " 
//               << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() 
//               << " ms\n";
// }

 //int main() {
 //    const size_t times = 1000000;
 //    HashBucket::initMemoryPool();

//     // BenchmarkMemoryPool(times);
//     // BenchmarkNew(times);

//     auto p=newElement<int>(33);
//     std::cout<<(*p)<<std::endl;

//     return 0;
// }