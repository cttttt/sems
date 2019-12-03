#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

char *prog_name = NULL;

enum operation {
  ACQUIRE,
  RELEASE,
  DESTROY,
  UNKNOWN
};

int sem_op(char *name, enum operation op) {
  if (op == DESTROY) {
    return sem_unlink(name);
  }

  sem_t *sem = sem_open(name, O_CREAT, 0666, 1);

  if (sem == SEM_FAILED) {
    fprintf(stderr, "while opening semaphore, %s: %s\n", name, strerror(errno));
    return 1;
  }

  int res;

  switch (op) {
    case ACQUIRE:
      res = sem_wait(sem);
      break;
    case RELEASE:
      res = sem_post(sem);
      break;
    case DESTROY:
    case UNKNOWN:
      res = 0;
      break;
  }

  if (res != 0) {
    fprintf(stderr, "while operating on semaphore, %s: %s\n", name, strerror(errno));
    return res;
  }

  return sem_close(sem);
}

enum operation operation_from(char *name) {
  if (name == NULL) {
    return UNKNOWN;
  } else if (strcmp(name, "acquire") == 0) {
    return ACQUIRE;
  } else if (strcmp(name, "release") == 0) {
    return RELEASE;
  } else if (strcmp(name, "destroy") == 0) {
    return DESTROY;
  } else {
    return UNKNOWN;
  }
}

void usage() {
    printf("Usage: %s (acquire|release|destroy) NAME\n", prog_name);
}

int main(int argc, char *argv[]) {
  prog_name = argv[0];

  if (argc != 3) {
    usage();
    return 1;
  }
  
  enum operation op = operation_from(argv[1]);

  if (op == UNKNOWN) {
    usage();
    return 1;
  }

  char *name = argv[2];
  return sem_op(name, op);
}
