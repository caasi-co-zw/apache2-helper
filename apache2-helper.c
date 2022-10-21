#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

// constants <3
const int MAX_MENU_ITEMS = 4;
const long double VERSION = 1.001;

// prototype :)
bool is_root(void);
int menu_list(void);
void edit_host(void);
void view_hosts(void);
void add_new_host(void);
void restart_apache(void);
void get_menu(int option);
bool dir_exists(DIR* dir);
bool host_exists(char *host);
char get_hosts_path(char *host);
int create_hosts_file(char *hosts_name,char *hosts_email);

// entry point
int main() {

    // check if correct platform
#ifndef __linux__
    printf("Sorry. System is built for linux only.");
    return 1;
#endif // __linux__


    //make sure is root
    if(!is_root()) {
        printf("Please run as root.\n");
        return 1;
    }

    int option = 1;

    // menu loop
    while (option < MAX_MENU_ITEMS && option > 0) {
        option = menu_list();
        get_menu(option);
    }

    printf("\nGood day!\n");
    return EXIT_SUCCESS;
}

// list of options
int menu_list(void) {
    system("clear");
    printf("\n1. New Virtual Host");
    printf("\n2. Edit Existing Virtual Host");
    printf("\n3. View Existing Hosts");
    printf("\n4. Exit\n\n");

    int option;
    scanf("%d",&option);

    return option;
}

char get_hosts_path(char *host) {
    return strcat("/etc/apache2/sites-available", strcat(host,".conf"));
}

void get_menu(int option) {
    switch(option) {
    case 1:
        add_new_host();
        break;
    case 2:
        edit_host();
        break;
    default:
        break;
    }
}

void add_new_host(void) {
    char domain_name;
    char domain_email;

    printf("\nDomain eg. google.com\t\t");
    scanf("%s",&domain_name);

    printf("Email eg. is admin@%s\t\t",&domain_name);
    scanf("%s",&domain_email);

    if (create_hosts_file(domain_name,domain_email)) {
        printf("\nVirtual host has been created.\n");
        restart_apache();
    }
}

void edit_host(void) {
    //
}

void view_hosts(void) {}

void restart_apache(void) {
    printf("\nRestarting apache\n");
    system("sudo systemctl reload apache2");
}

int create_hosts_file(char *hosts_name,char *hosts_email) {
    char directory = strcat("/var/www/",hosts_name);
    char path = get_hosts_path(strcat("/",hosts_name));
    char vhost = sprintf("<VirtualHost *:80>\n\tServerName %s\n\tServerAlias www.%s\n\tServerAdmin %s\n\tDocumentRoot /var/www/%s\n\tErrorLog ${APACHE_LOG_DIR}/error.log\n\tCustomLog ${APACHE_LOG_DIR}/access.log combined\n</VirtualHost>",hosts_name,hosts_name,hosts_name,hosts_name,hosts_email);

    // make sure directory exists
    if(!dir_exists(directory) && !mkdir(directory,0777)) {
        printf("Could not create folder");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(path, "w");

    // Print error message and exit if fopen() failed
    if (!file) {
        perror(path);
        return EXIT_FAILURE;
    }

    // Writes text to file. Unlike puts(), fputs() does not add a new-line.
    if (fputs(vhost, file) == EOF) {
        perror(path);
        return EXIT_FAILURE;
    }

    // Close file
    if (fclose(file)) {
        perror(path);
        return EXIT_FAILURE;
    }

    system(sprintf("sudo a2ensite %s.conf",&hosts_name));
    printf("Virtual host created.");
    return EXIT_SUCCESS;
}

bool is_root(void) {
    return (geteuid() == 0);
}

bool dir_exists(DIR* dir) {
    return dir ? true:false;
}

bool host_exists(char *host) {
    char *path = get_hosts_path(host);
    FILE *file = fopen(path, "r");

    return file;
}
