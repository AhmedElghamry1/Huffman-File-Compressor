#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string>
#include <new>
#include <cstring>
#include <limits>
using namespace std;
FILE* searchingforfile(string filepath) {

   

        if (!filepath.empty() && filepath.front() == '"' && filepath.back() == '"') {
            filepath = filepath.substr(1, filepath.length() - 2);
        }

    FILE* file = fopen(filepath.c_str(), "rb");

    if (!file) {
        printf("Error: file cannot be found\n");
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long long size = ftell(file);

    if (size > (4ll * 1024 * 1024 * 1024)) {
        fclose(file);
        printf("Error: file is too large\n");
        return nullptr;
    }

    rewind(file);
    return file;
   

}
void frequencytable(FILE* file, long long freq[256]) {

   
        unsigned char ch;

    for (int i = 0; i < 256; i++)
        freq[i] = 0;

    while (fread(&ch, 1, 1, file) == 1)
        freq[ch]++;
   

}
struct node {

   
        unsigned char ch;
    long long freq;
    node* left;
    node* right;

    node(unsigned char c, long long f) {
        ch = c;
        freq = f;
        left = nullptr;
        right = nullptr;
    }

    node(long long f, node* l, node* r) {
        ch = '\0';
        freq = f;
        left = l;
        right = r;
    }
   

};
void freeTree(node* root) {

   
        if (!root)
            return;

    freeTree(root->left);
    freeTree(root->right);
    delete root;
   

}
class MinHeap {

private:

   
        node** arr;
    int size;
    int capacity;

    void checkforspace() {

        if (size < capacity)
            return;

        node** newarr = new(nothrow) node * [capacity * 2];

        if (!newarr) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        for (int i = 0; i < size; i++)
            newarr[i] = arr[i];

        delete[] arr;
        arr = newarr;
        capacity *= 2;
    }

    int left(int parent) { return 2 * parent + 1; }
    int right(int parent) { return 2 * parent + 2; }
    int parent(int child) { return (child - 1) / 2; }

    bool hasLeft(int index) { return left(index) < size; }
    bool hasRight(int index) { return right(index) < size; }
    bool hasParent(int index) { return index > 0; }

    void swapNodes(int i1, int i2) {
        node* temp = arr[i1];
        arr[i1] = arr[i2];
        arr[i2] = temp;
    }

    void heapifyUp() {

        int index = size - 1;

        while (hasParent(index) && arr[parent(index)]->freq > arr[index]->freq) {
            swapNodes(parent(index), index);
            index = parent(index);
        }
    }

    void heapifyDown() {

        int index = 0;

        while (hasLeft(index)) {

            int smallest = left(index);

            if (hasRight(index) && arr[right(index)]->freq < arr[smallest]->freq)
                smallest = right(index);

            if (arr[index]->freq <= arr[smallest]->freq)
                break;

            swapNodes(index, smallest);
            index = smallest;
        }
    }
   

public:

   
        MinHeap() {

        capacity = 10;
        size = 0;

        arr = new(nothrow) node * [capacity];

        if (!arr) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    ~MinHeap() {
        delete[] arr;
    }

    void add(node* data) {

        checkforspace();
        arr[size] = data;
        size++;

        heapifyUp();
    }

    node* extractMin() {

        if (size == 0)
            return nullptr;

        node* minNode = arr[0];

        arr[0] = arr[size - 1];
        size--;

        heapifyDown();

        return minNode;
    }

    int getSize() { return size; }
   

};
class huffmantree {

public:

   
        void storingcodes(node* current, char* code, int depth, char codes[256][256]) {

        if (!current)
            return;

        if (depth >= 255)
            return;

        if (!current->left && !current->right) {

            code[depth] = '\0';
            int index = (unsigned char)current->ch;
            strcpy(codes[index], code);
            return;
        }

        code[depth] = '0';
        storingcodes(current->left, code, depth + 1, codes);

        code[depth] = '1';
        storingcodes(current->right, code, depth + 1, codes);
    }

    void display(char codes[256][256]) {

        for (int i = 0; i < 256; i++) {

            if (codes[i][0] != '\0') {

                if (i == ' ')
                    printf("space : %s\n", codes[i]);
                else
                    printf("%c : %s\n", (char)i, codes[i]);
            }
        }
    }
   

};
void writeCodeInFile(const char* originalName, char codes[256][256], int rounds = 1) {

   
        char codefilename[300];

    strcpy(codefilename, originalName);

    char* dot = strrchr(codefilename, '.');

    if (dot)
        *dot = '\0';

    strcat(codefilename, ".cod");

    FILE* codfile = fopen(codefilename, "w");

    if (!codfile)
        return;

    fprintf(codfile, "%d\n", rounds);

    for (int i = 0; i < 256; i++) {

        if (codes[i][0] != '\0')
            fprintf(codfile, "%d %s\n", i, codes[i]);
    }

    fclose(codfile);
   

}
int compressfile(const char* inputname, const char* outputname, char codes[256][256]) {

   
        FILE* in = fopen(inputname, "rb");

    if (!in)
        return 0;

    FILE* out = fopen(outputname, "wb");

    if (!out) {
        fclose(in);
        return 0;
    }

    unsigned char ch;
    unsigned char byte = 0;
    int count = 0;

    while (fread(&ch, 1, 1, in) == 1) {

        for (int i = 0; codes[ch][i] != '\0'; i++) {

            if (codes[ch][i] == '1') {

                int pos = 7 - count;
                unsigned char mask = (1 << pos);
                byte |= mask;
            }

            count++;

            if (count == 8) {

                fwrite(&byte, 1, 1, out);
                byte = 0;
                count = 0;
            }
        }
    }

    if (count > 0)
        fwrite(&byte, 1, 1, out);

    fclose(in);
    fclose(out);

    return 1;
   

}
