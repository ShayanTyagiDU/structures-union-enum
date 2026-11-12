// Structures - 2
// 5.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define MAX_CAPACITY 1000
#define EPSILON 1e-5

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

typedef struct {
    Rectangle *data;
    size_t count;
    size_t capacity;
} Inventory;

// Utility to clear input buffer
void clear_input() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// Safely reads a positive finite float
float read_positive_float(const char *prompt) {
    char input[100];
    char *end;
    float val;
    
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL) exit(0);
        
        errno = 0;
        val = strtof(input, &end);
        
        while (*end == ' ' || *end == '\t') end++;
        
        if (end != input && (*end == '\n' || *end == '\0') && errno != ERANGE) {
            if (isfinite(val) && val > 0.0f) {
                return val;
            } else {
                printf("Error: Value must be a finite positive number (no NaN/infinity, > 0).\n");
            }
        } else {
            printf("Error: Invalid numeric input.\n");
        }
    }
}

// Safely reads an integer within a range
int read_int_range(const char *prompt, int min, int max) {
    int val;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1 && val >= min && val <= max) {
            clear_input();
            return val;
        }
        printf("Error: Please enter a valid integer between %d and %d.\n", min, max);
        clear_input();
    }
}

// Computes derived geometry fields
void compute_geometry(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

// Determines if rectangle is a square using tolerance
int is_square(const Rectangle *r) {
    return fabs(r->width - r->height) < EPSILON;
}

// Returns index of rectangle by ID, or -1 if not found
int find_rectangle(const Inventory *inv, int id) {
    for (size_t i = 0; i < inv->count; i++) {
        if (inv->data[i].id == id) return (int)i;
    }
    return -1;
}

// Expands inventory array dynamically
int expand_inventory(Inventory *inv) {
    if (inv->capacity >= MAX_CAPACITY) {
        printf("Error: Maximum inventory limit (%d) reached.\n", MAX_CAPACITY);
        return 0;
    }
    
    size_t new_cap = inv->capacity * 2;
    if (new_cap > MAX_CAPACITY) new_cap = MAX_CAPACITY;
    
    Rectangle *temp = (Rectangle *)realloc(inv->data, new_cap * sizeof(Rectangle));
    if (!temp) {
        printf("Error: Memory reallocation failed. Original data preserved.\n");
        return 0;
    }
    
    inv->data = temp;
    inv->capacity = new_cap;
    return 1;
}

// Adds a new rectangle
void add_rectangle(Inventory *inv) {
    if (inv->count == inv->capacity && !expand_inventory(inv)) return;

    int id = read_int_range("Enter unique positive Rectangle ID: ", 1, 999999);
    
    if (find_rectangle(inv, id) != -1) {
        printf("Error: Rectangle ID %d already exists.\n", id);
        return;
    }

    float width = read_positive_float("Enter width: ");
    float height = read_positive_float("Enter height: ");

    Rectangle r;
    r.id = id;
    r.width = width;
    r.height = height;
    compute_geometry(&r);

    inv->data[inv->count++] = r;
    printf("Success: Rectangle %d added.\n", id);
}

// Displays a single rectangle
void print_rectangle(const Rectangle *r) {
    printf("ID: %-5d | W: %-6.2f | H: %-6.2f | Area: %-8.2f | Perim: %-8.2f | Type: %s\n",
           r->id, r->width, r->height, r->area, r->perimeter,
           is_square(r) ? "Square" : "Rectangle");
}

// Displays all rectangles
void display_all(const Inventory *inv) {
    if (inv->count == 0) {
        printf("\nThe inventory is empty.\n");
        return;
    }
    printf("\n--- Rectangle Inventory (%zu/%zu) ---\n", inv->count, inv->capacity);
    for (size_t i = 0; i < inv->count; i++) {
        print_rectangle(&inv->data[i]);
    }
}

// Updates an existing rectangle
void update_rectangle(Inventory *inv) {
    int id = read_int_range("Enter ID to update: ", 1, 999999);
    int idx = find_rectangle(inv, id);
    
    if (idx == -1) {
        printf("Error: Rectangle %d not found.\n", id);
        return;
    }

    printf("Updating Rectangle %d...\n", id);
    inv->data[idx].width = read_positive_float("Enter new width: ");
    inv->data[idx].height = read_positive_float("Enter new height: ");
    compute_geometry(&inv->data[idx]);
    
    printf("Success: Rectangle %d updated.\n", id);
}

// Deletes a rectangle and shifts remaining items
void delete_rectangle(Inventory *inv) {
    int id = read_int_range("Enter ID to delete: ", 1, 999999);
    int idx = find_rectangle(inv, id);
    
    if (idx == -1) {
        printf("Error: Rectangle %d not found.\n", id);
        return;
    }

    for (size_t i = idx; i < inv->count - 1; i++) {
        inv->data[i] = inv->data[i + 1];
    }
    inv->count--;
    printf("Success: Rectangle %d deleted.\n", id);
}

// Computes and displays summary statistics
void display_statistics(const Inventory *inv) {
    if (inv->count == 0) {
        printf("\nNo rectangles available to calculate statistics.\n");
        return;
    }

    float total_area = 0.0f;
    int max_idx = 0, min_idx = 0;

    for (size_t i = 0; i < inv->count; i++) {
        float a = inv->data[i].area;
        total_area += a;
        if (a > inv->data[max_idx].area) max_idx = (int)i;
        if (a < inv->data[min_idx].area) min_idx = (int)i;
    }

    printf("\n--- Geometry Analytics ---\n");
    printf("Total Area: %.2f\n", total_area);
    printf("Average Area: %.2f\n", total_area / inv->count);
    printf("Largest Area : %.2f (ID: %d)\n", inv->data[max_idx].area, inv->data[max_idx].id);
    printf("Smallest Area: %.2f (ID: %d)\n", inv->data[min_idx].area, inv->data[min_idx].id);
}

// Comparator for sorting (Area ASC, breaking ties with ID ASC)
int compare_rectangles(const void *a, const void *b) {
    const Rectangle *r1 = (const Rectangle *)a;
    const Rectangle *r2 = (const Rectangle *)b;
    
    if (fabs(r1->area - r2->area) > EPSILON) {
        return (r1->area > r2->area) ? 1 : -1;
    }
    return r1->id - r2->id;
}

// Displays a sorted copy of the inventory
void display_sorted(const Inventory *inv) {
    if (inv->count == 0) {
        printf("\nNo rectangles to sort.\n");
        return;
    }

    Rectangle *copy = (Rectangle *)malloc(inv->count * sizeof(Rectangle));
    if (!copy) {
        printf("Error: Memory allocation failed for sorting.\n");
        return;
    }
    
    memcpy(copy, inv->data, inv->count * sizeof(Rectangle));
    qsort(copy, inv->count, sizeof(Rectangle), compare_rectangles);

    printf("\n--- Rectangles Sorted by Area (Ascending) ---\n");
    for (size_t i = 0; i < inv->count; i++) {
        print_rectangle(&copy[i]);
    }
    free(copy);
}

// Displays rectangles exceeding a user-defined area threshold
void filter_by_area(const Inventory *inv) {
    if (inv->count == 0) {
        printf("\nNo rectangles to filter.\n");
        return;
    }
    
    float threshold = read_positive_float("Enter minimum area threshold: ");
    int found = 0;
    
    printf("\n--- Rectangles with Area > %.2f ---\n", threshold);
    for (size_t i = 0; i < inv->count; i++) {
        if (inv->data[i].area > threshold) {
            print_rectangle(&inv->data[i]);
            found++;
        }
    }
    if (!found) printf("No rectangles exceed the specified area.\n");
}

int main() {
    int initial_cap = read_int_range("Enter initial inventory capacity (1-1000): ", 1, MAX_CAPACITY);
    
    Inventory inv;
    inv.capacity = initial_cap;
    inv.count = 0;
    inv.data = (Rectangle *)malloc(initial_cap * sizeof(Rectangle));
    
    if (!inv.data) {
        printf("Critical Error: Initial memory allocation failed.\n");
        return 1;
    }

    int choice;
    do {
        printf("\n=== Rectangle Inventory Menu ===\n");
        printf("1. Add Rectangle\n2. Display All\n3. Update Dimensions\n");
        printf("4. Delete Rectangle\n5. Analytics Summary\n6. Display Sorted (Area)\n");
        printf("7. Filter by Area Threshold\n8. Exit\n");
        
        choice = read_int_range("Select an option: ", 1, 8);

        switch (choice) {
            case 1: add_rectangle(&inv); break;
            case 2: display_all(&inv); break;
            case 3: update_rectangle(&inv); break;
            case 4: delete_rectangle(&inv); break;
            case 5: display_statistics(&inv); break;
            case 6: display_sorted(&inv); break;
            case 7: filter_by_area(&inv); break;
            case 8: 
                free(inv.data);
                printf("Memory released. Exiting...\n");
                break;
        }
    } while (choice != 8);

    return 0;
    /*Enter initial inventory capacity (1-1000): 23

=== Rectangle Inventory Menu ===
1. Add Rectangle
2. Display All
3. Update Dimensions
4. Delete Rectangle
5. Analytics Summary
6. Display Sorted (Area)
7. Filter by Area Threshold
8. Exit
Select an option: 1
Enter unique positive Rectangle ID: 1
Enter width: 23
Enter height: 98
Success: Rectangle 1 added.

=== Rectangle Inventory Menu ===
1. Add Rectangle
2. Display All
3. Update Dimensions
4. Delete Rectangle
5. Analytics Summary
6. Display Sorted (Area)
7. Filter by Area Threshold
8. Exit
Select an option: 2

--- Rectangle Inventory (1/23) ---
ID: 1     | W: 23.00  | H: 98.00  | Area: 2254.00  | Perim: 242.00   | Type: Rectangle*/
    //output
}
