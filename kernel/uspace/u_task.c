/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#define PROC_MSG_NUM 100

static kstat_t task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *ustack_buf,
                           size_t ustack_size, cpu_stack_t *kstack_buf, size_t kstack_size,
                           task_entry_t entry, uint8_t autorun,uint8_t mm_alloc_flag,
                           uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid, uint8_t is_proc)
{
    CPSR_ALLOC();

    cpu_stack_t *tmp;
    ktask_t     *cur_task;
    ktask_t     *proc_task;
    uint8_t      i = 0;

    NULL_PARA_CHK(task);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(entry);
    NULL_PARA_CHK(ustack_buf);
    NULL_PARA_CHK(kstack_buf);

    if (kstack_size == 0u) {
        return RHINO_TASK_INV_STACK_SIZE;
    }

    if (prio >= RHINO_CONFIG_PRI_MAX) {
        return RHINO_BEYOND_MAX_PRI;
    }

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    /* idle task is only allowed to create once */
    if (prio == RHINO_IDLE_PRI) {
        if (g_idle_task_spawned[cpu_num] > 0u) {
            RHINO_CRITICAL_EXIT();
            return RHINO_IDLE_TASK_EXIST;
        }

        g_idle_task_spawned[cpu_num] = 1u;
    }

    RHINO_CRITICAL_EXIT();

    memset(task, 0, sizeof(ktask_t));

#if (RHINO_CONFIG_SCHED_RR > 0)
    if (ticks > 0u) {
        task->time_total = ticks;
    } else {
        task->time_total = RHINO_CONFIG_TIME_SLICE_DEFAULT;
    }

    task->time_slice   = task->time_total;
    task->sched_policy = KSCHED_RR;
#endif

    if (autorun > 0u) {
        task->task_state    = K_RDY;
    } else {
        task->task_state    = K_SUSPENDED;
        task->suspend_count = 1u;
    }

    /* init all the stack element to 0 */
    task->task_stack_base = kstack_buf;
    tmp = kstack_buf;

    memset(tmp, 0, kstack_size * sizeof(cpu_stack_t));

    task->task_name          = name;
    task->prio               = prio;
    task->b_prio             = prio;
    task->stack_size         = kstack_size;
    task->mm_alloc_flag      = mm_alloc_flag;
    task->cpu_num            = cpu_num;
    task->ustack_size        = ustack_size;
    task->task_ustack_base   = ustack_buf;
    task->mode               = 0x3;
    task->pid                = pid;
    task->is_proc            = is_proc;

    if (task->is_proc == 1) {
        task->proc_addr = task;
        klist_init(&task->task_head);
        klist_insert(&task->task_head, &task->task_user);
        /* process kobj init */
        klist_init(&(task.kobj_list.task_head));
        klist_init(&(task.kobj_list.mutex_head));
        klist_init(&(task.kobj_list.sem_head));
        klist_init(&(task.kobj_list.queue_head));
        klist_init(&(task.kobj_list.buf_queue_head));
        klist_init(&(task.kobj_list.event_head));
    }
    else {
        cur_task = krhino_cur_task_get();
        task->proc_addr = cur_task->proc_addr;
        proc_task = task->proc_addr;
        RHINO_CRITICAL_ENTER();
        klist_insert(&proc_task->task_head, &task->task_user);
        RHINO_CRITICAL_EXIT();
    }

    cpu_binded = cpu_binded;
    i          = i;

#if (RHINO_CONFIG_CPU_NUM > 1)
    task->cpu_binded    = cpu_binded;
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    tmp  = task->task_stack_base;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#else
    tmp  = (cpu_stack_t *)(task->task_stack_base) + task->stack_size - RHINO_CONFIG_STK_CHK_WORDS;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#endif
#endif

    cpu_utask_stack_init(task, kstack_buf, kstack_size,
                         ustack_buf, ustack_size,
                         arg, entry);

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_task_create_hook(task);
#endif

    TRACE_TASK_CREATE(task);

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_insert(&(g_kobj_list.task_head), &task->task_stats_item);
#endif

    if (autorun > 0u) {
        ready_list_add_tail(&g_ready_queue, task);
        /* if system is not start,not call core_sched */
        if (g_sys_stat == RHINO_RUNNING) {
            RHINO_CRITICAL_EXIT_SCHED();
            return RHINO_SUCCESS;
        }
    }

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

static kstat_t utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid,
                            uint8_t autorun, uint8_t is_proc)
{
    kstat_t      ret;
    cpu_stack_t *ktask_stack;
    ktask_t     *task_obj;

    NULL_PARA_CHK(task);

    if (kstack == 0) {
        return RHINO_INV_PARAM;
    }

    ktask_stack = krhino_mm_alloc(kstack * sizeof(cpu_stack_t));
    if (ktask_stack == NULL) {
        return RHINO_NO_MEM;
    }

    task_obj = krhino_mm_alloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        krhino_mm_free(ktask_stack);
        return RHINO_NO_MEM;
    }

   *task = task_obj;

    ret = task_create(task_obj, name, arg, pri, ticks, ustack_buf, ustack,
                      ktask_stack, kstack, entry, autorun, K_OBJ_DYN_ALLOC,
                      cpu_num, cpu_binded, pid, is_proc);

    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        krhino_mm_free(ktask_stack);
        krhino_mm_free(task_obj);

        return ret;
    }

    return ret;
}

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry, uint8_t autorun)
{
    ktask_t *cur_task;
    kstat_t  ret;

    cur_task = krhino_cur_task_get();

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, cur_task->pid, autorun, 0);

    return ret;
}

