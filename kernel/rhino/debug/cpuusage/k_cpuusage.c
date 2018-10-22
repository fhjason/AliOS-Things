/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_dbg_api.h"

#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)

typedef struct {
    uint8_t  task_name[32];
    uint32_t task_cpu_usage;
} task_cpuusage_info;

static uint32_t task_cpu_usage_period = 0;

void krhino_task_cpu_usage_stats()
{
    klist_t *taskhead = &g_kobj_list.task_head;
    klist_t *taskend  = taskhead;
    klist_t *tmp;
    ktask_t *task;

    static lr_timer_t stats_start = 0;
    lr_timer_t        stats_end;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);
        task->task_exec_time = task->task_time_total_run -
                               task->task_time_total_run_prev;
        task->task_time_total_run_prev = task->task_time_total_run;
    }
    RHINO_CPU_INTRPT_ENABLE();

    stats_end             = (lr_timer_t)LR_COUNT_GET();
    task_cpu_usage_period = stats_end - stats_start;
    stats_start           = stats_end;
}

void krhino_total_cpu_usage_show()
{
    uint32_t    i;
    klist_t    *taskhead = &g_kobj_list.task_head;
    klist_t    *taskend  = taskhead;
    klist_t    *tmp;
    ktask_t    *task;
    const char *task_name;
    lr_timer_t  task_cpu_usage;
    uint32_t    total_cpu_usage[RHINO_CONFIG_CPU_NUM];

    uint32_t            tasknum = 0;
    task_cpuusage_info *taskinfo;
    task_cpuusage_info *taskinfoeach;

    printf("-----------------------\n");
#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        total_cpu_usage[i] = krhino_total_cpu_usage_get(i);
        printf("CPU usage :%3d.%02d%%  \n", (int)total_cpu_usage[i]/100,
               (int)total_cpu_usage[i]%100);
    }
#else
    total_cpu_usage[0] = krhino_total_cpu_usage_get(0);
    printf("CPU usage :%3d.%02d%%  \n", (int)total_cpu_usage[0] / 100,
           (int)total_cpu_usage[0] % 100);
#endif

    printf("-----------------------\n");
    printf("Name               %%CPU\n");
    printf("-----------------------\n");

    krhino_sched_disable();
    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        tasknum ++;
    }
    taskinfo = krhino_mm_alloc(tasknum * sizeof(task_cpuusage_info));
    if (taskinfo ==  NULL) {
        krhino_sched_enable();
        return RHINO_NO_MEM;
    }
    memset(taskinfo, 0, tasknum * sizeof(task_cpuusage_info));

    taskinfoeach = taskinfo;

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task = krhino_list_entry(tmp, ktask_t, task_stats_item);

        if (task->task_name != NULL) {
            task_name = task->task_name;
        } else {
            task_name = "anonym";
        }
        taskinfoeach->task_cpu_usage = krhino_task_cpu_usage_get(task);
        strncpy(taskinfoeach->task_name, task_name, sizeof(taskinfoeach->task_name) - 1);
        taskinfoeach++;
    }
    krhino_sched_enable();

    for (i = 0; i < tasknum; i++) {
        taskinfoeach   = taskinfo + i;
        task_name      = taskinfoeach->task_name;
        task_cpu_usage = taskinfoeach->task_cpu_usage;

        printf("%-19s%3d.%02d\n", task_name, (int)task_cpu_usage / 100,
               (int)task_cpu_usage % 100);
    }

    krhino_mm_free(taskinfo);

    printf("-----------------------\n");
}

/* one in ten thousand */
uint32_t krhino_task_cpu_usage_get(ktask_t *task)
{
    uint32_t task_cpu_usage = 0;

    if (task_cpu_usage_period == 0) {
        return 0;
    }

    task_cpu_usage = (uint64_t)(task->task_exec_time) * 10000 / task_cpu_usage_period;

    return task_cpu_usage;
}

/* one in ten thousand */
uint32_t krhino_total_cpu_usage_get(uint32_t cpuid)
{
    uint32_t total_cpu_usage = 0;
    total_cpu_usage          = 10000 - krhino_task_cpu_usage_get(&g_idle_task[cpuid]);
    return total_cpu_usage;
}

#if (RHINO_CONFIG_CPU_USAGE_PERIOD > 0)
static ktimer_t cpu_usage_timer;

static void cpu_usage_timer_handler(void *timer, void *args)
{
    krhino_task_cpu_usage_stats();
}

kstat_t krhino_task_cpu_usage_init()
{
    kstat_t ret = RHINO_SUCCESS;
    sys_time_t cpu_usage_period = krhino_ms_to_ticks(RHINO_CONFIG_CPU_USAGE_PERIOD);

    if (cpu_usage_period == 0) {
        return RHINO_INV_PARAM;
    }

    ret = krhino_timer_create(&cpu_usage_timer, "cpu_usage_timer",
                              cpu_usage_timer_handler, 1, cpu_usage_period, NULL, 1);
    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    return ret;
}

#endif

#endif
