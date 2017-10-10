#define DEBUG 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "insock.h"
#include "dbg.h"

void *receiveMsgs(void *sockfd);
