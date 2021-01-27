#include "vfs.h"
#include <stdlib.h>
#include <string.h>
#include "myfatfs.h"
//#include "includes.h"

MYFILE infoFile={
  .title = "bSD:",
  .source = BOARD_SD
};

bool mountFS(void)
{
//  resetInfoFile();   //needn't this
  switch (infoFile.source)
  {
    case TFT_SD:
      return mountSDCard();

    case TFT_UDISK:
      return mountUDisk();

    case BOARD_SD:
      return mountBoardSDCard();
  }
  return false;
}

/*
*/
void clearInfoFile(void)
{
  uint8_t i=0;
  for (i=0; i<infoFile.F_num; i++)
  {
    free(infoFile.folder[i]);
    infoFile.folder[i] = 0;
  }
  for (i=0; i<infoFile.f_num; i++)
  {
    free(infoFile.file[i]);
    infoFile.file[i] = 0;
  }
  infoFile.F_num = 0;
  infoFile.f_num = 0;
}

TCHAR* getCurFileSource(void)
{
  switch (infoFile.source)
  {
    case TFT_SD:     return "SD:";
    case TFT_UDISK:  return "U:";
    case BOARD_SD:   return "bSD:";
  }
  return NULL;
}

/*
infoFile
*/
void resetInfoFile(void)
{
  FS_SOURCE source = infoFile.source;
  clearInfoFile();
  memset(&infoFile, 0, sizeof(infoFile));
  infoFile.source = source;

  strncpy(infoFile.title, getCurFileSource(), MAX_PATH_LEN);
}

/*
*/
bool scanPrintFiles(void)
{
  clearInfoFile();
  switch (infoFile.source)
  {
    case TFT_SD:
    case TFT_UDISK:
    case BOARD_SD:
      return scanPrintFilesFatFs();
  }
  return false;
}

/*
*/
bool EnterDir(const char *nextdir)
{
  if(strlen(infoFile.title)+strlen(nextdir)+2>=MAX_PATH_LEN) return 0;
  strcat(infoFile.title,"/");
  strcat(infoFile.title,nextdir);
  return 1;
}

/*
*/
void ExitDir(void)
{
  int i=strlen(infoFile.title);
  for(;i>0&&infoFile.title[i]!='/';i--)
  {
  }
  infoFile.title[i]=0;
}

/*
*/
bool IsRootDir(void)
{
  return !strchr(infoFile.title,'/');
}
