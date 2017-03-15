
/*
 * ------------------------------------------------------------
 * "THE BEERWARE LICENSE" (Revision 42):
 * Bharadwaaj wrote this code. As long as you retain this
 * notice, you can do whatever you want with this stuff. If we
 * meet someday, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * ------------------------------------------------------------
*/

#include "SKEAZ1284.h"
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 10 )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_SEND_PARAMETER			( 0x1111UL )
#define mainQUEUE_RECEIVE_PARAMETER			( 0x22UL )

/* The number of the LED that is toggled. */
#define mainLED_TO_TOGGLE					( 0 )
/*-----------------------------------------------------------*/


/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );


static QueueHandle_t xQueue = NULL;
int main(void)
{

	/* Create the queue. */
		xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

		if( xQueue != NULL )
		{
			/* Start the two tasks as described in the comments at the top of this
			file. */
			xTaskCreate( prvQueueReceiveTask,					/* The function that implements the task. */
						"Rx", 									/* The text name assigned to the task - for debug only as it is not used by the kernel. */
						configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
						( void * ) mainQUEUE_RECEIVE_PARAMETER, /* The parameter passed to the task - just to check the functionality. */
						mainQUEUE_RECEIVE_TASK_PRIORITY, 		/* The priority assigned to the task. */
						NULL );									/* The task handle is not required, so NULL is passed. */

			xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, ( void * ) mainQUEUE_SEND_PARAMETER, mainQUEUE_SEND_TASK_PRIORITY, NULL );

			/* Start the tasks and timer running. */
			vTaskStartScheduler();
		}

		/* If all is well, the scheduler will now be running, and the following
		line will never be reached.  If the following line does execute, then
		there was insufficient FreeRTOS heap memory available for the idle and/or
		timer tasks	to be created.  See the memory management section on the
		FreeRTOS web site for more details. */
		for( ;; );
}



static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 1000UL;

	/* Check the task parameter is as expected. */
	configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_SEND_PARAMETER );

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, 0U );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;

	/* Check the task parameter is as expected. */
	configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_RECEIVE_PARAMETER );

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		if( ulReceivedValue == 1000UL )
		{
		//	vParTestToggleLED( mainLED_TO_TOGGLE );
			ulReceivedValue = 0U;
		}
	}
}
/*-----------------------------------------------------------*/





















/************* Hooks*****************************/


void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY 1
void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()).  The code in this
	tick hook implementation is for demonstration only - it has no real
	purpose.  It just gives a semaphore every 50ms.  The semaphore unblocks a
	task that then toggles an LED.  Additionally, the call to
	vQueueSetAccessQueueSetFromISR() is part of the "standard demo tasks"
	functionality. */

	/* The semaphore and associated task are not created when the simple blinky
	demo is used. */
	#if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0
	{
	static unsigned long ulLastGiveTime = 0UL;
	const unsigned long ulRate = 50UL / portTICK_PERIOD_MS;
	extern SemaphoreHandle_t xLEDSemaphore;

		configASSERT( xLEDSemaphore );

		if( ( xTaskGetTickCountFromISR() - ulLastGiveTime ) > ulRate )
		{
			/* The second parameter is normally used to determine if a context
			switch should be performed or not.  In this case the function is
			being performed from the tick hook, so the scheduler will make that
			assessment before returning to a task anyway - so the parameter is
			not needed and is just set to NULL. */
			xSemaphoreGiveFromISR( xLEDSemaphore, NULL );
			ulLastGiveTime += ulRate;
		}

		/* Write to a queue that is in use as part of the queue set demo to
		demonstrate using queue sets from an ISR. */
		vQueueSetAccessQueueSetFromISR();
	}
	#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
