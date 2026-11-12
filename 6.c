// 6.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

// 1. Core Data Structures
typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

// 2. Tagged Union Setup
typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT,
    VALUE_NONE // Used for uninitialized or invalid states
} ValueType;

typedef struct {
    UnionData data;
    ValueType tag;
} TaggedRecord;

// 3. Safe Setter Functions
void set_int(TaggedRecord *rec, int val) {
    rec->data.i = val;
    rec->tag = VALUE_INT;
}

void set_float(TaggedRecord *rec, float val) {
    rec->data.f = val;
    rec->tag = VALUE_FLOAT;
}

void set_text(TaggedRecord *rec, const char *text) {
    if (strlen(text) >= 20) {
        printf("  [Error] String '%s' exceeds 19 characters. Assignment rejected.\n", text);
        return; 
    }
    strcpy(rec->data.str, text);
    rec->tag = VALUE_TEXT;
}

// 4. Safe Display Function
void display_record(const TaggedRecord *rec) {
    switch (rec->tag) {
        case VALUE_INT:
            printf("  [INT]   %d\n", rec->data.i);
            break;
        case VALUE_FLOAT:
            printf("  [FLOAT] %.2f\n", rec->data.f);
            break;
        case VALUE_TEXT:
            printf("  [TEXT]  %s\n", rec->data.str);
            break;
        default:
            printf("  [ERROR] Invalid or uninitialized tag detected!\n");
            break;
    }
}

int main() {
    printf("=== PART 1: MEMORY LAYOUT ANALYSIS ===\n");
    
    StructData sdata;
    UnionData udata;
    
    // Print Sizes and Alignments
    printf("Sizes:\n");
    printf("  sizeof(StructData) = %zu bytes\n", sizeof(StructData));
    printf("  sizeof(UnionData)  = %zu bytes\n", sizeof(UnionData));
    printf("  sizeof(TaggedRecord) = %zu bytes\n", sizeof(TaggedRecord));
    
    printf("\nAlignments:\n");
    printf("  alignof(StructData) = %zu bytes\n", alignof(StructData));
    printf("  alignof(UnionData)  = %zu bytes\n", alignof(UnionData));

    // Print Structure Offsets
    printf("\nStructure Member Offsets (stddef.h -> offsetof):\n");
    printf("  Offset of sdata.i   : %zu bytes\n", offsetof(StructData, i));
    printf("  Offset of sdata.f   : %zu bytes\n", offsetof(StructData, f));
    printf("  Offset of sdata.str : %zu bytes\n", offsetof(StructData, str));

    // Print Memory Addresses
    printf("\nMemory Addresses (Struct vs Union):\n");
    printf("  sdata base address : %p\n", (void*)&sdata);
    printf("  sdata.i address    : %p\n", (void*)&sdata.i);
    printf("  sdata.f address    : %p\n", (void*)&sdata.f);
    printf("  sdata.str address  : %p\n", (void*)&sdata.str);
    
    printf("  ---\n");
    printf("  udata base address : %p\n", (void*)&udata);
    printf("  udata.i address    : %p\n", (void*)&udata.i);
    printf("  udata.f address    : %p\n", (void*)&udata.f);
    printf("  udata.str address  : %p\n", (void*)&udata.str);

    // Bulk Memory Comparison
    printf("\nBulk Memory Usage (100 records):\n");
    printf("  100 Structs : %zu bytes\n", 100 * sizeof(StructData));
    printf("  100 Unions  : %zu bytes\n", 100 * sizeof(UnionData));


    printf("\n\n=== PART 2: SAFE TAGGED UNION DEMONSTRATION ===\n");

    // Array of 5 records
    TaggedRecord records[5];
    
    // Illustrative usage scenario
    printf("Populating 5 tagged records...\n");
    set_int(&records[0], 10);
    set_float(&records[1], 220.5f);
    set_text(&records[2], "Union Programming");
    set_int(&records[3], 999);
    set_text(&records[4], "Short String");

    for (int i = 0; i < 5; i++) {
        printf("Record %d: ", i + 1);
        display_record(&records[i]);
    }

    printf("\n=== PART 3: EDGE CASE TESTS ===\n");
    
    TaggedRecord edge_test;

    // 1. Maximum-length valid string (19 chars + null terminator)
    printf("Test A: Maximum valid string length (19 chars):\n");
    set_text(&edge_test, "1234567890123456789"); 
    display_record(&edge_test);

    // 2. Overlength string rejection
    printf("\nTest B: Overlength string (25 chars):\n");
    set_text(&edge_test, "This string is way too long");
    display_record(&edge_test); // Should still safely print the previous value

    // 3. Switching a single record dynamically
    printf("\nTest C: Switching single record (Int -> Text -> Float):\n");
    set_int(&edge_test, 42);
    display_record(&edge_test);
    
    set_text(&edge_test, "Overwritten!");
    display_record(&edge_test);
    
    set_float(&edge_test, 3.14159f);
    display_record(&edge_test);

    // 4. Invalid tag handling
    printf("\nTest D: Invalid Tag Recovery:\n");
    edge_test.tag = VALUE_NONE; // Simulating corruption or uninitialized state
    display_record(&edge_test);

    return 0;
}
