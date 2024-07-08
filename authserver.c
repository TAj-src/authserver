/*

Written by G7TAJ 2024

https://github.com/taj-src

Portions (c) G8BPQ - from the LinBPQ/BPQ32 suite

You should have received a copy of the GNU General Public License
along with this code.  If not, see http://www.gnu.org/licenses

*/

/*
 This is a OTP auth program for calling a url from a LinBPQ node.
 See https://github.com/taj-src/authserver for more details

 Use the LinBPQ CmdLineAuth program to generate auth codes appending the OTP to the end of your password

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <unistd.h>
#include <signal.h>

#define UCHAR unsigned char
#define BOOL int
#define TRUE 1
#define FALSE 0


char PROGDIR[1024];


typedef int  int32_t;
typedef unsigned   uint32_t;

#include "md5_authserver.c"

void handle_sigpipe(int sig) {
    fprintf(stderr, "Broken pipe error detected\n");
    exit(EXIT_FAILURE);
}

void remove_CRLF(char *str) {
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != '\n' && str[i] != '\r') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}
void CreateOneTimePassword(char * Password, char * KeyPhrase, int TimeOffset)
{
        // Create a time dependent One Time Password from the KeyPhrase
        // TimeOffset is used when checking to allow for slight variation in clocks

        time_t NOW = time(NULL);
        UCHAR Hash[16];
        char Key[1000];
        int i, chr;

        NOW = NOW/30 + TimeOffset;                              // Only Change every 30 secs

        sprintf(Key, "%s%x", KeyPhrase, (int)NOW);
        md5(Key, Hash);

        for (i=0; i<16; i++)
        {
                chr = (Hash[i] & 31);
                if (chr > 9) chr += 7;

                Password[i] = chr + 48;
        }

        Password[16] = 0;
        return;
}



BOOL CheckOneTimePassword(char * Password, char * KeyPhrase)
{
        char CheckPassword[17];
        int Offsets[10] = {0, -1, 1};
//, -2, 2, -3, 3, -4, 4};
        int i, Pass;

        if (strlen(Password) < 16)
                Pass = atoi(Password);

        for (i = 0; i < 3; i++)
        {
               CreateOneTimePassword(CheckPassword, KeyPhrase, Offsets[i]);

                if (strlen(Password) < 16)
                {
                        // Using a numeric extract

                        long long Val;

                        memcpy(&Val, CheckPassword, 8);
                        Val = Val %= 1000000;

                        if (Pass == Val)
                                return TRUE;

                }
                else
                        if (memcmp(Password, CheckPassword, 16) == 0)
                                return TRUE;
        }

        return FALSE;
}


#define MAX_MENU_ITEMS 100
#define MAX_LENGTH 50
#define ACTION_LENGTH 500

typedef struct MenuItem {
    char text[MAX_LENGTH];
    char action[ACTION_LENGTH];
} MenuItem;



void do_menu_action( char * action ) {

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, action);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "failed :(\n");
        }
        curl_easy_cleanup(curl);
    }
}


void getdir() {
  char  buf[PATH_MAX];

    pid_t pid = getpid();
    sprintf(buf, "/proc/%d/exe", pid);
    int len = readlink(buf, buf, PATH_MAX);
    buf[len] = '\0';

    char *last_slash = strrchr(buf, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        strcpy(buf, buf);
    }
    strcpy( PROGDIR , buf);

}


BOOL checkcall(char * callsign, char *passwd) {

    char CREDS[1024], tmp_call[10], tmp_passwd[256];
    char line[MAX_LENGTH];
    int index = 0;

    sprintf(CREDS,"%s/authserver.creds", PROGDIR);

    FILE *file = fopen(CREDS, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening Cred file\n");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%[^,],%s", tmp_call, tmp_passwd);

	if (strcmp(callsign, tmp_call) == 0) {
		strcpy(passwd, tmp_passwd);
	        fclose(file);
		return TRUE;
	}


        index++;
    }

    fclose(file);
    return FALSE;

}

int do_menu() {

    int i;
    MenuItem menu[MAX_MENU_ITEMS];
    char line[MAX_LENGTH];
    int index = 0;
    int choice;
    char MENU_FILE[1024];
    char buf[PATH_MAX];

    sprintf(MENU_FILE,"%s/authserver.menu", PROGDIR);

    FILE *file = fopen(MENU_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }


    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%[^,],%s", menu[index].text, menu[index].action);
        index++;
    }

    fclose(file);

    do {
        for (i = 0; i < index; i++) {
            printf("%d. %s\n", i + 1, menu[i].text);
        }
        printf("Enter your choice (0 to exit): ");
	fflush(stdout);
        scanf("%d", &choice);


    int c;
    while ((c = getchar()) != '\n' && c != EOF);

        if (choice >= 1 && choice <= index) {
            do_menu_action( menu[choice - 1].action );
        }
    } while (choice != 0);

    return 0;
}



void main ()
{
  char buffer[256];
  char FILEPASSWD[256];
  int tries;
  int random_number;

  signal(SIGPIPE, handle_sigpipe);

  getdir();

  fgets(buffer, sizeof(buffer), stdin);
  remove_CRLF(buffer);

  fflush(stdout);
  FILEPASSWD[0]='\0';

  if (!checkcall(buffer, FILEPASSWD)) {
    return;
  }

  while (1) {
        if ( tries >1 )
	   return;

        random_number = rand() % 900000 + 100000;
	sprintf(FILEPASSWD,"%s%d", FILEPASSWD, random_number);

	printf("Your code=%d\nEnter AuthCode: ", random_number);
        fflush(stdout);

        fgets(buffer, sizeof(buffer), stdin);
	if ( CheckOneTimePassword(buffer, FILEPASSWD) )
	{
		printf("Code verified\n");
                fflush(stdout);

		do_menu();
		fflush(stdin);
		exit(0);
		return;
	} else {
		printf("Invalid code\n");
	        fflush(stdout);
		FILEPASSWD[ strlen(FILEPASSWD)-6 ]='\0';	//wipe out last code ready for next try
		tries++;
	}
  }

}
