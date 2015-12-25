/* ==========================================================================
 * @file    : capture_thread.c
 *
 * @description : This file contains the video capture thread.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <lock_screen.h>
#include <yuyv2jpegsave.h>

struct capturebuffer {
	unsigned char *start;
	size_t offset;
	unsigned int length;
};

/****************************************************************************
 * @function : This is the capture thread main function. It captures video frames
 *          using V4l2 and passes those buffers to other threads.
 *
 * @arg  : void
 * @return     : void
 * *************************************************************************/
void *captureThread(void)
{
	int fd, i;
	struct v4l2_capability cap;
	enum v4l2_buf_type type;
	struct v4l2_streamparm parm;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct capturebuffer buffers[10];
	char *buff = NULL;

	VIDEO_CONFIG *config = getConfig();

	if((fd = open(config->dev, O_RDWR, 0)) < 0) {
		perror("video device open");
		exit(0);
	}

	if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
		perror("VIDIOC_QUERYCAP");
		return NULL;
	}
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("NO CAPTURE SUPPORT\n");
		return NULL;
	}
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("NO STREAMING SUPPORT\n");
		return NULL;
	}

	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = 25;
	parm.parm.capture.capturemode = 0;
	if (ioctl(fd, VIDIOC_S_PARM, &parm) < 0) {
		perror("VIDIOC_S_PARM\n");
		return NULL;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = config->w;
	fmt.fmt.pix.height = config->h;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
		perror("VIDIOC_S_FMT");
		return NULL;
	}

	memset(&req, 0, sizeof(req));
	req.count = 10;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
		perror("VIDIOC_REQBUFS");
		return NULL;
	}

	for(i = 0; i < 10; i++) {
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
			perror("VIDIOC_QUERYBUF");
			return NULL;
		}

		buffers[i].length = buf.length;
		buffers[i].offset = (size_t) buf.m.offset;
		buffers[i].start = mmap(NULL, buffers[i].length,
					PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, buffers[i].offset);
		memset(buffers[i].start, 0xFF, buffers[i].length);
	}

	for (i = 0; i < 10; i++) {
		memset(&buf, 0, sizeof (buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.m.offset = buffers[i].offset;
		if (ioctl (fd, VIDIOC_QBUF, &buf) == -1) {
			perror("VIDIOC_QBUF");
			return NULL;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl (fd, VIDIOC_STREAMON, &type) == -1) {
		perror("VIDIOC_STREAMON");
		return NULL;
	}
	buff = calloc(1, config->f_size);
	if(buff == NULL) {
		perror("calloc");
		return NULL;
	}
	for(ever) {
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if(ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
			perror("VIDIOC_DQBUF");
			return NULL;
		}
		if(config->save) {
			memcpy(buff, buffers[buf.index].start, config->f_size);
			yuyv2jpegsave(buff, config->w, config->h);
			config->save = 0;
			sleep(1);
		}
		if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			perror("VIDIOC_QBUF");
			return NULL;
		}
		usleep(10);
		if(!config->running)
			break;
	}
	free(buff);
	if (ioctl (fd, VIDIOC_STREAMOFF, &type) == -1) {
		perror("VIDIOC_STREAMOFF");
		return NULL;
	}
	close(fd);

	return 0;
}
