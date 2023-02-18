#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define NAME_LENGTH		16
#define PHONE_LENGTH	32
#define MAX_LENGTH		48
#define INFO			printf

#define LIST_INSERT(item, list) do{							\
	item->next = list;										\
	item->prev = NULL;										\
	if((list) != NULL) (list)->prev = item;					\
	list = item;											\
}while(0)

#define LIST_DELETE(item, list) do{							\
	if(item->prev != NULL) item->prev->next = item->next;	\
	if (item->next != NULL) item->next->prev = item->prev;	\
	if (list == item) list = item->next;					\
	item->prev = item->next = NULL;							\
}while(0)

struct person {
	char name[NAME_LENGTH];
	char phone[PHONE_LENGTH];

	struct person* next;
	struct person* prev;
};

struct contacts {
	struct person* people;
	int count;
};

enum oper_select
{
	OPER_INSERT = 1,
	OPER_DELETE,
	OPER_SEARCH,
	OPER_PRINT,
	OPER_SAVE,
	OPER_LOAD
};
int person_insert(struct person** people, struct person* ps)
{
	if (ps == NULL) return -1;
	LIST_INSERT(ps, *people);
	return 0;
}

int person_delete(struct person** people, struct person* ps)
{
	if (ps == NULL) return -1;
	LIST_DELETE(ps, *people);
	return 0;
}

struct person* person_search(struct person* people, const char* name)
{
	struct person* temp = NULL;
	for (temp = people; temp != NULL; temp = temp->next)
	{
		if (!strcmp(temp->name, name))
		{
			break;
		}
	}
	return temp;
}

int person_print(struct person* people)
{
	struct person* temp = NULL;
	INFO("%s\n", people->name);
	for (temp = people; temp != NULL; temp = temp->next)
	{
		INFO("name:%s, phone:%s\n", temp->name, temp->phone);
	}
	return 0;
}

int save_file(struct person* people, const char* filename)
{
	if (people == NULL) return -1;
	FILE* fp = fopen(filename, "w");
	if (fp == NULL) return -1;
	struct person* temp = NULL;
	for (temp = people; temp != NULL; temp = temp->next)
	{
		fprintf(fp, "name:%s, phone:%s\n", temp->name, temp->phone);
		fflush(fp);
	}
	fclose(fp);
	return 0;
}

int load_file(struct person** people, int* count, const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return -1;
	char name_phone[MAX_LENGTH] = { 0 };
	char name[MAX_LENGTH] = { 0 };
	char phone[MAX_LENGTH] = { 0 };
	int index = 0;
	while (fscanf(fp, "%s", name_phone) != EOF)
	{
		index++;
		if (index % 2 != 0)
		{
			int status = 0, j = 0;
			for (int i = 0; name_phone[i] != ','; i++)
			{
				if (name_phone[i] == ':')
				{
					status = 1;
				}
				else if (status)
				{
					name[j++] = name_phone[i];
				}
			}
			name[j] = '\0';
		}
		else
		{
			int status = 0, j = 0;
			for (int i = 0; name_phone[i] != '\0'; i++)
			{
				if (name_phone[i] == ':')
				{
					status = 1;
				}
				else if (status)
				{
					phone[j++] = name_phone[i];
				}
			}
			phone[j] = '\0';
			struct person* ps = (struct person*)malloc(sizeof(struct person));
			if (ps == NULL) return -1;
			strcpy(ps->name, name);
			strcpy(ps->phone, phone);
			person_insert(people, ps); 
			(*count)++;
			memset(name, 0, sizeof(name));
			memset(phone, 0, sizeof(phone));
		}
		memset(name_phone, 0, sizeof(name_phone));
	}
	fclose(fp);
	return 0;
}

int insert_entry(struct contacts* cts)
{
	if (cts == NULL) return -1;
	struct person* p = (struct person*)malloc(sizeof(struct person));
	if (p == NULL) return -2;
	INFO("Please Input Name:");
	scanf("%s", &p->name);
	INFO("Please Input Phone:");
	scanf("%s", &p->phone);
	if (person_insert(&cts->people, p) != 0)
	{
		free(p);
		return -3;
	}
	cts->count++;
	INFO("Insert Success.\n");
	return 0;
}

int print_entry(struct contacts* cts)
{
	if (cts == NULL) return -1;
	person_print(cts->people);
	return 0;
}

int delete_entry(struct contacts* cts)
{
	if (cts == NULL) return -1;
	INFO("Please Input Name:");
	char name[NAME_LENGTH] = { 0 };
	scanf("%s", &name);
	struct person* ps = person_search(cts->people, name);
	if (ps == NULL)
	{
		INFO("Person don't Exit.\n");
		return -2;
	}
	person_delete(&cts->people, ps);
	free(ps);
	return 0;
}

int search_entry(struct contacts* cts)
{
	if (cts == NULL) return -1;
	INFO("Please Input Name:");
	char name[NAME_LENGTH] = { 0 };
	scanf("%s", &name);
	struct person* ps = person_search(cts->people, name);
	if (ps == NULL)
	{
		INFO("Person don't Exit.\n");
		return -2;
	}
	INFO("name:%s, phone:%s\n", ps->name, ps->phone);
	return 0;
}

int save_entry(struct contacts* cts)
{
	if (cts == NULL) return -1;
	INFO("Please Input Filename:");
	char filename[MAX_LENGTH] = { 0 };
	scanf("%s", &filename);
	save_file(cts->people, filename);
	return 0;
}

int load_entry(struct contacts* cts)
{
	INFO("Please Input Filename:");
	char filename[MAX_LENGTH] = { 0 };
	scanf("%s", &filename);
	load_file(&cts->people, &cts->count, filename);
	return 0;
}
void menu_info(void) {
	INFO("\n\n*********************************************************************\n");
	INFO("*************1.Add Person\t\t2.Del Person*****************\n");
	INFO("*************3.Search Person\t\t4.Print Person***************\n");
	INFO("*************5.Save Person\t\t6.Load Person****************\n");
	INFO("*********************************************************************\n");
}
int main()
{
	struct contacts* cts = (struct contacts*)malloc(sizeof(struct contacts));
	memset(cts, 0, sizeof(struct contacts));
	while (1)
	{
		menu_info();
		int select = 0;
		scanf("%d", &select);
		switch (select)
		{
		case OPER_INSERT:
			insert_entry(cts);
			break;
		case OPER_DELETE:
			delete_entry(cts);
			break;
		case OPER_SEARCH:
			search_entry(cts);
			break;
		case OPER_PRINT:
			print_entry(cts);
			break;
		case OPER_SAVE:
			save_entry(cts);
			break;
		case OPER_LOAD:
			load_entry(cts);
			break;
		default:
			goto exit;
		}
	}
exit:
	return 0;
}