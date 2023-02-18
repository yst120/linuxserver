#include<stdio.h>
#include<stdlib.h>
struct word {
	char str[200];
	int fqt;
}word[500];
/*
#define INIT	0
#define OUT		0
#define IN		1
int splite(char c)
{
	if ((c == ' ') || (c == '\n') || (c == '\t') || (c == '\"')
		(c == '\'') || (c == '+') || (c == ',') || (c == ';') || (c == '.'))
	{
		return 1;
	}
	else return 0;
}
int count_word(char* filename)
{
	int status = INIT;
	int word = 0;
	FIEL* fp = fopen(filename, 'r');
	if (fp == NULL) return -1;
	char ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		if (splite(ch) || ch == '-')         //加入'-'判断
		{
			status = OUT;
			if (ch == '-') word--;           //单词计数-1
		}
		else if (status == OUT)
		{
			status = IN;
			word++;
		}
	}
	return word;
}
*/
int count_word(char* filename)
{
	FIFL* fp = fopen(filename, 'r');
	if (fp == NULL) return -1;
	char s[200];
	int count = 0;
	while (fscanf(fp, "%s", s) != EOF)
	{
		int new = 1;
		for (int i = 0; i < count; i++)
		{
			if (strcmp(word[i].str, s) == 0)
			{
				word[i].fqt++;
				new = 0;
				break;
			}
		}
		if (new)
		{
			strcpy(word[count].str, s);
			word[count].fqt = 1;
		}
	}
	for (int i = 0; i < count; i++)
	{
		printf("%s的次数是%d\n", word[i].str, word[i].fqt);
	}
	return 0;
}
int main(int argc, char* argv[])
{
	if (argc < 2) return -1;
	count_word(argv[1]);
	/*printf("word=%d\n", count_word(argv[1]));*/
	return 0;
}