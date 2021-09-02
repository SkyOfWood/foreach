#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
using namespace std;

int g_circle = 1;
int g_threadNum = 1;
int g_maxThread = 40;
char g_separator = ' ';
bool g_threadLimit = true;
string g_cmd;
string g_danger_cmd;
vector<string> text;
vector<string> v_cmd;
vector<string> circle_cmd;
pthread_mutex_t work_mutex;
vector<string>::iterator ix_iter;

void help(void)
{
	cout << "Author: mhts(mhts2002@qq.com)" << endl
		 << "ARM64 Maker: flynn1994@qq.com" << endl
		 << "" << endl
		 << "Usage:foreach -[hv] [-w threads] [-c circle time] [-s seperator] cmd ..." << endl
		 << "      -v   Display verbose messages" << endl
		 << "      -h   Help" << endl
		 << "      -c   circle:circle time(default value is 1)" << endl
		 << "      -s   sep:default is space or \\t" << endl
		 << "      -w   threads:thread number(default value is 1)" << endl
		 << "      -m   set thread max number no limit(default is limit to 40)" << endl
		 << "      cmd  the command to be executed!" << endl;
}

void version(void)
{
	cout << "foreach: Version 1.0" << endl
		 << "Date: 2021-09-02" << endl;
}

void parse(int argc, char **argv)
{
	if (argc < 2)
	{
		help();
		exit(0);
	}

	opterr = 0;
	char cOpt = 0;
	while ((cOpt = getopt(argc, argv, "hmvs:c:w:")) != char(-1))
	{
		switch (cOpt)
		{
		case 'c':
			g_circle = atoi(optarg);
			break;
		case 'w':
			g_threadNum = atoi(optarg);
			break;
		case 'm':
			g_threadLimit = false;
			break;
		case 's':
			g_separator = *optarg;
			break;
		case 'h':
			help();
			exit(0);
		case 'v':
			version();
			exit(0);
		default:
			help();
			exit(-1);
			break;
		}
	}

	if (optind < argc)
	{
		g_cmd = argv[optind];
	}

	if (g_threadLimit && g_threadNum > g_maxThread)
	{
		g_threadNum = g_maxThread;
	}

#ifdef DEBUG
	// cout << "g_circle = " << g_circle << endl
	//	  << "g_threadNum = " << g_threadNum << endl
	//	  << "g_separator = " << g_separator << endl
	//	  << "g_cmd = " << g_cmd << endl;
#endif
}

void getText(void)
{
	if (isatty(0))
	{
		text.push_back(g_cmd);
		return;
	}

	string line;
	while (getline(cin, line))
	{
		text.push_back(line);
	}

#ifdef DEBUG
	// cout << text[0] << endl;
	// cout << text[1] << endl;
	// cout << text[2] << endl;
#endif
}

void replaceChar(char *str)
{
	while (*str != '\0')
	{
		if (*str == '\t')
		{
			*str = ' ';
		}
		str++;
	}
}

void parseCmd(void)
{
	vector<string>::iterator ix = text.begin();
	for (; ix != text.end(); ix++)
	{
		vector<string> line;
		line.push_back(*ix);

		//separate the line by 'g_separator' from stdin save into 'vector<string> line'
		char *tmp_line = new char[(*ix).size() + 1];
		char *pch;
		strcpy(tmp_line, (*ix).c_str());

		// replace \t with ' '
		replaceChar(tmp_line);

		pch = strtok(tmp_line, &g_separator);
		while (pch != NULL)
		{
			line.push_back(pch);
			pch = strtok(NULL, &g_separator);
		}

		//assembling the 'cmd' and 'line'
		string cmd;
		string cmd_tmp = g_cmd;
		size_t pos = 0;
		while ((pos = cmd_tmp.find('#')) != string::npos)
		{
			cmd += cmd_tmp.substr(0, pos);
			string s_row;
			while (isdigit(cmd_tmp[pos + 1]))
			{
				s_row += cmd_tmp[++pos];
			}
			if (!s_row.empty())
			{
				size_t row = atoi(s_row.c_str());
				if (row < line.size())
				{
					cmd += line[row];
				}
			}
			pos++;
			cmd_tmp = cmd_tmp.substr(pos);
		}
		cmd += cmd_tmp;
		v_cmd.push_back(cmd);

		delete[] tmp_line;
	}
}

void circleCmd(void)
{
	vector<string>::iterator b_cmd = v_cmd.begin();
	for (; b_cmd != v_cmd.end(); b_cmd++)
	{
		for (int ix = 0; ix < g_circle; ix++)
		{
			circle_cmd.push_back(*b_cmd);
		}
	}

#ifdef DEBUG
	cout << circle_cmd[0] << endl;
	cout << circle_cmd[1] << endl;
	cout << circle_cmd[2] << endl;

#endif
}

void *do_cmd(void *)
{
	while (true)
	{
		pthread_mutex_lock(&work_mutex);
		if (ix_iter == circle_cmd.end())
		{
			pthread_mutex_unlock(&work_mutex);
			break;
		}
		string cmd;
		cmd = *ix_iter;
		++ix_iter;
		pthread_mutex_unlock(&work_mutex);
		system(cmd.c_str());
	}
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	parse(argc, argv);
	getText();
	parseCmd();
	circleCmd();

	if (g_threadNum > (int)circle_cmd.size())
	{
		g_threadNum = circle_cmd.size();
	}

	ix_iter = circle_cmd.begin();
	int res;
	pthread_t pthread_id;
	pthread_t *p_id = new pthread_t[g_threadNum];

	pthread_mutex_t work_mutex;
	res = pthread_mutex_init(&work_mutex, NULL);
	if (res != 0)
	{
		cout << "init pthread_mutex fail!" << endl;
		exit(-1);
	}

	for (int pthread_num = 0; pthread_num < g_threadNum; pthread_num++)
	{
		res = pthread_create(&pthread_id, NULL, &do_cmd, NULL);
		if (res != 0)
		{
			cout << "create new pthread fail!" << endl;
			exit(-1);
		}
		p_id[pthread_num] = pthread_id;
	}

	for (int pthread_num = 0; pthread_num < g_threadNum; pthread_num++)
	{
		pthread_join(p_id[pthread_num], NULL);
	}
	pthread_mutex_destroy(&work_mutex);
	delete[] p_id;
}
