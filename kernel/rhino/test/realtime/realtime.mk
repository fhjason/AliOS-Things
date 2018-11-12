NAME := realtime

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_COMPONENTS += rhino

GLOBAL_INCLUDES += include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := auxiliary.c \
                   sched_yield.c \
				   sched_preempt.c \
				   memory_blk.c \
				   memory_tlf.c \
				   sync_sem.c \
				   sync_mutex.c \
				   message_queue.c \
				   sync_mutex_shuffling.c \
				   sync_sem_shuffling.c \
				   interrupt_respond.c \
				   interrupt_wakeuptask.c \
				   sync_sem_rrwp_task.c \
				   message_queue_rrwp_task.c \
				   interrupt_preempt.c \
				   rttest_main.c \
				   sync_sem_active.c \
				   sync_mutex_active.c \
				   sched_resume_active.c \
				   global_object.c


