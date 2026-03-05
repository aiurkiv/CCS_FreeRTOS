#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

// *** Quanto maior o valor, menor a prioridade
#define PRIORITY_KERNEL_INTERRUPT           ( 6 << 5 )  /* Priority 7, or 0xE0 as only the top three bits are implemented.  This is the lowest priority. */
#define PRIORITY_MAX_SYSCALL_INTERRUPT      ( 5 << 5 )  /* Priority 5, or 0xA0 as only the top three bits are implemented. */
#define PRIORITY_INTERRUPT_BUTTONS          ( 7 << 5 ) 
#define PRIORITY_INTERRUPT_TIMER_BOUNCING   ( 7 << 5 ) 

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

#endif // __DEFINITIONS_H__
