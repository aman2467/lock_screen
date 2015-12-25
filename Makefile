# =========================================================================================
#  @file    : Makefile
#
#  @description : Makefile for project.
#
#  @author  : Aman Kumar (2015)
#
#  @copyright   : The code contained herein is licensed under the GNU General Public License.
#               You may obtain a copy of the GNU General Public License Version 2 or later
#               at the following locations:
#               http://www.opensource.org/licenses/gpl-license.html
#               http://www.gnu.org/copyleft/gpl.html
# =========================================================================================*/

VERBOSE = @
BASEDIR = $(PWD)
PROJECT="lock_screen"
CC =gcc
NONE=\033[0m
BOLD=\033[01;37m
GREEN=\033[01;32m
RED=\033[01;31m
BIN_DIR=$(BASEDIR)/bin
INC_DIR=$(BASEDIR)/inc
SRC_DIR=$(BASEDIR)/src
LIB_DIR=${BASEDIR}/lib
UTILS_DIR=$(BASEDIR)/utils
CFLAGS += -g -O3 -Wall
LDFLAGS= -L${LIB_DIR}
LIBS= -lSDL -lpthread -lyuyv2jpegsave -ljpeg
CPPFLAGS += -I. \
		-I./inc/ \

.PHONY: clean all ${PROJECT}

all: ${PROJECT}

${PROJECT}:
	${VERBOSE}sed -i "s,PATH,$(UTILS_DIR),g" $(INC_DIR)/${PROJECT}.h
	${VERBOSE}${CC} $(SRC_DIR)/*.c ${CFLAGS} ${LDFLAGS} ${CPPFLAGS} -o ${BIN_DIR}/${PROJECT} ${LIBS}
	${VERBOSE}sed -i "s,$(UTILS_DIR),"PATH",g" $(INC_DIR)/${PROJECT}.h
	${VERBOSE}echo "${BOLD}All ${GREEN}done${BOLD}..!!${NONE}"

clean:
	${VERBOSE}rm -f $(BIN_DIR)/*
	${VERBOSE}echo "${BOLD}All Binaries ${RED}removed${NONE}"
