#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>

//插入删除任务
#define LIST_INSERT(item, list) do{				\
	printf("INSERT\n");							\
	item->prev = NULL;							\
	item->next = list;							\
	if ((list) != NULL) (list)->prev = item;	\
	(list) = item;								\
}while(0)

#define LIST_REMOVE(item, list) do{								\
	printf("REMOVE\n");											\
	if (item->prev != NULL) item->prev->next = item->next;		\
	if (item->next != NULL) item->next->prev = item->prev;		\
	if (list == item) list = item->next;						\
	item->prev = item->next = NULL;								\
}while(0)

//任务、工作人员、管理人员结构体
struct nTask {
	void (*task_func)(struct nTask* task);
	void* user_data;
	struct nTask* prev;
	struct nTask* next;
};

struct nWorker {
	pthread_t thread_id;
	int terminate;
	struct nManager* manager;
	struct nWorker* prev;
	struct nWorker* next;
};

struct nManager {
	struct nTask* tasks;
	struct nWorker* workers;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

//线程回调函数
static void* nThreadPoolCallback(void* arg)
{
	printf("CALLBACK\n");
	struct nWorker* worker = (struct nWorker*)arg;
	while (1)
	{
		pthread_mutex_lock(&worker->manager->mutex);
		while (worker->manager->tasks == NULL)
		{
			if (worker->terminate) break;
			pthread_cond_wait(&worker->manager->cond, &worker->manager->mutex);
		}
		if (worker->terminate)
		{
			pthread_mutex_unlock(&worker->manager->mutex);
			break;
		}
		struct nTask* task = worker->manager->tasks;
		LIST_REMOVE(task, worker->manager->tasks);
		pthread_mutex_unlock(&worker->manager->mutex);
		task->task_func(task);
	}
	free(worker);
}

//线程池创建函数
int nThreadPoolCreate(struct nManager* pool, int numWorkers)
{
	printf("CREATE\n");
	if (pool == NULL) return -1;
	if (numWorkers < 1) numWorkers = 1;
	memset(pool, 0, sizeof(struct nManager));
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));
	int i = 0;
	for (i = 0; i < numWorkers; i++)                            //创建numWorkes个工作人员线程放进线程池pool里
	{
		struct nWorker* worker = (struct nWorker*)malloc(sizeof(struct nWorker));
		if (worker == NULL)
		{
			perror("malloc");
			return -2;
		}
		memset(worker, 0, sizeof(struct nWorker));
		worker->manager = pool;
		int ret = pthread_create(&worker->thread_id, NULL, nThreadPoolCallback, worker);
		if (ret)
		{
			perror("phtread_create");
			free(worker);
			return -3;
		}
		LIST_INSERT(worker, pool->workers);
	}
	return 0;
}

//线程池摧毁函数
int nThreadPoolDestory(struct nManager* pool, int numWorkers)
{
	printf("DESTORY\n");
	struct nWorker* worker = NULL;
	for (worker = pool->workers; worker != NULL; worker = worker->next)
	{
		worker->terminate; //?
	}
	pthread_mutex_lock(&pool->mutex);
	pthread_cond_broadcast(&pool->cond);
	pthread_mutex_unlock(&pool->mutex);
	pool->workers = NULL;
	pool->tasks = NULL;
	return 0;
}

//任务创建函数
int nThreadPoolPushTask(struct nManager* pool, struct nTask* task)
{
	printf("PUSHTASK\n");
	pthread_mutex_lock(&pool->mutex);
	LIST_INSERT(task, pool->tasks);
	pthread_cond_signal(&pool->cond);
	pthread_mutex_unlock(&pool->mutex);
}

#if 1

#define THREADPOOL_INIT_COUNT	20
#define TASK_INIT_SIZE			30

 //任务入口函数
void task_entry(struct nTask* task)
{
	int idx = *(int*)task->user_data;
	printf("idx: %d\n", idx);
	free(task->user_data);
	free(task);
}

int main(void)
{
	printf("Start\n");
	struct nManager pool = { 0 };
	nThreadPoolCreate(&pool, THREADPOOL_INIT_COUNT);
	int i = 0;
	for (i = 0; i < TASK_INIT_SIZE; i++)
	{
		struct nTask* task = (struct nTask*)malloc(sizeof(struct nTask));
		if (task == NULL)
		{
			perror("malloc");
			exit(1);
		}
		memset(task, 0, sizeof(struct nTask));
		task->task_func = task_entry;
		task->user_data = malloc(sizeof(int));
		*(int*)task->user_data = i;
		nThreadPoolPushTask(&pool, task);
	}
	getchar();
	printf("End\n");
}

#endif
