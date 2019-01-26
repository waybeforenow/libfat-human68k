/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <stdio.h>
#include <time.h>
#include "fake_bootsect.h"

verbosity global_loglevel = LEVEL_ERROR | LEVEL_WARN | LEVEL_INFO
#ifdef _DEBUG
                            | LEVEL_DEBUG
#endif
    ;

const char *f_errstr(FRESULT r) {
  const char *errstrs[] = {
      "Succeeded",
      "A hard error occurred in the low level disk I/O layer",
      "Assertion failed",
      "The physical drive cannot work",
      "Could not find the file",
      "Could not find the path",
      "The path name format is invalid",
      "Access denied due to prohibited access or directory full",
      "Access denied due to prohibited access",
      "The file/directory object is invalid",
      "The physical drive is write protected",
      "The logical drive number is invalid",
      "The volume has no work area",
      "There is no valid FAT volume",
      "The f_mkfs() aborted due to any parameter error",
      "Could not get a grant to access the volume within defined period",
      "The operation is rejected according to the file sharing policy",
      "LFN working buffer could not be allocated",
      "Number of open files > _FS_SHARE",
      "Given parameter is invalid",
  };
  if (r <= 19) return errstrs[r];
  return "Unknown error";
}

// Global filesystem object.
human68k_fs *human68k_global_fs;

// Initialize a human68k fs.
DSTATUS disk_initialize(BYTE pdrv) {
  LOG_DEBUG("FatFs requested disk initialize");
  // TODO: open the file.

  return RES_OK;
}

// Get fs status.
DSTATUS disk_status(BYTE pdrv) {
  LOG_DEBUG("FatFs requested disk status");
  return RES_OK;
}

// Read data from the fs.
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
  LOGF_DEBUG("FatFs requested to read %u sectors with offset %lu", count,
             sector);
  if ((1 || (human68k_global_fs->hints & FS_REPLACE_SECTOR0)) && sector == 0) {
    // read from the fake boot-sector instead
    LOG_DEBUG("giving them the fake one");
    memcpy(buff, fake_bootsect, _MIN_SS);

    count--;
    sector++;
    buff += sizeof(BYTE) * _MIN_SS;
  }

  fseek(human68k_global_fs->fp,
        (human68k_global_fs->hints & FS_HAS_DIFC_HEADER)
            ? sector * _MIN_SS + DIFC_HEADER_SIZE
            : sector * _MIN_SS,
        SEEK_SET);
  fread(buff, _MIN_SS, count, human68k_global_fs->fp);
  return RES_OK;
}

// Write data to the fs.
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
  LOGF_DEBUG("FatFs requested to write %u sectors with offset %lu", count,
             sector);

  if ((human68k_global_fs->hints & FS_REPLACE_SECTOR0) && sector == 0) {
    // avoid writing to the boot-sector if it's being replaced
    count--;
    sector++;
    buff += sizeof(BYTE) * _MIN_SS;
  }

  if (count != 0) {
    fseek(human68k_global_fs->fp,
          (human68k_global_fs->hints & FS_HAS_DIFC_HEADER)
              ? sector * _MIN_SS + DIFC_HEADER_SIZE
              : sector * _MIN_SS,
          SEEK_SET);
    /*debug XXX*/
    char c;
    LOGF_DEBUG("File offset is %d", ftell(human68k_global_fs->fp));
    for (int i = 0; i < _MIN_SS; i++) {
      fread(&c, 1, 1, human68k_global_fs->fp);
      if (c != buff[i]) {
        LOGF_DEBUG("FatFs changed byte at offset %x from %x to %x", i, c,
                   buff[i]);
      }
    }
    fseek(human68k_global_fs->fp,
          (human68k_global_fs->hints & FS_HAS_DIFC_HEADER)
              ? sector * _MIN_SS + DIFC_HEADER_SIZE
              : sector * _MIN_SS,
          SEEK_SET);
    /*end debug XXX*/

    if (fwrite(buff, _MIN_SS, count, human68k_global_fs->fp) < count) {
      return 1; /* XXX */
    }
  }

  return RES_OK;
}

// IOCTL functions.
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  LOGF_DEBUG("FatFs requested ioctl %u", cmd);
  switch (cmd) {
    case CTRL_SYNC: {
      fflush(human68k_global_fs->fp);
      break;
    }
    case GET_SECTOR_COUNT: {
      *(UINT *)buff = 0xc800;
      break;
    }
    case GET_BLOCK_SIZE: {
      *(UINT *)buff = 1024;
    }
  }

  return RES_OK;
}

DWORD get_fattime(void) {
  time_t t = time(NULL);
  struct tm *tmr = localtime(&t);
  int year = tmr->tm_year < 80 ? 0 : tmr->tm_year - 80;
  return ((DWORD)(year) << 25) | ((DWORD)(tmr->tm_mon + 1) << 21) |
         ((DWORD)tmr->tm_mday << 16) | (WORD)(tmr->tm_hour << 11) |
         (WORD)(tmr->tm_min << 5) | (WORD)(tmr->tm_sec >> 1);
}

