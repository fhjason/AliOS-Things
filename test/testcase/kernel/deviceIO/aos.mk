NAME := deviceIO_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := test code of deviceIO

$(NAME)_COMPONENTS += kernel.fs.vfs

$(NAME)_SOURCES += deviceIO_test.c

$(NAME)_CFLAGS += -Wall -Werror
