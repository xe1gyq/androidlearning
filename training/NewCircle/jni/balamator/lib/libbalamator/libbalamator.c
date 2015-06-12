#define LOG_NDEBUG 1                                               /* <1> */
#define LOG_TAG "balamator"                                          /* <1> */
#define LOG_ID LOG_ID_MAIN                                         /* <2> */

#include <balamator.h>                                               /* <3> */
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>

static const char THE_DEVICE[] = "/sys/kernel/balamator/status";

static int balamator_exists() {
    int fd;

    fd = TEMP_FAILURE_RETRY(open(THE_DEVICE, O_RDWR));
    if(fd < 0) {
        ALOGE("balamator/status file does not exist : %d", fd);
        return 0;
    }

    close(fd);
    return 1;
}

static int sendit(unsigned int status)
{
    int to_write, written, ret, fd;

    char value[20];

    fd = TEMP_FAILURE_RETRY(open(THE_DEVICE, O_RDWR));
    if(fd < 0) {
        return -errno;
    }

    to_write = snprintf(value, sizeof(value), "%u\n", status);
    written = TEMP_FAILURE_RETRY(write(fd, value, to_write));

    if (written == -1) {
        ret = -errno;
    } else if (written != to_write) {
        /* even though EAGAIN is an errno value that could be set
           by write() in some cases, none of them apply here.  So, this return
           value can be clearly identified when debugging and suggests the
           caller that it may try to call vibraror_on() again */
        ret = -EAGAIN;
    } else {
        ret = 0;
    }

    errno = 0;
    close(fd);

    return ret;
}

static int balamator_on(struct balamator_device_t* balamdev, unsigned int status)
{
	return sendit(status);
}


static int balamator_off(struct balamator_device_t* balamdev, unsigned int status)
{
	return sendit(0);
}

static int open_balamator(const struct hw_module_t *module, char const *name,
   struct hw_device_t **device) {

    if (!balamator_exists()) {
        ALOGE("Balamator device does not exist. Cannot start Balamator");
        return -ENODEV;
    }

    struct balamator_device_t *dev = malloc(sizeof(struct balamator_device_t));
    if (!dev) {
        ALOGE("Can not allocate memory for balamator device");
        return -ENOMEM;
    }

    return 0;
  }


static struct hw_module_methods_t balamator_module_methods = {
  .open = open_balamator,                                            /* <6> */
};

struct hw_module_t HAL_MODULE_INFO_SYM = {                         /* <7> */
  .tag = HARDWARE_MODULE_TAG,
  .version_major = 1,
  .version_minor = 0,
  .id = BALAMATOR_HARDWARE_MODULE_ID,                                /* <3> */
  .name = "balamator module",
  .author = "Netavam Inc.",
  .methods = &balamator_module_methods,                              /* <6> */
};
