/*
 * ====================================================================
 *  AI-Assisted Student Record and Result Management System
 * ====================================================================
 *  University : University of Petroleum and Energy Studies (UPES)
 *  Course     : Programming in C (PRC1001)
 *  Student    : KARTIKEY 
 *  SAP ID     : 590032883
 *
 *  A console, menu-driven program that keeps student records in an
 *  array of structures, calculates CGPA, sorts and searches records,
 *  and gives simple academic advice based on CGPA (if-else logic).
 *
 *  Build : gcc -Wall -Wextra -o student_record 590032883.c
 *  Run   : ./student_record            (add --echo to echo typed input)
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS   100
#define NUM_SEMESTERS  8
#define NOT_AVAILABLE  (-1.0)   /* stored in sgpa[] when a result is not available */
#define LINE_SIZE      128

/* One student record */
typedef struct {
    long   sap_id;
    char   roll_no[20];
    char   name[50];
    char   phone[16];
    char   email[50];
    int    year;                   /* 1 to 4 */
    int    semester;               /* current semester, 1 to 8 */
    double sgpa[NUM_SEMESTERS];    /* sgpa[0] = semester 1 ... NOT_AVAILABLE if no result yet */
} Student;

static Student students[MAX_STUDENTS];   /* all records live here */
static int     student_count = 0;        /* how many records are in use */
static int     echo_input    = 0;        /* 1 when run with --echo */

/* ------------------------------------------------------------------ */
/*  Safe input helpers (every input is read with fgets, then checked)  */
/* ------------------------------------------------------------------ */

