#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

const int max_num = 10000;
const int word_size = 64;

#define INDEX(A,B) ((A-B)/word_size)

int vertify_word_inc(char *pw, int argc, char **argv)
{
	int include_flg = 0;
	int i;
	for(i=1; i<argc; i++)
	{
		if(strstr(pw, argv[i]))
		{
			include_flg = 1;
			break;
		}
	}

	return include_flg;
}

int vertify_word_again(char *pw, char **list, int *lstcnt)
{
	int again_flg = 0;
	int i = 0;
	while(list[i][0])
	{
		if(strcmp(pw, list[i]) == 0)
		{
			again_flg = 1;
			lstcnt[i]++;   //重复的，计数+1
			break;
		}
		i++;
	}

	return again_flg;
}

int add_pd_word(char **list, int *lstcnt, char **phead, char **ptail, int argc, char **argv)
{
	char *head = *phead;
	char *tail = *ptail;
	
	//read all file content into array
    FILE *fp = fopen("temp_pd.html", "rb");
    fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
    char *content = (char *)malloc((fsize+1)*sizeof(char));
	memset(content, 0, sizeof(char)*(fsize+1));
	if(content == NULL)
		return -1;
	fread(content, sizeof(char), fsize, fp); 

    //point to last "count"
	char *subStr = "count";
	char *last_pos = NULL;
	char *pos_tmp = strstr(content, subStr);

	while(pos_tmp)
	{
		last_pos = pos_tmp;
		pos_tmp = strstr(pos_tmp+strlen("count"), subStr);

		if(pos_tmp != NULL)
		{
			last_pos = pos_tmp;
		}
		else
		{
			break;
		}
	}

	if(last_pos)
	{
		char str_num[5];
		memset(str_num, 0, 5);
		char *start_flg = ":";
		char *end_flg   = ",";
		char *p_start = strstr(last_pos, start_flg);
		char *p_end   = strstr(p_start, end_flg);
		strncpy(str_num, p_start+strlen(start_flg), p_end-p_start-1);
		int word_cnt = atoi(str_num) - 1;

		char **pword = (char **)malloc(word_cnt * sizeof(char*));
		if(pword == NULL)
			return -2;
		pword[0] = (char *)malloc(sizeof(char) * word_cnt * word_size);
		if(pword[0] == NULL)
			return -3;
		memset(pword[0], 0, sizeof(char)*word_cnt * word_size);

		int i;
		for(i=1; i<word_cnt; i++)
		{
			pword[i] = pword[i-1] + word_size;
		}

		p_start = content;
		for(i=0; i<word_cnt; i++)
		{
			start_flg = "\":\"";
			end_flg   = "\",\"";
			p_start = strstr(p_start, start_flg);
			p_end   = strstr(p_start, end_flg);
			strncpy(pword[i], p_start+strlen(start_flg), p_end-p_start-strlen(start_flg)); //
			p_start = p_end;
		}

		for(i=0; i<word_cnt; i++)
		{
			if(vertify_word_inc(pword[i], argc, argv) && !vertify_word_again(pword[i], list, lstcnt))
			{
				strcpy(tail, pword[i]);
				printf("%s ******** %d/%d ********\n", __FUNCTION__, INDEX(head, list[0])+1, INDEX(tail, list[0])+1);
				lstcnt[INDEX(tail,list[0])]++;  //添加到搜索队列后，对应计�?1
				tail+=word_size;
			}
		}

		free(pword[0]);
		free(pword);
	}

	//*phead +=word_size;
	*ptail = tail;
	
	if(fp != NULL)
		fclose(fp);
	return 0;
}

int  add_sug_word(char **list, int *lstcnt, char **phead, char **ptail, int argc, char **argv)
{
	char *head = *phead;
	char *tail = *ptail;
	
	//read all file content into array
	FILE *fp = fopen("temp_sug.html", "rb");
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *content = (char *)malloc((fsize+1) * sizeof(char));
	memset(content, 0, sizeof(char)*(fsize+1));
	if(content == NULL)
		return -1;

	fread(content, sizeof(char), fsize, fp); 

	fseek(fp, 0, SEEK_SET);
	int word_cnt = 0;
	char buf[256];
	while(fgets(buf, 256, fp))
	{
		word_cnt++;
	}

	if(word_cnt != 0)
	{
		char **pword = (char **)malloc(word_cnt * sizeof(char *));
		if(pword == NULL)
			return -2;
		pword[0] = (char *)malloc(sizeof(char) * word_cnt * word_size);
		if(pword[0] == NULL)
			return -3;
		memset(pword[0], 0, sizeof(char)*word_cnt * word_size);

		int i;
		for(i=1; i<word_cnt; i++)
		{
			pword[i] = pword[i-1] + word_size;
		}

		char *start_flg = "\">";
		char *end_flg   = "<";
		char *p_start = content;
		char *p_end   = NULL;
		for(i=0; i<word_cnt; i++)
		{
			p_start = strstr(p_start, start_flg);
			p_end   = strstr(p_start, end_flg);
			strncpy(pword[i], p_start+strlen(start_flg), p_end-p_start-strlen(start_flg)); //
			p_start = p_end;
		}

		for(i=0; i<word_cnt; i++)
		{
			if(vertify_word_inc(pword[i], argc, argv) && !vertify_word_again(pword[i], list, lstcnt))
			{
				strcpy(tail, pword[i]);
				printf("%s ******** %d/%d ********\n", __FUNCTION__, INDEX(head, list[0])+1, INDEX(tail, list[0])+1);
				lstcnt[INDEX(tail, list[0])]++;  //添加到搜索队列后，对应计�?1
				tail+=word_size;
			}
		}

		free(pword[0]);
		free(pword);
	}

	*phead +=word_size;
	*ptail = tail;
	
	if(fp != NULL)
		fclose(fp);
	return 0;
}


