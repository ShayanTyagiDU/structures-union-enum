/**
 * 10.c - Safe Heterogeneous Data Container Library
 * 
 * Implements a memory-safe typed-value container capable of storing and replacing
 * values of different C types (int, float, char, string) with full edge-case handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. Enumeration for data types including EMPTY
typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

// Union to store the data
typedef union {
    int int_val;
    float float_val;
    char char_val;
    char *string_val;
} Data;

// Container structure combining union and tag
typedef struct {
    Data data;
    DataType type;
} Container;

// --- API Declarations ---
void container_init(Container *c);
void container_clear(Container *c);

int container_set_int(Container *c, int value);
int container_set_float(Container *c, float value);
int container_set_char(Container *c, char value);
int container_set_string(Container *c, const char *value);

int container_get_int(const Container *c, int *out_value);
int container_get_float(const Container *c, float *out_value);
int container_get_char(const Container *c, char *out_value);
int container_get_string(const Container *c, char **out_value);

int container_copy(Container *dest, const Container *src);
const char* container_type_name(const Container *c);
void container_print(const Container *c);


// --- API Implementations ---

void container_init(Container *c) {
    if (c) {
        c->type = TYPE_EMPTY;
    }
}

void container_clear(Container *c) {
    if (!c) return;
    if (c->type == TYPE_STRING && c->data.string_val != NULL) {
        free(c->data.string_val);
        c->data.string_val = NULL;
    }
    c->type = TYPE_EMPTY;
}

int container_set_int(Container *c, int value) {
    if (!c) return 0;
    container_clear(c);
    c->data.int_val = value;
    c->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *c, float value) {
    if (!c) return 0;
    container_clear(c);
    c->data.float_val = value;
    c->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *c, char value) {
    if (!c) return 0;
    container_clear(c);
    c->data.char_val = value;
    c->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *c, const char *value) {
    if (!c || !value) return 0;
    
    // Handle self-assignment safely (e.g., set_string(c, c->data.string_val))
    if (c->type == TYPE_STRING && c->data.string_val == value) {
        return 1;
    }

    size_t len = strlen(value);
    char *new_str = (char *)malloc(len + 1);
    if (!new_str) {
        return 0; // Allocation failure; previous state preserved
    }
    
    memcpy(new_str, value, len + 1);
    
    container_clear(c);
    c->data.string_val = new_str;
    c->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *c, int *out_value) {
    if (!c || !out_value || c->type != TYPE_INT) return 0;
    *out_value = c->data.int_val;
    return 1;
}

int container_get_float(const Container *c, float *out_value) {
    if (!c || !out_value || c->type != TYPE_FLOAT) return 0;
    *out_value = c->data.float_val;
    return 1;
}

int container_get_char(const Container *c, char *out_value) {
    if (!c || !out_value || c->type != TYPE_CHAR) return 0;
    *out_value = c->data.char_val;
    return 1;
}

int container_get_string(const Container *c, char **out_value) {
    if (!c || !out_value || c->type != TYPE_STRING) return 0;
    *out_value = c->data.string_val;
    return 1;
}

int container_copy(Container *dest, const Container *src) {
    if (!dest || !src) return 0;
    if (dest == src) return 1; // Self-copy safety

    switch (src->type) {
        case TYPE_EMPTY:
            container_clear(dest);
            return 1;
        case TYPE_INT:
            return container_set_int(dest, src->data.int_val);
        case TYPE_FLOAT:
            return container_set_float(dest, src->data.float_val);
        case TYPE_CHAR:
            return container_set_char(dest, src->data.char_val);
        case TYPE_STRING:
            return container_set_string(dest, src->data.string_val);
        default:
            return 0;
    }
}

const char* container_type_name(const Container *c) {
    if (!c) return "NULL";
    switch (c->type) {
        case TYPE_EMPTY:  return "EMPTY";
        case TYPE_INT:    return "INT";
        case TYPE_FLOAT:  return "FLOAT";
        case TYPE_CHAR:   return "CHAR";
        case TYPE_STRING: return "STRING";
        default:          return "UNKNOWN";
    }
}

void container_print(const Container *c) {
    if (!c) return;
    switch (c->type) {
        case TYPE_EMPTY:
            printf("[EMPTY]\n");
            break;
        case TYPE_INT:
            printf("%d\n", c->data.int_val);
            break;
        case TYPE_FLOAT:
            printf("%f\n", c->data.float_val);
            break;
        case TYPE_CHAR:
            printf("'%c'\n", c->data.char_val);
            break;
        case TYPE_STRING:
            printf("\"%s\"\n", c->data.string_val);
            break;
    }
}

// --- Demonstration & Edge Case Testing ---

int main() {
    printf("--- 1. Basic Mixed-Type Array Demonstration ---\n");
    Container arr[6];
    for (int i = 0; i < 6; i++) {
        container_init(&arr[i]);
    }

    container_set_int(&arr[0], 10);
    container_set_float(&arr[1], 3.14f);
    container_set_char(&arr[2], 'A');
    container_set_string(&arr[3], "Hello");
    container_set_int(&arr[4], 0);        // Edge case: valid integer zero
    container_set_string(&arr[5], "Data");

    for (int i = 0; i < 6; i++) {
        printf("Index %d | Type: %-7s | Value: ", i, container_type_name(&arr[i]));
        container_print(&arr[i]);
    }

    printf("\n--- 2. Deep Copy and Modification Demonstration ---\n");
    Container copy_container;
    container_init(&copy_container);
    
    // Copy the last string container
    container_copy(&copy_container, &arr[5]);
    printf("Original before change: "); container_print(&arr[5]);
    printf("Copy before change:     "); container_print(&copy_container);
    
    // Change original string
    container_set_string(&arr[5], "Modified Data");
    printf("Original after change:  "); container_print(&arr[5]);
    printf("Copy after change:      "); container_print(&copy_container); // Copy should remain unchanged

    printf("\n--- 3. Type Transitions & Memory Management ---\n");
    Container transition_c;
    container_init(&transition_c);
    
    container_set_string(&transition_c, "Initial String");
    printf("Set to STRING: "); container_print(&transition_c);
    
    container_set_int(&transition_c, 42); // String freed automatically
    printf("Set to INT:    "); container_print(&transition_c);
    
    container_set_string(&transition_c, "Another String");
    printf("Set to STRING: "); container_print(&transition_c);
    
    container_set_float(&transition_c, 9.99f); // String freed automatically
    printf("Set to FLOAT:  "); container_print(&transition_c);
    
    container_clear(&transition_c);

    printf("\n--- 4. Edge Case Testing ---\n");
    Container edge_c;
    container_init(&edge_c);
    
    // Wrong-type getter
    container_set_int(&edge_c, 100);
    float f_val;
    if (!container_get_float(&edge_c, &f_val)) {
        printf("Wrong-type getter correctly rejected.\n");
    }

    // Empty container
    container_clear(&edge_c);
    int i_val;
    if (!container_get_int(&edge_c, &i_val)) {
        printf("Read from empty container correctly rejected.\n");
    }

    // Repeated clear
    container_clear(&edge_c);
    container_clear(&edge_c);
    printf("Repeated clear survived safely.\n");

    // Empty string
    container_set_string(&edge_c, "");
    printf("Empty string test: "); container_print(&edge_c);

    // Self-copy
    container_copy(&edge_c, &edge_c);
    printf("Self-copy test survived: "); container_print(&edge_c);

    // String replacement with same source pointer
    char *internal_str;
    container_get_string(&edge_c, &internal_str);
    container_set_string(&edge_c, internal_str);
    printf("Same-pointer replacement survived: "); container_print(&edge_c);

    // Cleanup entire array and stray containers
    for (int i = 0; i < 6; i++) {
        container_clear(&arr[i]);
    }
    container_clear(&copy_container);
    container_clear(&edge_c);

    printf("\nAll memory safely cleared. Exiting.\n");
    return 0;
    /*--- 1. Basic Mixed-Type Array Demonstration ---
Index 0 | Type: INT     | Value: 10
Index 1 | Type: FLOAT   | Value: 3.140000
Index 2 | Type: CHAR    | Value: 'A'
Index 3 | Type: STRING  | Value: "Hello"
Index 4 | Type: INT     | Value: 0
Index 5 | Type: STRING  | Value: "Data"

--- 2. Deep Copy and Modification Demonstration ---
Original before change: "Data"
Copy before change:     "Data"
Original after change:  "Modified Data"
Copy after change:      "Data"

--- 3. Type Transitions & Memory Management ---
Set to STRING: "Initial String"
Set to INT:    42
Set to STRING: "Another String"
Set to FLOAT:  9.990000

--- 4. Edge Case Testing ---
Wrong-type getter correctly rejected.
Read from empty container correctly rejected.
Repeated clear survived safely.
Empty string test: ""
Self-copy test survived: ""
Same-pointer replacement survived: ""

All memory safely cleared. Exiting.*/
    // output
}
