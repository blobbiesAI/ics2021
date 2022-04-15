REPO_PATH = repo
ifeq ($(wildcard repo/LICENSE.TXT),)
  $(shell git clone --depth=1 https://gitee.com/blobbies/compiler-rt.git $(REPO_PATH))
endif

NAME = compiler-rt
SRCS = $(shell find repo/ -name "*.c")
include $(NAVY_HOME)/Makefile
