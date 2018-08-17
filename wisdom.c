#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

char greeting[] = "\nHello there\n1. Receive wisdom\n2. Add wisdom\nSelection >";
char prompt[] = "Enter some wisdom\n";
char pat[] = "Achievement unlocked!\n";
char secret[] = "secret key";
char* wisdoms[] = { "Don't drink coffee past 4 PM unless you want to stay awake until the AM", "A great shell to use is /bin/bash", "If your ball is too big for your mouth, it's not your ball" };

int infd = 0; /* stdin */
int outfd = 1; /* stdout */

#define DATA_SIZE 128

typedef struct _WisdomList {
  struct  _WisdomList *next;
  char    data[DATA_SIZE];
} WisdomList; 

struct _WisdomList  *head = NULL;

typedef void (*fptr)(void);

void write_secret(void) {
  write(outfd, secret, sizeof(secret));
  return;
}

void shell(){
    /* Debug use only, delete this before production use! */
    /* UPDATE: Nevermind, I deleted option 3, let's just keep this in for when we need it!!111 */

    gid_t gid = getegid();
    setresgid(gid, gid, gid);
    system("/bin/sh -i");
}

void get_wisdom(void) {
  char buf[] = "no wisdom\n";
  if(head == NULL) {
    write(outfd, buf, sizeof(buf)-sizeof(char));
  } else {
    WisdomList  *l = head;
    while(l != NULL) {
      write(outfd, l->data, strlen(l->data));
      write(outfd, "\n", 1);
      l = l->next;
    }
  }
  return;
}

void add_wisdom(char wis[]) {
  WisdomList  *l = malloc(sizeof(WisdomList));

  if(l != NULL) {
    memset(l, 0, sizeof(WisdomList));
    strcpy(l->data, wis);
    if(head == NULL) {
      head = l;
    } else {
      WisdomList  *v = head;
      while(v->next != NULL) {
        v = v->next;
      }
      v->next = l;
    }
  }

  return;
}

void put_wisdom(void) {
  char  wis[DATA_SIZE] = {0}; 
  int   r;

  r = write(outfd, prompt, sizeof(prompt)-sizeof(char));
  if(r < 0) {
    return;
  }
 
  r = (int)gets(wis); 
  if (r == 0)
    return;

  WisdomList  *l = malloc(sizeof(WisdomList));

  if(l != NULL) {
    memset(l, 0, sizeof(WisdomList));
    strcpy(l->data, wis);
    if(head == NULL) {
      head = l;
    } else {
      WisdomList  *v = head;
      while(v->next != NULL) {
        v = v->next;
      }
      v->next = l;
    }
  }

  return;
}

fptr  ptrs[3] = { NULL, get_wisdom, put_wisdom }; // shell };

int main(int argc, char *argv[]) {
  
  int i;
  for (i=0; i<3; i++) {
    add_wisdom(wisdoms[i]);
  }

  while(1) {
      char  buf[1024] = {0};
      int r;
      fptr p = shell;
      r = write(outfd, greeting, sizeof(greeting)-sizeof(char));
      if(r < 0) {
        break;
      }
      r = read(infd, buf, sizeof(buf)-sizeof(char));
      if(r > 0) {
        buf[r] = '\0';
        int s = atoi(buf);
        fptr tmp = ptrs[s];
        tmp();
      } else {
        break;
      }
  }

  return 0;
}
