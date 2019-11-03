#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include <pthread.h>

#include "gl/logging/logging.h"
#include "gl/tester/student.h"
#include "gl/tester/problem.h"

#define MAX_PATH 1000
#define MAX_CMD 1000

#define STUDENTS_DIR "--students_dir="
#define PROBLEMS_DIR "--problems_dir="
#define MODE_CLASS "--class"
#define MODE_STUDENT "--student"
// TODO(giolekva): working
#define WORKING_DIR "--results_dir="

typedef enum {
	      STUDENT = 0,
	      CLASS = 1
} TesterMode;

typedef struct {
  TesterMode mode;
  char* students_dir;
  char* problems_dir;
  char* working_dir;
} TesterOpts;

#define STR_EQ(str, substr) (strcmp(str, substr) == 0)
#define STR_STARTS_WITH(str, substr) (strncmp(str, substr, strlen(substr)) == 0)

void TesterOptsInit(TesterOpts* opts, int argc, char* argv[]) {
  opts->mode = STUDENT;
  opts->students_dir = NULL;
  opts->problems_dir = NULL;
  opts->working_dir = NULL;
  bool mode_class = false;
  bool mode_student = false;
  for (int i = 1; i < argc; ++i) {
    if (STR_STARTS_WITH(argv[i], STUDENTS_DIR)) {
      opts->students_dir = argv[i] + strlen(STUDENTS_DIR);
    } else if (STR_STARTS_WITH(argv[i], PROBLEMS_DIR)) {
      opts->problems_dir = argv[i] + strlen(PROBLEMS_DIR);
    } else if(STR_STARTS_WITH(argv[i], WORKING_DIR)) {
      opts->working_dir = malloc(strlen(argv[i]) - strlen(WORKING_DIR) + 14);
      assert(opts->working_dir != NULL);
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);      
      sprintf(opts->working_dir, "%s/%.2d%.2d%.2d%.2d%.2d%.2d",
	      argv[i] + strlen(WORKING_DIR), tm.tm_mday, tm.tm_mon + 1,
	      tm.tm_year - 100, tm.tm_hour, tm.tm_min, tm.tm_sec);
      CHECK(mkdir(opts->working_dir, 0777) == 0);
    } else if (STR_EQ(argv[i], MODE_CLASS)) {
      mode_class = true;
    } else if (STR_EQ(argv[i], MODE_STUDENT)) {
      mode_student = true;
    }
  }
  CHECK(opts->students_dir);
  CHECK(opts->problems_dir);
  CHECK(!(mode_class && mode_student));
  if (mode_class) {
    opts->mode = CLASS;
  }
}

void TesterOptsDispose(TesterOpts* opts) {
  if (opts->working_dir != NULL) {
    free(opts->working_dir);
  }
}

int ExecCmd(char* name, char* cmd, List* logs) {
  LOG_INFO("%s: %s", name, cmd);
  FILE* out = popen(cmd, "r");
  char c;
  while ((c = getc(out)) != EOF) {
    if (logs != NULL) {
      for (int i = 0; i < logs->size; ++i) {
	FILE** log = ListGet(logs, i);
	putc(c, *log);
      }
    }
  }
  int status = pclose(out);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return 1;
}

int ExecCmdInWorkingDir(char* name, char* cmd, List* logs,
			const char* working_dir) {
  char new_cmd[10000];
  sprintf(new_cmd, "cd %s && %s", working_dir, cmd);
  return ExecCmd(name, new_cmd, logs);
}

void PrepareWorkingDir(Student* student, ProblemInfo* problem,
		       const char* student_dir, const char* working_dir) {
  List logs;
  ListInit(&logs, sizeof(FILE**), /*free_fn=*/NULL);
  // ListAdd(&logs, &stdout);
  char mkdir_cmd[MAX_CMD];
  sprintf(mkdir_cmd, "mkdir -p %s", working_dir);
  CHECK(!ExecCmd("Creating working directory", mkdir_cmd, &logs));
  sprintf(mkdir_cmd, "mkdir -p %s/logs", working_dir);
  CHECK(!ExecCmd("Creating logs directory", mkdir_cmd, &logs));  
  char cmd[MAX_CMD];
  sprintf(cmd, "cp -R %s/%s/* %s", student_dir, problem->id,
	  working_dir);
  ExecCmd("Copying student files", cmd, &logs);
  for (int i = 0; i < problem->files_to_preserve.size; ++i) {
    char* f = *(char**)ListGet(&problem->files_to_preserve, i);
    bool overwrite = (f[0] == '+');
    f += 2;
    char* pos = strrchr(f, '/');
    if (pos != NULL) {
      char* mkd = strndup(f, pos - f);
      sprintf(cmd, "mkdir -p %s/%s", working_dir, mkd);
      CHECK(!ExecCmd("Creating directory", cmd, &logs));
      free(mkd);
    }
    if (overwrite) {
      sprintf(cmd, "cp -f %s/%s %s/%s", problem->src_dir, f, working_dir, f);
    } else {
      sprintf(cmd, "cp -u %s/%s %s/%s", problem->src_dir, f, working_dir, f);
    }
    CHECK(!ExecCmd("Preserving original file", cmd, &logs));
  }
  ListDispose(&logs);
}