kstat_t krhino_utask_del(ktask_t *task)
{
    return krhino_task_dyn_del(task);
}

kstat_t krhino_uprocess_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               uint32_t pid, uint8_t autorun)
{
    ktask_t  *task_tmp;
    kqueue_t *queue;
    kstat_t   ret;

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, pid, 0, 1);

    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    task_tmp = *task;
    ret = krhino_queue_dyn_create(&queue, "res_queue", PROC_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        krhino_task_dyn_del(task_tmp);
        return ret;
    }

    task_tmp->res_q = queue;

    if (autorun > 0) {
        krhino_task_resume(task_tmp);
    }

    return ret;
}

kstat_t krhino_uprocess_exit(void)
{
    CPSR_ALLOC();

    kstat_t   ret;
    ktask_t  *cur_task;
    ktask_t  *task_del;
    ktask_t  *cur_proc;
    klist_t  *head;
    klist_t  *end;
    klist_t  *tmp;

    cur_task = krhino_cur_task_get();

    cur_proc = cur_task->proc_addr;

    head = &(cur_proc->task_head);
    end = head;

    /*head to user task*/
    RHINO_CRITICAL_ENTER();
    head = head->next->next;
    tmp = head->next;
    RHINO_CRITICAL_EXIT();

    /*step 1:delete user task*/
    while (tmp != end) {

        task_del = krhino_list_entry(tmp, ktask_t, task_user);

        RHINO_CRITICAL_ENTER();
        klist_rm(tmp);
        tmp = tmp->next;
        RHINO_CRITICAL_EXIT();

        ret = krhino_task_dyn_del(task_del);
    }

    /*step 2: delete utask*/
    ret = krhino_task_dyn_del(cur_task->proc_addr);
    return ret;
}

void krhino_uprocess_res_get(int32_t id, void **res)
{
    CPSR_ALLOC();

    ktask_t *cur_task;
    ktask_t *cur_proc;
    klist_t *head;

    cur_task = krhino_cur_task_get();
    cur_proc = cur_task->proc_addr;
    head = &cur_proc->task_head;
    if (id == 0) {
        *res = cur_proc->res_q;
    } else if (id == 1){
        RHINO_CRITICAL_ENTER();
        if (head->next->next == head) {
            *res = (void *)1;
        }
        else {
            *res = 0;
        }
        RHINO_CRITICAL_EXIT();
    }
}

