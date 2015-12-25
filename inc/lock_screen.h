/* ==========================================================================
 * @file    : lock_screen.h
 *
 * @description : This file contains common definitions and declaration for
 *           the application.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#ifndef _LOCK_SCREEN_H
#define _LOCK_SCREEN_H

#define ever ;;
#define UTILS_DIR "PATH"
#define SCREEN_W 1366
#define SCREEN_H 768

typedef struct video_config {
	char dev[20];
	int w;
	int h;
	int f_size;
	int save;
	int running;
} VIDEO_CONFIG;

VIDEO_CONFIG *getConfig(void);

#endif