int main()
{
	FILE *fp_input = fopen("input.txt", "rb+");
	if(fp_input == NULL)
	{
		printf("can not open input.txt!\n");
		exit(-1);
	}

	int param_num = 0;
	char buf[word_size];
	while(fgets(buf, word_size, fp_input) && buf[0]!='\0')
	{
		param_num++;
	}
	fseek(fp_input, 0, SEEK_SET);

	if(param_num < 2)
	{
		printf("input param not enough,please check!\n");
		exit(-1);
	}
	
	char **param = (char**)malloc(param_num * sizeof(char*));
	if(param == NULL)
	{
		printf("malloc param failed!\n");
		exit(-1);
	}
	param[0] = (char*)malloc(sizeof(char)*param_num*word_size);
	if(param[0] == NULL)
	{
		printf("malloc param[0] failed!\n");
		exit(-1);
	}
	memset(param[0], 0, sizeof(char)*param_num*word_size);

	int i;
	for(i=1; i<param_num; i++)
		param[i] = param[i-1] + word_size;

	for(i=0; i<param_num; i++)
	{
		fgets(param[i], word_size, fp_input);
		param[i][strlen(param[i])] = '\0';
	}
	
	int pre_set_nun = atoi(param[0]);
	if(pre_set_nun<=0 || pre_set_nun >9900)
	{
		printf("pre_set_num is not correct, please check!\n");
		exit(-1);
	}
	
	//vs debug use
	//argc = 2;
	//argv[1] = "gift";
    int *lst_cnt = (int *)malloc(max_num*sizeof(int));
	if(lst_cnt == NULL)
	{
		printf("malloc lst_cnt failed!\n");
		exit(-1);
	}
	memset(lst_cnt, 0, max_num*sizeof(int));
	
	char **list = (char**)malloc(max_num*sizeof(char*));
	if(list == NULL)
	{
		printf("malloc list failed!\n");
		exit(-1);
	}
	list[0] = (char*)malloc(sizeof(char)*max_num*word_size);
	if(list[0] == NULL)
	{
		printf("malloc list[0] failed!\n");
		exit(-1);
	}
	memset(list[0], 0, sizeof(char)*max_num*word_size);

	for(i=1; i<max_num; i++)
		list[i] = list[i-1] + word_size;

    char *head = list[0];
	char *tail = list[1];
	
	//strcpy(list[0],argv[1]);
	strcpy(list[0],param[1]);
	lst_cnt[0]++;            //list中存入一个word，计�?1

	if(list[0] != NULL)
	{
		while(head != tail && INDEX(tail, list[0])<pre_set_nun)
		{
			//step1: get downpull words
			
			char url[512] = "'https://www.etsy.com/suggestions_ajax.php?extras=%7B%26quot%3Bexpt%26quot%3B%3A%26quot%3Boff%26quot%3B%2C%26quot%3Blang%26quot%3B%3A%26quot%3Ben-US%26quot%3B%2C%26quot%3Bextras%26quot%3B%3A%5B%5D%7D&version=10_12672349415_15&search_query=";
            char head_trans[128];
			strcpy(head_trans,head);
			for(i=0; i<strlen(head); i++)
			{
				if(head_trans[i] == ' ')
					head_trans[i] = '+';
			}
			strcat(url, head_trans);
			strcat(url, "&search_type=all'");
			printf("url0: %s\n", url);
			char cmdline[512];
			sprintf(cmdline, "/home/hxs/data/downpull.sh %s", url);
			system(cmdline);
			add_pd_word(list, lst_cnt, &head, &tail, param_num, param);
			printf("pulldown words proc down\n");

			//step2: get suggestion words
			
			sprintf(url, "https://www.etsy.com/search?q=");
			strcpy(head_trans,head);
			for(i=0; i<strlen(head); i++)
			{
				if(head_trans[i] == ' ')
					head_trans[i] = '+';
			}
			strcat(url, head_trans);
			printf("url1: %s\n", url);
			sprintf(cmdline, "/home/hxs/data/related.sh %s", url);
			system(cmdline);
			add_sug_word(list, lst_cnt, &head, &tail, param_num, param);
			printf("related proc down\n");
		}
	}
	else
	{
		printf("Error: root word empty\n");
		exit(-1);
	}

	char word_file_name[256];
	sprintf(word_file_name, "Words_%d_%s", param_num, param[1]);
	for(i=2; i<param_num; i++)
	{
		strcat(word_file_name, "+");
		strcat(word_file_name, param[i]);
	}
	strcat(word_file_name, ".txt");

	char count_file_name[256];
	sprintf(count_file_name, "Counts_%d_%s", param_num, param[1]);
	for(i=2; i<param_num; i++)
	{
		strcat(count_file_name, "+");
		strcat(count_file_name, param[i]);
	}
	strcat(count_file_name, ".txt");


	FILE *fp_word = fopen(word_file_name, "wb+");
	FILE *fp_cnt  = fopen(count_file_name, "wb+");
	
	fputs(list[0], fp_word);
	char count_tmp[16];
	sprintf(count_tmp, "%d", lst_cnt[0]);
	fputs(count_tmp, fp_cnt);
	
	i = 1;
	while(list[i][0])
	{
		fputs("\r\n", fp_word);
		fputs(list[i], fp_word);

		fputs("\r\n", fp_cnt);
		sprintf(count_tmp, "%d", lst_cnt[i]);
	    fputs(count_tmp, fp_cnt);
		
		i++;
	}

    free(list[0]);
	free(list);
	free(lst_cnt);
	free(param[0]);
	free(param);
	
	fclose(fp_word);
	fclose(fp_cnt);
	return 0;
}