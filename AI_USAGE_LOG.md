# AI Usage Log

**Student:** `<YOUR NAME>`  |  **SAP ID:** `<YOUR SAP ID>`
**Project:** AI-Assisted Student Record and Result Management System (PRC1001)

This log records every use of an AI tool in this project: which tool, what was asked, and how the answer was used or checked. Only add rows for prompts you really asked.

## Log

| AI Tool | Prompt / Question Asked | How I Used / Verified the Response |
| --- | --- | --- |
| Claude (Anthropic) | I uploaded the project assignment and asked: "Can you do this project for me and tell me how to do it, how to upload to GitHub, and everything that is required?" | Received a first version of the program (struct array, menu, add/search/delete/display, CGPA, year-wise average, bubble-sort by SAP ID and name, advisor), README, tests and GitHub steps. I compiled it, read every function, and re-ran the sample input. **[EDIT: write what you personally changed or checked, e.g. "changed the Excellent limit from 8.5 to 9.0"]** |
| Claude (Anthropic) | **[REPLACE with your own prompt, e.g. "Suggest simple CGPA-based academic advice categories for a college student on a 10-point scale."]** | **[REPLACE: which categories and limits you got, and which you kept, changed or rejected and why]** |
| Claude (Anthropic) | **[REPLACE with your own prompt, e.g. "Give me test cases for a function that averages the available SGPA values, including a student with no SGPA."]** | **[REPLACE: which cases you tried by hand and whether any answer needed correcting]** |
| Claude (Anthropic) | **[REPLACE with your own prompt, e.g. "Explain how bubble sort works in my sort_by_sap function line by line."]** | **[REPLACE: what you learned and how you checked it, e.g. by tracing 3 records on paper]** |

## Verification checks (CGPA and advisor)

These hand calculations match the program output in `tests/sample_output.txt`:

| Student | SGPA values used | Hand calculation | CGPA | Category |
| --- | --- | --- | --- | --- |
| Aarav Sharma | 9.2, 8.9 | (9.2 + 8.9) / 2 | 9.05 | Excellent Performance |
| Diya Verma | 8.1, 7.6, 7.9, 7.4 | 31.0 / 4 | 7.75 | Good Performance |
| Kabir Singh | 6.4, 5.9, 6.2 | 18.5 / 3 | 6.17 | Needs Improvement |
| Meera Nair | 5.2, 5.5, 4.9, 5.1, 4.8 | 25.5 / 5 | 5.10 | Requires Immediate Attention |

Year-wise check: year 2 has Diya (7.75) and Kabir (6.17), so (7.75 + 6.1667) / 2 = 6.96, which is what the program prints.

## What I learned from using AI in this project

- **Writing a clear prompt:** **[EDIT in your own words]**
- **Verifying instead of copying:** **[EDIT in your own words, e.g. the limit values and every test case were checked by hand]**
- **Turning an idea into C code:** the category idea became one `if / else if / else` chain in `academic_advisor()`, with the CGPA limits as the conditions.
