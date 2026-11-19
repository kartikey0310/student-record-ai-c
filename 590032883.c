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

/* CGPA = average of the SGPA values that are available.
   Returns NOT_AVAILABLE (-1) if the student has no SGPA yet. */
static double calculate_cgpa(const Student *s)
{
    double sum = 0.0;
    int count = 0;
    int i;

    for (i = 0; i < NUM_SEMESTERS; i++) {
        if (s->sgpa[i] >= 0.0) {
            sum += s->sgpa[i];
            count++;
        }
    }

    if (count == 0) {
        return NOT_AVAILABLE;
    }
    return sum / count;
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
    {
        double cgpa = calculate_cgpa(s);
        if (cgpa >= 0.0) {
            printf("  CGPA     : %.2f\n", cgpa);
        } else {
            printf("  CGPA     : not available\n");
        }
    }
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

static void search_student(void)
{
    long sap_id;
    int index;

    printf("\n--- Search Student ---\n");
    sap_id = read_long("Enter SAP ID to search: ", 10000000L, 999999999L);

    index = find_index_by_sap(sap_id);
    if (index == -1) {
        printf("No student found with SAP ID %ld.\n", sap_id);
    } else {
        printf("Student found:\n");
        print_student(&students[index]);
    }
}

static void delete_student(void)
{
    long sap_id;
    int index;
    int i;
    char answer[LINE_SIZE];

    printf("\n--- Delete Student ---\n");
    sap_id = read_long("Enter SAP ID to delete: ", 10000000L, 999999999L);

    index = find_index_by_sap(sap_id);
    if (index == -1) {
        printf("No student found with SAP ID %ld.\n", sap_id);
        return;
    }

    printf("You are about to delete:\n");
    print_student(&students[index]);
    read_line("Are you sure? (y/n): ", answer, sizeof(answer));

    if (answer[0] != 'y' && answer[0] != 'Y') {
        printf("Delete cancelled.\n");
        return;
    }

    /* Shift every later record one place to the left */
    for (i = index; i < student_count - 1; i++) {
        students[i] = students[i + 1];
    }
    student_count--;
    printf("Record deleted. Total records: %d\n", student_count);
}

/* Adds 5 ready-made students so the program can be tested quickly. */
static void load_sample_data(void)
{
    static const Student samples[5] = {
        { 500012301L, "R2142250101", "Rohan Gupta",  "9811122233", "rohan.gupta@example.com",
          4, 8, { 8.8, 9.1, 8.6, 9.0, 8.9, 9.2, 8.7, 9.3 } },
        { 500012302L, "R2142250102", "Kabir Singh",  "9822233344", "kabir.singh@example.com",
          2, 3, { 6.4, 5.9, 6.2, -1.0, -1.0, -1.0, -1.0, -1.0 } },
        { 500012303L, "R2142250103", "Aarav Sharma", "9876543210", "aarav.sharma@example.com",
          1, 2, { 9.2, 8.9, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 } },
        { 500012304L, "R2142250104", "Diya Verma",   "9833344455", "diya.verma@example.com",
          2, 4, { 8.1, 7.6, 7.9, 7.4, -1.0, -1.0, -1.0, -1.0 } },
        { 500012305L, "R2142250105", "Meera Nair",   "9844455566", "meera.nair@example.com",
          3, 5, { 5.2, 5.5, 4.9, 5.1, 4.8, -1.0, -1.0, -1.0 } }
    };
    int i;
    int added = 0;

    for (i = 0; i < 5; i++) {
        if (student_count < MAX_STUDENTS && find_index_by_sap(samples[i].sap_id) == -1) {
            students[student_count] = samples[i];
            student_count++;
            added++;
        }
    }
    printf("\n%d sample student(s) loaded. Total records: %d\n", added, student_count);
}

/* Shows the CGPA of every student in a small table. */
static void show_cgpa_of_all(void)
{
    int i;
    double cgpa;

    printf("\n--- CGPA of Each Student ---\n");
    if (student_count == 0) {
        printf("  No records to show.\n");
        return;
    }

    printf("  %-11s %-22s %s\n", "SAP ID", "Name", "CGPA");
    for (i = 0; i < student_count; i++) {
        cgpa = calculate_cgpa(&students[i]);
        if (cgpa >= 0.0) {
            printf("  %-11ld %-22s %.2f\n", students[i].sap_id, students[i].name, cgpa);
        } else {
            printf("  %-11ld %-22s not available\n", students[i].sap_id, students[i].name);
        }
    }
}

/* Average of the students' CGPA values, separately for each year. */
static void show_yearwise_average(void)
{
    int year;
    int i;
    int count;
    double sum;
    double cgpa;

    printf("\n--- Average CGPA of Each Year ---\n");
    for (year = 1; year <= 4; year++) {
        sum = 0.0;
        count = 0;
        for (i = 0; i < student_count; i++) {
            if (students[i].year == year) {
                cgpa = calculate_cgpa(&students[i]);
                if (cgpa >= 0.0) {
                    sum += cgpa;
                    count++;
                }
            }
        }
        if (count > 0) {
            printf("  Year %d : %d student(s), average CGPA = %.2f\n", year, count, sum / count);
        } else {
            printf("  Year %d : no students with a CGPA\n", year);
        }
    }
}

/* Compares two names ignoring upper/lower case (like strcmp). */
static int compare_names(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Bubble sort by SAP ID, smallest first. */
static void sort_by_sap(Student list[], int n)
{
    int i, j;
    Student temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (list[j].sap_id > list[j + 1].sap_id) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

/* Bubble sort by name, A to Z. */
static void sort_by_name(Student list[], int n)
{
    int i, j;
    Student temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (compare_names(list[j].name, list[j + 1].name) > 0) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

/* Sorting works on a copy, so the original order of records is not changed. */
static void display_sorted_by_sap(void)
{
    Student copy[MAX_STUDENTS];

    memcpy(copy, students, sizeof(Student) * student_count);
    sort_by_sap(copy, student_count);
    display_list(copy, student_count, "Records in Increasing Order of SAP ID");
}

static void display_sorted_by_name(void)
{
    Student copy[MAX_STUDENTS];

    memcpy(copy, students, sizeof(Student) * student_count);
    sort_by_name(copy, student_count);
    display_list(copy, student_count, "Records in Increasing Order of Name");
}

/* ------------------------------------------------------------------ */
/*  Part B: AI-Assisted Academic Advisor                               */
/*  The four categories and the CGPA limits were first suggested by an */
/*  AI tool and then checked/adjusted by the student (see              */
/*  AI_USAGE_LOG.md). The recommendation itself is produced by the     */
/*  if-else statements below - no AI is called while the program runs. */
/* ------------------------------------------------------------------ */
static void academic_advisor(void)
{
    long sap_id;
    int index;
    int i;
    int weakest = -1;
    int last = -1;
    int prev = -1;
    double cgpa;
    const Student *s;
    const char *category;
    const char *advice;

    printf("\n--- AI-Assisted Academic Advisor ---\n");
    sap_id = read_long("Enter SAP ID of the student: ", 10000000L, 999999999L);

    index = find_index_by_sap(sap_id);
    if (index == -1) {
        printf("No student found with SAP ID %ld.\n", sap_id);
        return;
    }

    s = &students[index];
    cgpa = calculate_cgpa(s);
    if (cgpa < 0.0) {
        printf("%s has no SGPA available, so no advice can be given yet.\n", s->name);
        return;
    }

    /* Main decision: choose the category from the CGPA */
    if (cgpa >= 8.5) {
        category = "Excellent Performance";
        advice = "Excellent performance - maintain your current study pattern and try advanced projects or research.";
    } else if (cgpa >= 7.0) {
        category = "Good Performance";
        advice = "Good performance - keep revising regularly and work on your weaker subjects to move higher.";
    } else if (cgpa >= 5.5) {
        category = "Needs Improvement";
        advice = "Needs improvement - focus more on subjects with lower SGPA and follow a weekly study plan.";
    } else {
        category = "Requires Immediate Attention";
        advice = "Requires immediate attention - meet your faculty advisor, attend extra classes and start remedial study now.";
    }

    printf("Student  : %s (SAP ID %ld)\n", s->name, s->sap_id);
    printf("CGPA     : %.2f\n", cgpa);
    printf("Category : %s\n", category);
    printf("Advice   : %s\n", advice);

    /* Extra hints from the individual SGPA values */
    for (i = 0; i < NUM_SEMESTERS; i++) {
        if (s->sgpa[i] >= 0.0) {
            if (weakest == -1 || s->sgpa[i] < s->sgpa[weakest]) {
                weakest = i;
            }
        }
    }
    if (cgpa < 7.0) {
        printf("Hint     : Weakest result so far is semester %d (SGPA %.2f) - revise its subjects first.\n",
               weakest + 1, s->sgpa[weakest]);
    }

    /* Find the two most recent available SGPA values */
    for (i = NUM_SEMESTERS - 1; i >= 0; i--) {
        if (s->sgpa[i] >= 0.0) {
            if (last == -1) {
                last = i;
            } else if (prev == -1) {
                prev = i;
            }
        }
    }
    if (prev != -1) {
        if (s->sgpa[last] < s->sgpa[prev]) {
            printf("Trend    : Latest SGPA (%.2f) is lower than the one before (%.2f) - find out what went wrong.\n",
                   s->sgpa[last], s->sgpa[prev]);
        } else if (s->sgpa[last] > s->sgpa[prev]) {
            printf("Trend    : Latest SGPA (%.2f) is higher than the one before (%.2f) - good progress.\n",
                   s->sgpa[last], s->sgpa[prev]);
        } else {
            printf("Trend    : Latest SGPA is the same as the one before (%.2f) - steady.\n",
                   s->sgpa[last]);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Menu and main                                                      */
/* ------------------------------------------------------------------ */

static void print_menu(void)
{
    printf("\n=========== STUDENT RECORD & RESULT MANAGEMENT ===========\n");
    printf("  1. Add student\n");
    printf("  2. Display all students\n");
    printf("  3. Search student by SAP ID\n");
    printf("  4. Delete student\n");
    printf("  5. Load 5 sample students (for testing)\n");
    printf("  6. Calculate CGPA of each student\n");
    printf("  7. Average CGPA of each year\n");
    printf("  8. Display students sorted by SAP ID\n");
    printf("  9. Display students sorted by Name\n");
    printf(" 10. Academic advisor (CGPA-based advice)\n");
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
            case 3:  search_student();         break;
            case 4:  delete_student();         break;
            case 5:  load_sample_data();       break;
            case 6:  show_cgpa_of_all();       break;
            case 7:  show_yearwise_average();  break;
            case 8:  display_sorted_by_sap();  break;
            case 9:  display_sorted_by_name(); break;
            case 10: academic_advisor();       break;
            case 0:  printf("Thank you. Goodbye!\n"); break;
            default: printf("That option is not available yet.\n"); break;
        }
    } while (choice != 0);

    return 0;
}
