#include "../headers/working_files.h"

int32_t create_working_directory_and_chdir(const char *dir)
{
  if (!dir) {
    return -1;
  }
  if (mkdir(dir, 0775) < 0) {
    if (errno != EEXIST) {
      syslog(LOG_INFO, "%s working directory", strerror(errno));
      return -1;
    }
  }
  chdir(dir);
  return 0;
}

int32_t create_accounts_file(const char *acc)
{
  int32_t fd;
  if (!acc) {
    return -1;
  }
  if ((fd = open(acc, O_WRONLY | O_CREAT, 0664)) < 0) {
    syslog(LOG_INFO, "%s accounts file", strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}

int32_t check_image(const char *img)
{
  int32_t fd;
  if (!img) {
    return -1;
  }
  if ((fd = open(img, O_RDONLY)) < 0) {
    syslog(LOG_INFO, "%s screen file", strerror(errno));
    return -1;
  }
  close(fd);
  return 0;
}
