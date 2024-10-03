#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <stdio.h>
#include <unistd.h>

#define OPTIONS "i:o:h"

typedef struct Code {
    uint64_t code;
    uint8_t code_length;
} Code;

uint32_t fill_histogram(FILE *fin, uint32_t *histogram) {
    // Clear all elements of the histogram array
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    uint32_t totalSize = 0;

    int byte;
    while ((byte = fgetc(fin)) != EOF) {
        histogram[byte]++;
        totalSize++;
    }

    // Apply the important hack to ensure at least two non-zero values in the histogram
    histogram[0]++;
    histogram[255]++;

    // Return the total size of the file
    return totalSize;
}

Node *create_tree(uint32_t *histogram, uint16_t *num_leaves) {
    PriorityQueue *pq = pq_create();

    // Go through the histogram and create a node for every non-zero histogram entry
    for (uint16_t i = 0; i < 256; i++) {
        if (histogram[i] > 0) {
            // Create a new node with the symbol and weight of the histogram entry
            Node *newNode = node_create((uint8_t) i, histogram[i]);

            // Increment the number of leaves
            (*num_leaves)++;

            // Put the new node in the priority queue
            enqueue(pq, newNode);
        }
    }

    // Run the Huffman Coding algorithm
    while (!pq_size_is_1(pq)) {
        // Dequeue into left
        Node *left = dequeue(pq);

        // Dequeue into right
        Node *right = dequeue(pq);

        // Create a new node with weight = left->weight + right->weight
        Node *newNode = node_create(0, left->weight + right->weight);

        // Set left and right children
        newNode->left = left;
        newNode->right = right;

        // Enqueue the new node
        enqueue(pq, newNode);
    }

    // Dequeue the queue’s only entry and return it
    Node *code_tree = dequeue(pq);

    // Free the Priority Queue
    pq_free(&pq);

    // Return huffman tree
    return code_tree;
}

void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    if (node->left != NULL && node->right != NULL) {
        // Internal node: recursive calls for left and right children
        fill_code_table(code_table, node->left, code, code_length + 1);

        // Append a 1 to code and recurse
        code |= (uint64_t) 1 << code_length;
        fill_code_table(code_table, node->right, code, code_length + 1);
    } else {
        // Leaf node: store the Huffman Code
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
    }
}

void huff_write_tree(BitWriter *outbuf, Node *node) {
    if (node->left == NULL) {
        /* node is a leaf */
        bit_write_bit(outbuf, 1);
        bit_write_uint8(outbuf, node->symbol);
    } else {
        /* node is internal */
        huff_write_tree(outbuf, node->left);
        huff_write_tree(outbuf, node->right);
        bit_write_bit(outbuf, 0);
    }
}

void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves,
    Node *code_tree, Code *code_table) {
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    bit_write_uint32(outbuf, filesize);
    bit_write_uint16(outbuf, num_leaves);

    huff_write_tree(outbuf, code_tree);

    while (1) {
        int b = fgetc(fin);
        if (b == EOF) {
            break;
        }

        uint64_t code = code_table[b].code;
        uint8_t code_length = code_table[b].code_length;

        for (int i = 0; i < code_length; ++i) {
            bit_write_bit(outbuf, code & 1);
            code >>= 1;
        }
    }
}

int main(int argc, char **argv) {
    int opt = 0;
    bool option_i = false;
    bool option_o = false;
    bool option_h = false;

    char *infile = NULL;
    char *outfile = NULL;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            option_i = true;
            infile = optarg;
            break;
        case 'o':
            option_o = true;
            outfile = optarg;
            break;
        case 'h': option_h = true; break;
        }
    }

    if (option_h || !option_i || !option_o) {
        printf("huff:  -i option is required\n");
        printf("Usage: huff -i infile -o outfile\n");
        printf("huff -v -i infile -o outfile\n");
        printf("huff -h\n");
        return 0;
    }

    if (option_i && option_o) {
        // Step 1: Read the file, and count the frequency of each symbol.
        FILE *fin = fopen(infile, "rb");
        if (fin == NULL) {
            fprintf(stderr, "Error: Unable to open input file %s\n", infile);
            return 1;
        }
        uint32_t histogram[256];
        uint32_t filesize = fill_histogram(fin, histogram);
        fclose(fin);

        // Step 2: Create a code tree from the histogram.
        Node *code_tree;
        uint16_t num_leaves = 0;
        code_tree = create_tree(histogram, &num_leaves);

        // Step 3: Fill a 256-entry code table, one entry for each byte value.
        Code code_table[256];
        fill_code_table(code_table, code_tree, 0, 0);

        // Step 4: Rewind the input file using fseek() in preparation for the next step.
        fin = fopen(infile, "rb");
        if (fin == NULL) {
            fprintf(stderr, "Error: Unable to open input file %s\n", infile);
            return 1;
        }

        // Step 5: Create a Huffman Coded output file from the input file.
        BitWriter *outbuf = bit_write_open(outfile);
        if (outbuf == NULL) {
            fclose(fin);
            fprintf(stderr, "Error: Unable to open output file %s\n", outfile);
            return 1;
        }
        huff_compress_file(outbuf, fin, filesize, num_leaves, code_tree, code_table);

        // Cleanup
        fclose(fin);
        bit_write_close(&outbuf);
    }

    return 0;
}