bool DetermineListOfTestsToRun(ProblemInfo* problem, const char* working_dir, List* test_names) {
  char buf[10000];
  FILE* fd = fmemopen(buf, 10000, "w");
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  // ListAdd(&log_streams, &stdout);
  ListAdd(&log_streams, &fd);
  char cmd[MAX_CMD];
  sprintf(cmd, "%s --list_tests", problem->test_binary);
  if (ExecCmdInWorkingDir("Determining list of tests to run", cmd, &log_streams, working_dir)) {
    fclose(fd);
    ListDispose(&log_streams);
    return false;
   }
  fclose(fd);
  ListDispose(&log_streams);
  for (int i = 0; buf[i] != '\0'; ) {
    int j = i;
    while (buf[j] != '\n' && buf[j] != '\0') {
      ++j;
    }
    char* name = strndup(buf + i, j - i);
    ListAdd(test_names, &name);
    i = j + 1;
  }
  return true;
}

void CalculateScore(ProblemInfo* problem, ProblemResult* result,
		    const char* working_dir) {
  #define RESULTS_FILE "logs/results"
  char results_file[1000];
  sprintf(results_file, "%s/%s", working_dir, RESULTS_FILE);
  FILE* results_fd = fopen(results_file, "w");
  assert(results_fd != NULL);
  for (int i = 0; i < result->tests.size; ++i) {
    TestResult* test = ListGet(&result->tests, i);
    fprintf(results_fd, "%s %d\n", test->name, test->succeeded);
  }
  fclose(results_fd);
  char buf[10000];
  FILE* fd = fmemopen(buf, 10000, "w");
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  // ListAdd(&log_streams, &stdout);
  ListAdd(&log_streams, &fd);
  char cmd[MAX_CMD];
  sprintf(cmd, "%s --results_file=%s", problem->test_binary, RESULTS_FILE);
  CHECK(!ExecCmdInWorkingDir("Calculating score", cmd, &log_streams, working_dir));
  fclose(fd);
  ListDispose(&log_streams);
  sscanf(buf, "%lf", &result->score);
}

void StrFree(void* pt) {
  free(*(char**)pt);
}

bool SolutionExists(Student* student, ProblemInfo* problem,
		    const char* student_dir) {
  char path[MAX_PATH];
  sprintf(path, "%s/%s", student_dir, problem->id);
  DIR* d = opendir(path);
  if (d != NULL) {
    closedir(d);
    return true;
  }
  return false;
}