/* Shows a prompt and reads one line of text without the newline. */
static void read_line(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, size, stdin) == NULL) {
        printf("\nInput ended. Exiting program.\n");
        exit(0);
    }

    /* If the line was longer than the buffer, throw away the rest of it */
    if (strchr(buffer, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (echo_input) {
        printf("%s\n", buffer);
    }
}

/* Keeps asking until the user types a whole number between min and max. */
static long read_long(const char *prompt, long min, long max)
{
    char line[LINE_SIZE];
    char *end;
    long value;

    while (1) {
        read_line(prompt, line, sizeof(line));
        value = strtol(line, &end, 10);
        if (end != line && *end == '\0' && value >= min && value <= max) {
            return value;
        }
        printf("  Invalid input. Please enter a whole number from %ld to %ld.\n", min, max);
    }
}

static int read_int(const char *prompt, int min, int max)
{
    return (int)read_long(prompt, min, max);
}

/* Reads one SGPA: a number from 0 to 10, or -1 when it is not available. */
static double read_sgpa(int semester)
{
    char prompt[80];
    char line[LINE_SIZE];
    char *end;
    double value;

    snprintf(prompt, sizeof(prompt),
             "  SGPA of semester %d (0-10, or -1 if not available): ", semester);

    while (1) {
        read_line(prompt, line, sizeof(line));
        value = strtod(line, &end);
        if (end != line && *end == '\0' &&
            (value == -1.0 || (value >= 0.0 && value <= 10.0))) {
            return value;
        }
        printf("  Invalid SGPA. Enter a value from 0 to 10, or -1 if not available.\n");
    }
}

/* Keeps asking until the user types at least one character. */
static void read_nonempty(const char *prompt, char *buffer, int size)
{
    while (1) {
        read_line(prompt, buffer, size);
        if (buffer[0] != '\0') {
            return;
        }
        printf("  This field cannot be empty.\n");
    }
}

/* A phone number must be exactly 10 digits. */
static int is_valid_phone(const char *s)
{
    int i;

    if (strlen(s) != 10) {
        return 0;
    }
    for (i = 0; i < 10; i++) {
        if (!isdigit((unsigned char)s[i])) {
            return 0;
        }
    }
    return 1;
}

/* A simple e-mail check: something@something.something, no spaces. */
static int is_valid_email(const char *s)
{
    const char *at  = strchr(s, '@');
    const char *dot;

    if (at == NULL || at == s || strchr(s, ' ') != NULL) {
        return 0;
    }
    dot = strrchr(at, '.');
    return dot != NULL && dot > at + 1 && dot[1] != '\0';
}

/* Returns the position of the student with this SAP ID, or -1 if absent. */
static int find_index_by_sap(long sap_id)
{
    int i;

    for (i = 0; i < student_count; i++) {
        if (students[i].sap_id == sap_id) {
            return i;
        }
    }
    return -1;
}


/* Prints one complete record. */
static void print_student(const Student *s)
{
    int i;

    printf("  SAP ID   : %ld\n", s->sap_id);
    printf("  Roll No. : %s\n", s->roll_no);
    printf("  Name     : %s\n", s->name);
    printf("  Phone    : %s\n", s->phone);
    printf("  Email    : %s\n", s->email);
    printf("  Year     : %d     Semester: %d\n", s->year, s->semester);

    printf("  SGPA     :");
    for (i = 0; i < NUM_SEMESTERS; i++) {
        if (s->sgpa[i] >= 0.0) {
            printf(" S%d=%.2f", i + 1, s->sgpa[i]);
        } else {
            printf(" S%d=--", i + 1);
        }
    }
    printf("\n");
}

/* Prints a list of records under a heading (used by all display options). */
static void display_list(const Student list[], int n, const char *title)
{
    int i;

    printf("\n--- %s (%d record%s) ---\n", title, n, n == 1 ? "" : "s");
    if (n == 0) {
        printf("  No records to show.\n");
        return;
    }
    for (i = 0; i < n; i++) {
        printf("\nRecord %d\n", i + 1);
        print_student(&list[i]);
    }
}

/* ------------------------------------------------------------------ */
/*  Menu option functions                                              */
/* ------------------------------------------------------------------ */

static void add_student(void)
{
    Student s;
    char prompt[40];
    int i;

    if (student_count >= MAX_STUDENTS) {
        printf("Storage is full. Delete a record before adding a new one.\n");
        return;
    }

    printf("\n--- Add New Student ---\n");

    s.sap_id = read_long("SAP ID (8 or 9 digits): ", 10000000L, 999999999L);
    if (find_index_by_sap(s.sap_id) != -1) {
        printf("A student with SAP ID %ld already exists. Record not added.\n", s.sap_id);
        return;
    }

    read_nonempty("Roll No.: ", s.roll_no, sizeof(s.roll_no));
    read_nonempty("Name: ", s.name, sizeof(s.name));

    while (1) {
        read_nonempty("Phone No. (10 digits): ", s.phone, sizeof(s.phone));
        if (is_valid_phone(s.phone)) {
            break;
        }
        printf("  Invalid phone number. It must contain exactly 10 digits.\n");
    }

    while (1) {
        read_nonempty("Email ID: ", s.email, sizeof(s.email));
        if (is_valid_email(s.email)) {
            break;
        }
        printf("  Invalid email. Use a format like name@example.com.\n");
    }

    s.year = read_int("Year (1-4): ", 1, 4);

    /* Year 1 -> semesters 1-2, year 2 -> 3-4, year 3 -> 5-6, year 4 -> 7-8 */
    snprintf(prompt, sizeof(prompt), "Semester (%d or %d): ", 2 * s.year - 1, 2 * s.year);
    s.semester = read_int(prompt, 2 * s.year - 1, 2 * s.year);

    printf("Enter the SGPA for all 8 semesters:\n");
    for (i = 0; i < NUM_SEMESTERS; i++) {
        s.sgpa[i] = read_sgpa(i + 1);
    }

    students[student_count] = s;
    student_count++;
    printf("Student added successfully. Total records: %d\n", student_count);
}

static void display_all_students(void)
{
    display_list(students, student_count, "All Student Records");
}




/* ------------------------------------------------------------------ */
/*  Menu and main                                                      */
/* ------------------------------------------------------------------ */

static void print_menu(void)
{
    printf("\n=========== STUDENT RECORD & RESULT MANAGEMENT ===========\n");
    printf("  1. Add student\n");
    printf("  2. Display all students\n");
    printf("  0. Exit\n");
    printf("==========================================================\n");
}

int main(int argc, char *argv[])
{
    int choice;

    if (argc > 1 && strcmp(argv[1], "--echo") == 0) {
        echo_input = 1;
    }

    do {
        print_menu();
        choice = read_int("Enter your choice: ", 0, 10);

        switch (choice) {
            case 1:  add_student();            break;
            case 2:  display_all_students();   break;
            case 0:  printf("Thank you. Goodbye!\n"); break;
            default: printf("That option is not available yet.\n"); break;
        }
    } while (choice != 0);

    return 0;
}
