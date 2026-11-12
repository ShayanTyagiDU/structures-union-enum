// Structures - 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_NAME_LEN 50

typedef struct {
    int roll;
    char name[MAX_NAME_LEN];
    int age;
    float gpa;
} Student;

typedef struct {
    Student *data;
    size_t count;
    size_t capacity;
} StudentList;

// Utility to safely read strings with spaces and flush stdin
void read_string(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            // Buffer overflowed, clear remaining stdin
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
    }
}

// Utility to clear input buffer after scanf
void clear_input() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

// Initializes the list with user-defined capacity
int init_list(StudentList *list) {
    int cap;
    printf("Enter initial capacity (>0): ");
    if (scanf("%d", &cap) != 1 || cap <= 0) {
        printf("Error: Invalid capacity. Defaulting to 2.\n");
        cap = 2;
        clear_input();
    }
    
    // Prevent excessively large initial allocations
    if ((size_t)cap > SIZE_MAX / sizeof(Student)) {
        printf("Error: Requested capacity too large.\n");
        return 0;
    }

    list->data = (Student*) malloc((size_t)cap * sizeof(Student));
    if (!list->data) {
        printf("Error: Memory allocation failed!\n");
        return 0;
    }
    list->capacity = (size_t)cap;
    list->count = 0;
    return 1;
}

// Returns the index of a student by roll number, or -1 if not found
int find_student(const StudentList *list, int roll) {
    for (size_t i = 0; i < list->count; i++) {
        if (list->data[i].roll == roll) return (int)i;
    }
    return -1;
}

// Expands the array capacity dynamically
int expand_list(StudentList *list) {
    size_t new_cap = list->capacity * 2;
    if (new_cap == 0) new_cap = 2;

    if (new_cap > SIZE_MAX / sizeof(Student)) {
        printf("Error: Arithmetic overflow. Cannot expand further.\n");
        return 0;
    }

    Student *temp = (Student*) realloc(list->data, new_cap * sizeof(Student));
    if (!temp) {
        printf("Error: Reallocation failed. Original data preserved.\n");
        return 0;
    }
    
    list->data = temp;
    list->capacity = new_cap;
    return 1;
}

// Adds a new student with validation
void add_student(StudentList *list) {
    if (list->count == list->capacity) {
        if (!expand_list(list)) return;
    }

    int roll;
    printf("Enter unique Roll Number (positive integer): ");
    scanf("%d", &roll);
    clear_input();

    if (roll <= 0) {
        printf("Error: Roll number must be positive.\n");
        return;
    }
    if (find_student(list, roll) != -1) {
        printf("Error: Roll number %d already exists.\n", roll);
        return;
    }

    char name[MAX_NAME_LEN];
    printf("Enter Name (max %d chars): ", MAX_NAME_LEN - 1);
    read_string(name, MAX_NAME_LEN);

    int age;
    printf("Enter Age (16-100): ");
    scanf("%d", &age);
    
    float gpa;
    printf("Enter GPA (0.00-4.00): ");
    scanf("%f", &gpa);
    clear_input();

    if (age < 16 || age > 100) {
        printf("Error: Age must be between 16 and 100.\n");
        return;
    }
    if (gpa < 0.0f || gpa > 4.0f) {
        printf("Error: GPA must be between 0.00 and 4.00.\n");
        return;
    }

    list->data[list->count].roll = roll;
    strcpy(list->data[list->count].name, name);
    list->data[list->count].age = age;
    list->data[list->count].gpa = gpa;
    list->count++;
    
    printf("Success: Student added.\n");
}

// Displays all records
void display_all(const StudentList *list) {
    if (list->count == 0) {
        printf("The collection is empty.\n");
        return;
    }
    printf("\n--- All Students (%zu/%zu) ---\n", list->count, list->capacity);
    for (size_t i = 0; i < list->count; i++) {
        printf("Roll: %d | Name: %s | Age: %d | GPA: %.2f\n", 
               list->data[i].roll, list->data[i].name, list->data[i].age, list->data[i].gpa);
    }
}

// Searches and displays a specific student
void search_and_display(const StudentList *list) {
    int roll;
    printf("Enter Roll Number to search: ");
    scanf("%d", &roll);
    clear_input();

    int idx = find_student(list, roll);
    if (idx != -1) {
        printf("Found: Roll: %d | Name: %s | Age: %d | GPA: %.2f\n", 
               list->data[idx].roll, list->data[idx].name, list->data[idx].age, list->data[idx].gpa);
    } else {
        printf("Error: Roll number %d not found.\n", roll);
    }
}

