#include <stdio.h>
#include <string.h>
#include <errno.h>

void
print_error (void) {
  printf("Problem connecting socket: %s\n", strerror (errno));
}

void
dump_sin (unsigned char *dp) {
  int i;

  printf("Dump of sockaddr_in =\n");
  for(i=0; i<16; i++)
    printf("%x ", *dp++);
  printf("\n");
}
