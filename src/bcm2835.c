#include "bcm2835.h"

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>

#define _perr(msg)    _log_err(__FUNCTION__, __LINE__, msg, -1)
#define _psyserr(msg) _log_err(__FUNCTION__, __LINE__, msg, errno)

int _MapPeripheral(__bcm_periph_s *p)
{
  int ret = -1;
  p->_mem_fd = open("/dev/gpiomem", O_SYNC|O_RDWR);
  if (p->_mem_fd < 0) {
    _psyserr("open /dev/gpiomem");
    return 0;
  } else {
    p->_map = mmap(NULL,
                  BLOCK_SIZE,
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  p->_mem_fd,
                  p->_bcm_hw_addr);

    if (p->_map != MAP_FAILED) {
      p->virt_addr = (volatile uint32_t *)p->_map;
      ret = 1;
    } else {
      _psyserr("mmap");
    }
  }

  return ret;
}

void _UnmapPeripheral(__bcm_periph_s *p)
{
  munmap(p->_map,
         BLOCK_SIZE);
  close(p->_mem_fd);
}

void _log_err(const char *func, const int line, char *msg, int errnum)
{
  if (errnum < 0)
    fprintf(stderr, "%s:%d: %s\n", func, line, msg);
  else
    fprintf(stderr, "%s:%d: %s: %s\n", func, line, msg, strerror(errnum));
}
