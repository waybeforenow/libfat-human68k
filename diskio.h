/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#include <stdint.h>
#include <stdio.h>
#include "ff.h"
#include "integer.h"

/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* Results of Disk Functions */
typedef enum {
  RES_OK = 0, /* 0: Successful */
  RES_ERROR,  /* 1: R/W Error */
  RES_WRPRT,  /* 2: Write Protected */
  RES_NOTRDY, /* 3: Not Ready */
  RES_PARERR  /* 4: Invalid Parameter */
} DRESULT;

/*---------------------------------------*/
/* Prototypes for disk control functions */
/*---------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv);
DSTATUS disk_status(BYTE pdrv);
DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff);

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT 0x01  /* Drive not initialized */
#define STA_NODISK 0x02  /* No medium in the drive */
#define STA_PROTECT 0x04 /* Write protected */

/* Command code for disk_ioctrl fucntion */

/* Generic command (used by FatFs) */
#define CTRL_SYNC 0        /* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT 1 /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE \
  2 /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE 3 /* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR \
  4 /* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_POWER 5  /* Get/Set power status */
#define CTRL_LOCK 6   /* Lock/Unlock media removal */
#define CTRL_EJECT 7  /* Eject media */
#define CTRL_FORMAT 8 /* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE 10   /* Get card type */
#define MMC_GET_CSD 11    /* Get CSD */
#define MMC_GET_CID 12    /* Get CID */
#define MMC_GET_OCR 13    /* Get OCR */
#define MMC_GET_SDSTAT 14 /* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV 20   /* Get F/W revision */
#define ATA_GET_MODEL 21 /* Get model name */
#define ATA_GET_SN 22    /* Get serial number */

#define _MIN_SS 1024

#define DIFC_HEADER_SIZE 0x100

typedef int8_t status;
#define OK 0
#define ERR_UNSPECIFIED -1
#define ERR_FILE -2
#define ERR_FILENAME -3
#define ERR_FS -3
#define ERR_DIRECTORY -4
#define ERR_MEMORY -5

typedef uint8_t human68k_fs_hints;
#define FS_USE_LFN 0x01
#define FS_FOLLOW_SYMLINKS 0x02
#define FS_REPLACE_SECTOR0 0x04
#define FS_HAS_DIFC_HEADER 0x10

typedef enum {
  DIM_UNKNOWN_FS_TYPE = -1,
  DIM_2HC1 = 0,
  DIM_2HD = 1,
  DIM_2HDA = 2,
  DIM_2HDE = 3,
  DIM_2HQ = 4,
  DIM_2HS = 5,
  HDF = 6,
  HDS = 7,
  XDF_2HD = 8
} human68k_fs_type;

typedef struct {
  TCHAR* label;
  DWORD vsn;
} human68k_fs_info;

typedef struct {
  char* path;     /* native encoding */
  char* basename; /* native encoding */
  FILE* fp;
  FATFS* fs;
  human68k_fs_info info;
  human68k_fs_hints hints;
  human68k_fs_type type;
} human68k_fs;

#ifndef LOG_STREAM
#define LOG_STREAM stderr
#endif

typedef uint8_t verbosity;
#define LEVEL_ERROR 0x01
#define LEVEL_WARN 0x02
#define LEVEL_INFO 0x04
#define LEVEL_DEBUG 0x10

const char* f_errstr(FRESULT r);

/* FATAL log entries can't be masked, and cause an immediate program
 * termination. */
#define LOGF_FATAL(fmt, ...)                                             \
  {                                                                      \
    fprintf(LOG_STREAM, "[FATAL] (%s:%d) " fmt "\n", __FILE__, __LINE__, \
            __VA_ARGS__);                                                \
    exit(1);                                                             \
  }
#define LOG_FATAL(msg) LOGF_FATAL("%s", msg)

#define LOGF_ERROR(fmt, ...)                                               \
  {                                                                        \
    extern verbosity global_loglevel;                                      \
    if (global_loglevel & LEVEL_ERROR) {                                   \
      fprintf(LOG_STREAM, "[ERROR] (%s:%d) " fmt "\n", __FILE__, __LINE__, \
              __VA_ARGS__);                                                \
      fflush(LOG_STREAM);                                                  \
    }                                                                      \
  }
#define LOG_ERROR(msg) LOGF_ERROR("%s", msg)

#define LOGF_WARN(fmt, ...)                                                \
  {                                                                        \
    extern verbosity global_loglevel;                                      \
    if (global_loglevel & LEVEL_WARN) {                                    \
      fprintf(LOG_STREAM, "[WARN]  (%s:%d) " fmt "\n", __FILE__, __LINE__, \
              __VA_ARGS__);                                                \
      fflush(LOG_STREAM);                                                  \
    }                                                                      \
  }
#define LOG_WARN(msg) LOGF_WARN("%s", msg)

#define LOG_INFO(fmt, ...)                                                 \
  {                                                                        \
    extern verbosity global_loglevel;                                      \
    if (global_loglevel & LEVEL_INFO) {                                    \
      fprintf(LOG_STREAM, "[INFO]  (%s:%d) " fmt "\n", __FILE__, __LINE__, \
              __VA_ARGS__);                                                \
      fflush(LOG_STREAM);                                                  \
    }                                                                      \
  }

#define LOGF_DEBUG(fmt, ...)                                                 \
  {                                                                          \
    extern verbosity global_loglevel;                                        \
    if (global_loglevel & LEVEL_DEBUG) {                                     \
      fprintf(LOG_STREAM, "[DEBUG] (%s:%-4d) " fmt "\n", __FILE__, __LINE__, \
              __VA_ARGS__);                                                  \
      fflush(LOG_STREAM);                                                    \
    }                                                                        \
  }

#define LOG_DEBUG(msg) LOGF_DEBUG("%s", msg)

#define ERR_WRAP(fx)                                                \
  {                                                                 \
    FRESULT fr;                                                     \
    fr = (fx);                                                      \
    if (fr) {                                                       \
      LOGF_FATAL("Error %d calling %s: %s", fr, #fx, f_errstr(fr)); \
    }                                                               \
  }

#endif
