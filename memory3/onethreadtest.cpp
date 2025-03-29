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

// ========== �������� ==========
const size_t TEST_TIMES = 1000000;  // ����ѭ������
const size_t SMALL_SIZE = 2;       // С�����С
const size_t LARGE_SIZE = 600;      // ������С

// ========== ���Զ����� ==========
struct SmallObj {
    char data[SMALL_SIZE];  // 64�ֽ�
    void fill() {
        for (size_t i = 0; i < SMALL_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

struct LargeObj {
    char data[LARGE_SIZE];  // 512�ֽ�
    void fill() {
        for (size_t i = 0; i < LARGE_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

// ========== �������� ==========

// ����1����������/�ͷ�С����
void TestSmallObject() {
    std::cout << "\n=== ����1��С����(" << SMALL_SIZE << "�ֽ�)��������/�ͷ� ===\n";

    // �ڴ�ز���
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        SmallObj* p = newElement<SmallObj>();
        p->fill();
        deleteElement(p);
    }
    auto pool_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    // ϵͳĬ�ϲ���
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        SmallObj* p = new SmallObj;
        p->fill();
        delete p;
    }
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "�ڴ�غ�ʱ: " << pool_time.count() << " ms\n";
    std::cout << "ϵͳĬ��: " << sys_time.count() << " ms\n";
    std::cout << "��������: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

// ����2����������/�ͷŴ����
void TestLargeObject() {
    std::cout << "\n=== ����2�������(" << LARGE_SIZE << "�ֽ�)��������/�ͷ� ===\n";

    // �ڴ�ز���
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        LargeObj* p = newElement<LargeObj>();
        p->fill();
        deleteElement(p);
    }
    auto pool_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    // ϵͳĬ�ϲ���
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_TIMES; ++i) {
        LargeObj* p = new LargeObj;
        p->fill();
        delete p;
    }
    auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "�ڴ�غ�ʱ: " << pool_time.count() << " ms\n";
    std::cout << "ϵͳĬ��: " << sys_time.count() << " ms\n";
    std::cout << "��������: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

// ����3����ϴ�С�������
void TestMixedObjects() {
    std::cout << "\n=== ����3����ϴ�С������� ===\n";

    std::mt19937 gen(42);
    std::uniform_int_distribution<> size_dist(1, 1024);

    // �ڴ�ز���
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

    // ϵͳĬ�ϲ���
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

    std::cout << "�ڴ�غ�ʱ: " << pool_time.count() << " ms\n";
    std::cout << "ϵͳĬ��: " << sys_time.count() << " ms\n";
    std::cout << "��������: "
        << (sys_time.count() - pool_time.count()) * 100 / sys_time.count()
        << "%\n";
}

int main() {
    // ��ʼ���ڴ��
    HashBucket::initMemoryPool();
    // ���в�������
    TestSmallObject();
    TestLargeObject();
    TestMixedObjects();

    return 0;
}





// // �����ࣨȷ��ʵ���ڴ���ʣ�
// struct P1 { int data[16]; };  // �����ڴ�ռ��
// struct P2 { int data[32]; };
// struct P3 { int data[64]; };
// struct P4 { int data[128]; };

// // ��ֹ�������Ż���ȫ�ֱ���
// volatile int anti_optimize;

// // ��׼���Ժ���
// void BenchmarkMemoryPool(size_t ntimes) {
//     std::mt19937 gen(42);
//     std::uniform_int_distribution<> dis(1, 100);

//     auto start = std::chrono::high_resolution_clock::now();
//     for (size_t i = 0; i < ntimes; i++) {
//         P1* p1 = newElement<P1>();
//         P2* p2 = newElement<P2>();
//         P3* p3 = newElement<P3>();
//         P4* p4 = newElement<P4>();

//         // ʵ���ڴ�д�루��ֹ�Ż���
//         p1->data[0] = dis(gen);
//         p2->data[0] = dis(gen);
//         p3->data[0] = dis(gen);
//         p4->data[0] = dis(gen);

//         // �ڴ�����
//         asm volatile("" : : : "memory");

//         anti_optimize = p1->data[0] + p2->data[0] + p3->data[0] + p4->data[0];

//         deleteElement(p1);
//         deleteElement(p2);
//         deleteElement(p3);
//         deleteElement(p4);
//     }
//     auto dur = std::chrono::high_resolution_clock::now() - start;
//     std::cout << "�ڴ�غ�ʱ: " 
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
//     std::cout << "new/delete ��ʱ: " 
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