// Updates an existing student's details
void update_student(StudentList *list) {
    int roll;
    printf("Enter Roll Number to update: ");
    scanf("%d", &roll);
    clear_input();

    int idx = find_student(list, roll);
    if (idx == -1) {
        printf("Error: Roll number %d not found.\n", roll);
        return;
    }

    printf("Enter new Age (16-100): ");
    int age;
    scanf("%d", &age);
    if (age < 16 || age > 100) {
        printf("Error: Invalid age.\n");
        clear_input();
        return;
    }

    printf("Enter new GPA (0.00-4.00): ");
    float gpa;
    scanf("%f", &gpa);
    if (gpa < 0.0f || gpa > 4.0f) {
        printf("Error: Invalid GPA.\n");
        clear_input();
        return;
    }
    clear_input();

    list->data[idx].age = age;
    list->data[idx].gpa = gpa;
    printf("Success: Student updated.\n");
}

// Deletes a student and shifts remaining elements to preserve state
void delete_student(StudentList *list) {
    int roll;
    printf("Enter Roll Number to delete: ");
    scanf("%d", &roll);
    clear_input();

    int idx = find_student(list, roll);
    if (idx == -1) {
        printf("Error: Roll number %d not found.\n", roll);
        return;
    }

    for (size_t i = idx; i < list->count - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->count--;
    printf("Success: Student %d removed.\n", roll);
}

// Analytics reporting
void display_analytics(const StudentList *list) {
    if (list->count == 0) {
        printf("No records available to compute analytics.\n");
        return;
    }

    float min_gpa = list->data[0].gpa;
    float max_gpa = list->data[0].gpa;
    float sum_gpa = 0.0f;
    int max_idx = 0, min_idx = 0;

    for (size_t i = 0; i < list->count; i++) {
        float gpa = list->data[i].gpa;
        sum_gpa += gpa;
        if (gpa > max_gpa) { max_gpa = gpa; max_idx = (int)i; }
        if (gpa < min_gpa) { min_gpa = gpa; min_idx = (int)i; }
    }

    printf("\n--- Academic Analytics ---\n");
    printf("Highest GPA: %.2f (Roll: %d, Name: %s)\n", max_gpa, list->data[max_idx].roll, list->data[max_idx].name);
    printf("Lowest GPA: %.2f (Roll: %d, Name: %s)\n", min_gpa, list->data[min_idx].roll, list->data[min_idx].name);
    printf("Average GPA: %.2f\n", sum_gpa / list->count);
}

// Comparator for qsort: descending GPA, ascending Roll Number
int compare_students(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    
    if (s1->gpa < s2->gpa) return 1;
    if (s1->gpa > s2->gpa) return -1;
    return (s1->roll - s2->roll);
}

// Sorts and displays a temporary copy of the records
void display_sorted(const StudentList *list) {
    if (list->count == 0) {
        printf("No records to sort.\n");
        return;
    }

    Student *copy = (Student*) malloc(list->count * sizeof(Student));
    if (!copy) {
        printf("Error: Memory allocation failed for sorting.\n");
        return;
    }
    
    memcpy(copy, list->data, list->count * sizeof(Student));
    qsort(copy, list->count, sizeof(Student), compare_students);

    printf("\n--- Students Sorted by GPA (Desc) ---\n");
    for (size_t i = 0; i < list->count; i++) {
        printf("Roll: %d | Name: %s | Age: %d | GPA: %.2f\n", 
               copy[i].roll, copy[i].name, copy[i].age, copy[i].gpa);
    }
    
    free(copy);
}

// Main Menu Loop
int main() {
    StudentList list = {NULL, 0, 0};
    
    if (!init_list(&list)) {
        return 1;
    }

    int choice;
    do {
        printf("\n=== Student Records Menu ===\n");
        printf("1. Add Student\n");
        printf("2. Display All\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Academic Analytics\n");
        printf("7. Display Sorted (GPA)\n");
        printf("8. Exit\n");
        printf("Select an option: ");
        
        if (scanf("%d", &choice) != 1) {
            choice = 0;
        }
        clear_input();

        switch(choice) {
            case 1: add_student(&list); break;
            case 2: display_all(&list); break;
            case 3: search_and_display(&list); break;
            case 4: update_student(&list); break;
            case 5: delete_student(&list); break;
            case 6: display_analytics(&list); break;
            case 7: display_sorted(&list); break;
            case 8: 
                free(list.data);
                printf("Memory released. Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 8);

    return 0;
}
