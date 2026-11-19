# AI-Assisted Student Record and Result Management System

**Course:** Programming in C (PRC1001) – B.Tech CSE, Semester I
**University:** University of Petroleum and Energy Studies (UPES)
**Student:** `KARTIKEY`  |  **SAP ID:** `590032883`

A console-based, menu-driven program written in C that stores student records, calculates CGPA, sorts and searches records, and gives simple academic advice from a student's CGPA. An AI tool was used as a *learning assistant* while designing the advisor logic (see [AI\_USAGE\_LOG.md](AI_USAGE_LOG.md)); the program itself does not call any AI service.

## Repository structure

```
student-record-ai-c/
├── 590032883.c            # the complete C program
├── README.md             # this file
├── AI\_USAGE\_LOG.md       # which AI tool was used, the prompts, and how answers were verified
└── tests/
    ├── sample\_input.txt  # typed input used for the sample run
    └── sample\_output.txt # output produced by that input
```

## How to compile and run

You need a C compiler such as GCC (Linux/macOS, or MinGW / Code::Blocks on Windows).

```bash
gcc -Wall -Wextra -o student\_record 590032883.c
./student\_record            # on Windows: student\_record.exe
```

To reproduce the sample output exactly (the `--echo` flag prints each typed line so the output file reads like a real session):

```bash
./student\_record --echo < tests/sample\_input.txt > tests/sample\_output.txt
```

## Menu options

|#|Option|What it does|
|-|-|-|
|1|Add student|Reads and validates all details of one student and stores them|
|2|Display all students|Shows every record (with CGPA) in the order they were added|
|3|Search student by SAP ID|Finds and displays one record|
|4|Delete student|Removes one record after a y/n confirmation|
|5|Load 5 sample students|Adds ready-made records for quick testing|
|6|Calculate CGPA of each student|Table of SAP ID, name and CGPA|
|7|Average CGPA of each year|Average of the students' CGPA for year 1, 2, 3 and 4|
|8|Display sorted by SAP ID|Increasing order of SAP ID|
|9|Display sorted by Name|Increasing order of name (A to Z, ignoring case)|
|10|Academic advisor|CGPA-based category and advice for one student|
|0|Exit|Ends the program|

## Data stored for every student

SAP ID, Roll No., Name, Phone No., Email ID, Year (1–4), Semester (1–8) and the SGPA of all eight semesters. An SGPA that is not available yet is entered as `-1` and is ignored in every calculation. Records are kept in an array of `struct Student` (maximum 100) and exist only while the program is running.

**Input checks:** SAP ID must be 8–9 digits and unique; phone must be exactly 10 digits; email must look like `name@example.com`; year must be 1–4 and the semester must belong to that year (year 1 → semester 1 or 2, and so on); SGPA must be 0–10 or -1.

## How CGPA is calculated

CGPA = (sum of the available SGPA values) ÷ (number of available SGPA values).
The year-wise average is the average of the CGPA values of the students who belong to that year.

## Part B – AI-assisted academic advisor

The advisor is a plain `if / else if / else` decision in `academic\_advisor()`. The four categories were suggested with the help of an AI tool and then checked and adjusted (see the AI usage log).

|CGPA|Category|Message shown|
|-|-|-|
|8.5 and above|Excellent Performance|Maintain your current study pattern and try advanced projects or research.|
|7.0 to below 8.5|Good Performance|Keep revising regularly and work on your weaker subjects.|
|5.5 to below 7.0|Needs Improvement|Focus more on subjects with lower SGPA and follow a weekly study plan.|
|below 5.5|Requires Immediate Attention|Meet your faculty advisor, attend extra classes and start remedial study.|

For CGPA below 7.0 the program also names the semester with the lowest SGPA, and for every student it compares the last two SGPA values to say whether results are improving or dropping.

### Sample advisor output (from `tests/sample\_output.txt`)

|Student|CGPA|Category|
|-|-|-|
|Aarav Sharma|9.05|Excellent Performance|
|Diya Verma|7.75|Good Performance|
|Kabir Singh|6.17|Needs Improvement|
|Meera Nair|5.10|Requires Immediate Attention|

## Development history (GitHub workflow)

The project was built one feature at a time, with a commit after each step:

1. **Initial C program** – structure, input helpers, add student, display all, menu
2. **Student record functions** – search, delete, sample data
3. **Sorting and CGPA functions** – CGPA, year-wise average, sort by SAP ID and by name
4. **AI-assisted advisor and documentation** – advisor function, README, AI usage log, tests

GitHub Issue #1 (*Implement CGPA calculation*) was opened before step 3 and closed when the CGPA function was finished.

## Limitations and ideas for improvement

* Records are not saved to a file, so they disappear when the program closes (file handling with `fopen` / `fprintf` would fix this).
* Search works by SAP ID only; searching by name could be added.
* CGPA is a simple average of SGPA values, as required; a real university CGPA is weighted by credits.

