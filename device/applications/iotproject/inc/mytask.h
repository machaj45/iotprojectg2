#ifndef _MYTASK
#define _MYTASK

#ifdef __cplusplus
extern "C" {
#endif
void Initialize_OS(void);
void StartDefaultTask(void const *argument);
void check_modules(void const *argument);
void murata_process_rx_response(void const *argument);
void vApplicationIdleHook(); 
#ifdef __cplusplus
}
#endif
#endif 
