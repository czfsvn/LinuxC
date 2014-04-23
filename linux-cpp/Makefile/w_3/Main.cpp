#include <iostream>
#include <mysql/mysql.h>


using namespace std;

/*
static char *server_args[] =
{
    "this_program",
    "--datadir=.",
    "--key_buffer_size=32M"
};

static char *server_groups[] =
{
    "embedded",
    "server",
    "this_program_SERVER",
    (char*)NULL
};
*/
void Test_Mysql();

int main()
{
    Test_Mysql();
    return 1;
}


void Test_Mysql()
{
 //   if (mysql_server_init((server_args) / sizeof(char*),
 //           server_args, server_groups))
  //      exit(1);

    /*
    char* server    = "192.168.2.102";
    char* user      = "63";
    char* pwd       = "123456";
    char* database  = "sguser0523";
    */
    std::string server    = "192.168.2.102";
    std::string user      = "63";
    std::string pwd       = "123456";
    std::string database  = "sguser0523";
    MYSQL*  conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server.c_str(), user.c_str(),
                pwd.c_str(), database.c_str(), 0, NULL, 0))
    {
        std::cout<<mysql_error(conn)<<std::endl;
        return;
    }

    if (mysql_query(conn, "show tables"))
    {
        std::cout<<mysql_error(conn)<<std::endl;
        return;
    }

    MYSQL_RES* res = mysql_use_result(conn);
    std::cout<<"Mysql tables in mysql database:"<<std::endl;
    MYSQL_ROW   row;
    while (row = mysql_fetch_row(res))
        std::cout<<"table:"<<row[0]<<std::endl;

    std::cout<<"Number of rows:"<<
        (unsigned long)mysql_num_rows(res)<<std::endl;

    MYSQL_FIELD* field;
    while (field = mysql_fetch_field(res))
        std::cout<<"field:"<<field->name<<std::endl;

    int num_fields = mysql_num_fields(res);
    for (int i = 0; i < num_fields; ++i)
    {
        field = mysql_fetch_field_direct(res, i);
        std::cout<<"Field "<<i<<" is "<<field->name<<std::endl;
    }

    if (!mysql_set_character_set(conn, "utf8"))
    {
        MY_CHARSET_INFO cs;
        mysql_get_character_set_info(conn, &cs);
        std::cout<<"character set infomation:"<<std::endl;
        std::cout<<"character set name:"<<cs.name<<std::endl;
        std::cout<<"collation name:"<<cs.csname<<std::endl;
        std::cout<<"comment:"<<cs.comment<<std::endl;
        std::cout<<"derectory:"<<cs.dir<<std::endl;
        std::cout<<"multi byte character min.length:"<<cs.mbminlen<<std::endl;
        std::cout<<"multi byte character max.length:"<<cs.mbmaxlen<<std::endl;
    }

    mysql_free_result(res);
    mysql_close(conn);
    mysql_server_end();
    return ;



}
