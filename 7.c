#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 2
#define MAX_INPUT_LENGTH 256

// Enums
typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef enum {
    STATUS_PENDING,
    STATUS_DONE
} TaskStatus;

// Task Structure
typedef struct {
    int id;
    char description[MAX_INPUT_LENGTH];
    TaskStatus status;
} Task;

// Global state for tasks
Task* tasks = NULL;
int task_count = 0;
int task_capacity = 0;

// Helper: Read a line of text securely, handling buffer overflow
int read_line(char* buffer, int max_length) {
    if (fgets(buffer, max_length, stdin) == NULL) {
        return 0; // EOF or error
    }
    
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        // Clear stdin buffer if the input was truncated
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }
    return 1;
}

// Helper: Convert string to uppercase for case-insensitive comparison
void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Function to convert string to CommandType
CommandType get_command_type(const char* commandStr) {
    char cmdCopy[MAX_INPUT_LENGTH];
    strncpy(cmdCopy, commandStr, sizeof(cmdCopy));
    cmdCopy[sizeof(cmdCopy) - 1] = '\0';
    to_uppercase(cmdCopy);

    if (strcmp(cmdCopy, "ADD") == 0) return CMD_ADD;
    if (strcmp(cmdCopy, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(cmdCopy, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(cmdCopy, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(cmdCopy, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(cmdCopy, "FIND") == 0) return CMD_FIND;
    if (strcmp(cmdCopy, "HELP") == 0) return CMD_HELP;
    if (strcmp(cmdCopy, "EXIT") == 0) return CMD_EXIT;
    
    return CMD_UNKNOWN;
}

// Helper: Find task index by ID
int find_task_index(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Command Handlers
void handle_add() {
    char input[MAX_INPUT_LENGTH];
    int id;

    printf("Enter Task ID (positive integer): ");
    if (!read_line(input, sizeof(input)) || sscanf(input, "%d", &id) != 1 || id <= 0) {
        printf("Error: Invalid task ID.\n");
        return;
    }

    if (find_task_index(id) != -1) {
        printf("Error: Task ID %d already exists.\n", id);
        return;
    }

    printf("Enter Description: ");
    if (!read_line(input, sizeof(input)) || strlen(input) == 0) {
        printf("Error: Description cannot be empty.\n");
        return;
    }

    if (task_count == task_capacity) {
        task_capacity = (task_capacity == 0) ? INITIAL_CAPACITY : task_capacity * 2;
        Task* new_tasks = realloc(tasks, task_capacity * sizeof(Task));
        if (!new_tasks) {
            printf("Error: Memory allocation failed.\n");
            return;
        }
        tasks = new_tasks;
    }

    tasks[task_count].id = id;
    strncpy(tasks[task_count].description, input, MAX_INPUT_LENGTH);
    tasks[task_count].status = STATUS_PENDING;
    task_count++;

    printf("Success: Task %d added.\n", id);
}

void handle_delete() {
    char input[MAX_INPUT_LENGTH];
    int id;

    printf("Enter Task ID to delete: ");
    if (!read_line(input, sizeof(input)) || sscanf(input, "%d", &id) != 1) {
        printf("Error: Invalid task ID.\n");
        return;
    }

    int idx = find_task_index(id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return;
    }

    for (int i = idx; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;
    printf("Success: Task %d deleted.\n", id);
}

void handle_view() {
    if (task_count == 0) {
        printf("No tasks available.\n");
        return;
    }
    printf("\n--- Task List ---\n");
    for (int i = 0; i < task_count; i++) {
        printf("[%s] ID: %d | %s\n", 
            tasks[i].status == STATUS_DONE ? "DONE" : "PENDING", 
            tasks[i].id, 
            tasks[i].description);
    }
    printf("-----------------\n");
}

void handle_update() {
    char input[MAX_INPUT_LENGTH];
    int id;

    printf("Enter Task ID to update: ");
    if (!read_line(input, sizeof(input)) || sscanf(input, "%d", &id) != 1) {
        printf("Error: Invalid task ID.\n");
        return;
    }

    int idx = find_task_index(id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return;
    }

    printf("Enter new description: ");
    if (!read_line(input, sizeof(input)) || strlen(input) == 0) {
        printf("Error: Description cannot be empty.\n");
        return;
    }

    strncpy(tasks[idx].description, input, MAX_INPUT_LENGTH);
    printf("Success: Task %d updated.\n", id);
}

void handle_complete() {
    char input[MAX_INPUT_LENGTH];
    int id;

    printf("Enter Task ID to complete: ");
    if (!read_line(input, sizeof(input)) || sscanf(input, "%d", &id) != 1) {
        printf("Error: Invalid task ID.\n");
        return;
    }

    int idx = find_task_index(id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return;
    }

    tasks[idx].status = STATUS_DONE;
    printf("Success: Task %d marked as DONE.\n", id);
}

void handle_find() {
    char input[MAX_INPUT_LENGTH];
    int id;

    printf("Enter Task ID to find: ");
    if (!read_line(input, sizeof(input)) || sscanf(input, "%d", &id) != 1) {
        printf("Error: Invalid task ID.\n");
        return;
    }

    int idx = find_task_index(id);
    if (idx == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return;
    }

    printf("\n--- Task Details ---\n");
    printf("ID: %d\n", tasks[idx].id);
    printf("Description: %s\n", tasks[idx].description);
    printf("Status: %s\n", tasks[idx].status == STATUS_DONE ? "DONE" : "PENDING");
    printf("--------------------\n");
}

void handle_help() {
    printf("\n--- Available Commands ---\n");
    printf("ADD      : Create a new task.\n");
    printf("DELETE   : Remove an existing task by ID.\n");
    printf("VIEW     : Display all tasks.\n");
    printf("UPDATE   : Change the description of a task.\n");
    printf("COMPLETE : Mark a task as DONE.\n");
    printf("FIND     : Display details of a specific task.\n");
    printf("HELP     : Show this command list.\n");
    printf("EXIT     : Terminate the program.\n");
    printf("--------------------------\n");
}

int main() {
    char input[MAX_INPUT_LENGTH];
    CommandType cmd;

    printf("Task Manager Initialized. Type HELP for commands.\n");

    while (1) {
        printf("\nEnter command: ");
        if (!read_line(input, sizeof(input))) {
            printf("\nEOF encountered. Exiting...\n");
            break;
        }

        // Handle empty lines gracefully
        if (strlen(input) == 0) continue;

        cmd = get_command_type(input);

        switch (cmd) {
            case CMD_ADD:
                handle_add();
                break;
            case CMD_DELETE:
                handle_delete();
                break;
            case CMD_VIEW:
                handle_view();
                break;
            case CMD_UPDATE:
                handle_update();
                break;
            case CMD_COMPLETE:
                handle_complete();
                break;
            case CMD_FIND:
                handle_find();
                break;
            case CMD_HELP:
                handle_help();
                break;
            case CMD_EXIT:
                printf("Exiting program and freeing memory.\n");
                if (tasks != NULL) {
                    free(tasks);
                }
                return 0;
            case CMD_UNKNOWN:
            default:
                printf("Unknown command. Type HELP for a list of valid commands.\n");
        }
    }

    if (tasks != NULL) {
        free(tasks);
    }
    return 0;
    /*Enter command: add
Enter Task ID (positive integer): 1
Enter Description: morning walk
Success: Task 1 added.

Enter command: view

--- Task List ---
[PENDING] ID: 1 | morning walk

//output
}
