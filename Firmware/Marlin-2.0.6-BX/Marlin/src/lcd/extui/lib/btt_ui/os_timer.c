#include "os_timer.h"
#include "includes.h"


/* 1ms */
uint32_t OS_GetTimeMs(void)
{
  return MarlinAPI_GetMS();
}

/*
 * task: task structure to be filled
 * time_ms:
 */
void OS_TaskInit(OS_TASK *task, uint32_t time_ms, FP_TASK function, void *para)
{
  task->time_ms = time_ms;
  task->task = function;
  task->para = para;
}
/*
*/
void OS_TaskLoop(OS_TASK *task_t)
{
  if(task_t->is_exist == 0)   return;
  if(OS_GetTimeMs() < task_t->next_time)  return;
  if(task_t->is_repeat == 0)
  {
    task_t->is_exist = 0;
  }
  else
  {
    task_t->next_time = OS_GetTimeMs() + task_t->time_ms;
  }
  (*task_t->task)(task_t->para);
}

/*
*/
void OS_TaskEnable(OS_TASK *task_t, uint8_t is_exec,uint8_t is_repeat)
{
  task_t->is_exist =1;
  task_t->is_repeat = is_repeat;
  task_t->next_time = OS_GetTimeMs() + task_t->time_ms;
  if(is_exec)
    (*task_t->task)(task_t->para);
}

/*
*/
void OS_TaskDisable(OS_TASK *task_t)
{
  task_t->is_exist = 0;
}

void Delay_ms(uint16_t ms)
{
  uint32_t timeout = OS_GetTimeMs() + ms;

  while(timeout > OS_GetTimeMs());
}
