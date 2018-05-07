#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

int Sirius_cd(char **args);
int Sirius_help(char **args);
int Sirius_exit(char **args);
int Sirius_ls(char **args);
int Sirius_execute(char ** args);

char *builtin_str[] = {
  "cd",
  "ls",
  "help",
  "exit"
};



int (*builtin_func[]) (char **) = {
  &Sirius_cd,
  &Sirius_ls,
  &Sirius_help,
  &Sirius_exit
};

int Sirius_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}


int Sirius_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Sirius: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("Sirius:cd");
    }
  }
  return 1;
}

int Sirius_ls( char **args)
{
 	struct dirent **namelist;
	int n;
	if(args[0]==NULL)
	{
		exit(EXIT_FAILURE);
	}
	else if (args[1]==NULL)
	{
		n=scandir(".",&namelist,NULL,alphasort);
	}
	else
	{
        pid_t pid, wpid;
        int status;
        pid = fork();
        if (pid == 0)
        {
            if (execvp(args[0], args) == -1) 
            {
                perror("Sirius");
            }
            exit(EXIT_FAILURE);
        } 
        else if (pid < 0) 
        {
            perror("Sirius");
        } 
        else 
        {
            do 
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
	}
	if(n < 0)
	{
		perror("Sirius:ls");
		exit(EXIT_FAILURE);
	}
	else
	{
		while (n--)
		{
			printf("%s\n",namelist[n]->d_name);
			free(namelist[n]);
		}
		free(namelist);
	}
	return 1;
}

int Sirius_help(char **args)
{
  args[0]="man";
  Sirius_execute(args);
  return 1;
}


int Sirius_exit(char **args)
{
  return 0;
}


int Sirius_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0)
   {
    if (execvp(args[0], args) == -1) {
      perror("Sirius");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {

    perror("Sirius");
  } else {

    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int Sirius_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < Sirius_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0 ) {
      return (*builtin_func[i])(args);
    }
  }

  return Sirius_launch(args);
}

#define SIRIUS_RL_BUFSIZE 1024

char *Sirius_read_line(void)
{
  int bufsize = SIRIUS_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "Sirius: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
   
    c = getchar();


    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;


    if (position >= bufsize) {
      bufsize += SIRIUS_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "Sirius: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define SIRIUS_TOK_BUFSIZE 64
#define SIRIUS_TOK_DELIM " \t\r\n\a"

char **Sirius_split_line(char *line)
{
  int bufsize = SIRIUS_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "Sirius: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SIRIUS_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SIRIUS_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "Sirius: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SIRIUS_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void Sirius_loop(void)
{
  char *line;
  char **args;
  int status;
  char cwd[1024];
  
  do {
    getcwd(cwd, sizeof(cwd));
    printf("Sirius (%s) $ ",cwd);
    line = Sirius_read_line();
    args = Sirius_split_line(line);
    status = Sirius_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  Sirius_loop();
  return EXIT_SUCCESS;
}