void EvaluateStudentOnProblem(Student* student, ProblemInfo* problem,
			      const char* working_dir, ProblemResult* result) {
  // char cwd[MAX_PATH];
  // getcwd(cwd, MAX_PATH);
  FILE* logs_fd = NULL;
  List log_streams;
  ListInit(&log_streams, sizeof(FILE**), /*free_fn=*/NULL);
  // ListAdd(&log_streams, &stdout);
  char logs_file[MAX_PATH];
  sprintf(logs_file, "%s/logs/evaluation.logs", working_dir);
  logs_fd = fopen(logs_file, "w");
  ListAdd(&log_streams, &logs_fd);
  // chdir(working_dir);
  if (ExecCmdInWorkingDir("Building student solution", problem->cmd_make_lib,
			  &log_streams, working_dir)) {
    goto ret;
  }
  if (ExecCmdInWorkingDir("Building test", problem->cmd_make_test, &log_streams,
			  working_dir)) {
    goto ret;      
  }
  result->test_compiled = true;
  List test_names;
  ListInit(&test_names, sizeof(char*), StrFree);
  CHECK(DetermineListOfTestsToRun(problem, working_dir, &test_names));
  for (int j = 0; j < test_names.size; ++j) {
    char* name = *(char**)ListGet(&test_names, j);
    TestResult res;
    TestResultInit(&res, strdup(name));   
    char desc[MAX_CMD];
    sprintf(desc, "Running test %s", name);
    char cmd[MAX_CMD];
    sprintf(cmd, "timeout 5s %s --run_test=%s --crash_on_failure", problem->test_binary, name);
    res.succeeded = !ExecCmdInWorkingDir(desc, cmd, &log_streams, working_dir);
    sprintf(desc, "Checking test %s on memory", name);
    sprintf(cmd, "timeout 5s valgrind -s --leak-check=full --show-leak-kinds=all --error-exitcode=1 --log-file=logs/valgrind.logs %s --run_test=%s", problem->test_binary, name);
    res.memory = !ExecCmdInWorkingDir(desc, cmd, &log_streams, working_dir);    
    ListAdd(&result->tests, &res);
  }
  ListDispose(&test_names);
  CalculateScore(problem, result, working_dir);
 ret:
  if (logs_fd != NULL) {
    fclose(logs_fd);
  }
  ListDispose(&log_streams); 
  // chdir(cwd);
}

void OutputResultAsCsv(StudentList* students, TesterOpts* opts) {
  if (opts->working_dir == NULL) {
    return;
  }
  char path[MAX_PATH];
  sprintf(path, "%s/results.csv", opts->working_dir);
  FILE* out = fopen(path, "w");
  StudentListToCsv(students, out);
  fclose(out);
  LOG_INFO("--- RESULTS: %s", path);
}

typedef struct {
  TesterOpts* opts;
  Student* student;
  ProblemInfo* problem;
  ProblemResult* result;
} Args;

void* Eval(void* pt) {
  Args* args = pt;
  char student_dir[MAX_PATH];
  sprintf(student_dir, "%s/%s", args->opts->students_dir, args->student->id);
  if (SolutionExists(args->student, args->problem, student_dir)) {
    args->result->solution_found = true;
    LOG_INFO("*** Evaluating problem: %s", args->problem->id);
    char working_dir[MAX_PATH];
    sprintf(working_dir, "%s/%s/%s", args->opts->working_dir, args->student->id,
	    args->problem->id);
    PrepareWorkingDir(args->student, args->problem, student_dir, working_dir);
    EvaluateStudentOnProblem(args->student, args->problem, working_dir,
			     args->result);
    LOG_INFO("### Done evaluating problem: %s", args->problem->id);
  } else  {
    LOG_INFO("### Student %s did not provide solution on problem %s",
	     args->student->id, args->problem->id);
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  TesterOpts opts;
  TesterOptsInit(&opts, argc, argv);
  StudentList students;
  ListStudents(opts.students_dir, &students);
  StudentListLogInfo(&students);
  ProblemSet problems;
  ListProblemSet(opts.problems_dir, &problems);
  pthread_t tids[100000];
  Args args[100000];
  int cnt = 0;
  for (int i = 0; i < students.size; ++i) {
    Student* student = StudentListGet(&students, i);
    /* if (strcmp(student->id, "vgamez") != 0) { */
    /*   continue; */
    /* } */
    for (int j = 0; j < problems.size; ++j) {
      ProblemInfo* problem = ProblemSetGet(&problems, j);      
      LOG_INFO("*** Scheduling for evaluation: %s %s ***", student->id,
	       problem->id);            
      ProblemResult result;
      ProblemResultInit(&result, problem->id);
      ListAdd(&student->problems, &result);
      args[cnt].opts = &opts;
      args[cnt].student = student;
      args[cnt].problem = problem;
      args[cnt].result = ListGet(&student->problems,
				 student->problems.size - 1);
      pthread_create(&tids[cnt], NULL, Eval, &args[cnt]);
      cnt++;      
    }
  }
  for (int i = 0; i < cnt; ++i) {
    pthread_join(tids[i], NULL);
    LOG_INFO("### Done evaluating student: %s ###", args[i].student->id);    
  }
  StudentListLogResults(&students);
  OutputResultAsCsv(&students, &opts);
  StudentListDispose(&students);
  ProblemSetDispose(&problems);
  TesterOptsDispose(&opts);
  return 0;
}
