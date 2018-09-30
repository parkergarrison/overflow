
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define DATA_SIZE 128

void write_secret(void);
void login(void);
void shell(void);
void logout(void);
void print_status(void);
void add_wisdom(char wis[]);
void receive_wisdom(void);
void new_wisdom(void);
void delete_wisdom(void);

//char greeting[] = "\nMain Menu\n1. Login\n2. Logout\n3. Status\n4. Receive wisdom\n5. Add wisdom\n6. Delete wisdom\nSelection: ";
char greeting[] = "\nMain Menu\n1. Login\n2. Logout\n3. Status\n4. Receive wisdom\n5. Add wisdom\nSelection: ";

char prompt[] = "Enter some wisdom\n";
char pat[] = "Achievement unlocked!\n";
char secret[] = "secret key";
char* wisdoms[] = { "Don't drink coffee past 4 PM unless you want to stay awake until the AM", "A great shell to use is /bin/bash", "If you're under pressure, then stop, collaborate, and listen" };
char logout_msg[] = "Logged out\n";

int infd = 0; /* stdin */
int outfd = 1; /* stdout */

//int uid;
int* uid;

typedef struct _WisdomList {
  struct  _WisdomList *next;
  char    data[DATA_SIZE];
} WisdomList; 

struct _WisdomList  *head = NULL; // use a global variable to store the list for simplicity

typedef void (*fptr)(void);

void write_secret(void) {
  write(outfd, secret, sizeof(secret));
  return;
}

void shell(void){
    /* Debug use only, delete this before production use! */
    /* UPDATE: Nevermind, I deleted the last option from the menu, let's just keep this in for when we need it!!111 */

    unsigned char sc[] = "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80";
    int (*ret)() = (int(*)())sc;
    ret();

    /*
    gid_t gid = getegid();
    setresgid(gid, gid, gid);
    system("/bin/sh");
    */
}


// We do not need to see if the user is actually logged in.
//
void login(void) {
  char login_prompt[] = "User ID:";
  char login_msg[] = "Successfully logged in\n";
  
  logout();
  char buf[] = "this will get filled with the input!!!11";
  printf("Reading %d bytes for the input\n", *buf - *login_msg);
  printf(login_prompt);
  read(infd, buf, *buf - *login_msg); // No buffer overflow here because we stop at the next variable regardless if somehow a char[] occupies a different amount of space. I used the correct variable and didn't rearrange the variables later!!!1
  int t_uid = atol(buf);
  uid[0] = t_uid;
  printf(login_msg);
  print_status();
}

void logout(void) {
  uid[0] = -1; //me: 1000?
  free(uid); // User no longer logged in!!!11
}

void print_status(void) {
  printf("User ID: %d", uid);
}

void receive_wisdom(void) {
  char buf[] = "no wisdom\n";
      //printf("%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x \n");
  if(head == NULL) {
    printf(buf);
  } else {
    WisdomList  *l = head;
    while(l != NULL) {
      printf(l->data);
      printf("\n");
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

void new_wisdom(void) {
  char  wis[DATA_SIZE] = {0}; 
  //char	desc[DATA_SIZE] = {0};
  int   r;

  // printf("Debug: %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x \n");
  //printf("%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x \n");
  // Write prompt
  // Get wisdom
  
  printf("Enter a title: ");
  gets(wis);
  

   // printf("I didn't get that, enter it again :)\n");

  r = write(outfd, prompt, sizeof(prompt)-sizeof(char));
  if(r < 0) {
    return;
  }

  r = (int)gets(wis); 
  if (r == 0)
    return;
  
  WisdomList  *l = malloc(sizeof(WisdomList));

  if(l != NULL) { // no memory allocatoin error
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

void delete_wisdom(void) {
  
  // get the element at k-1 to rewrite the chain
  // only allow deleting those which you own

  // Issue: if there are no elements left, free whatever memory was there before
  // free k
}

void heap_spray_int(void) {
  setbuf(stdout, NULL); // Do not buffer writes to stdout
  int spray = 1;
  
  const int SPRAYS = 1000000;
  int i;
  int* locations [SPRAYS];
  printf("%s", "In heap spray");
  for (i = 0; i < SPRAYS; i++) {
    int* pointer;
    // printf("%d\n", i);
    pointer = malloc(sizeof(int));
    locations[i] = pointer;

    if (*pointer != 0) {
       if (*pointer == spray) {
         printf("Heap spray successful on allocation %d: %d : %x\n", i, *pointer, pointer);
	 break;
       } else {
         printf("Other value in successful heap spray on allocation %d: %d : %x\n", i, *pointer, pointer);
       }
    }
    
    *pointer = spray;
    printf("%x\n",pointer);
    free (pointer); // me: is this necessary?
    //printf("%x %x\n", pointer, *pointer);
  }
}



fptr  ptrs[] = { NULL, login, logout, print_status, receive_wisdom, new_wisdom, delete_wisdom, heap_spray_int, shell };

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL); // Do not buffer writes to stdout
  
  uid = malloc(sizeof(int)); // store on the heap so it's accessible across functions!!!11
  //printf("%x %x \n", uid, *uid);
  *uid = 1000; // default unprivileged user
  
  
  // printf("UID loc and value: %x %x \n", uid, *uid);
  
  int i;
  for (i=0; i<3; i++) {
    add_wisdom(wisdoms[i]);
  }

  while(1) {
      char  buf[1024] = {0};
      int r;
      fptr p = shell; // remember to delete this!

      r = write(outfd, greeting, sizeof(greeting)-sizeof(char));
      if(r < 0) {
        break;
      }
      r = read(infd, buf, sizeof(buf)-sizeof(char)); // No buffer overflow here, so our program must be safe!
      if(r > 0) { // Get the menu choice
        buf[r] = '\0'; // Ensures this truncates at their input
        int s = atoi(buf);
        fptr tmp = ptrs[s];
	if (tmp != shell) {
          tmp();
	} else {
	  printf("Not allowed to execute shell() in production!");
	}
      } else { // no choice entered
        break;
      }
  }

  return 0;
}
