#include <iostream>

#include "gtk_wrapper.h"

typedef __gnu_cxx::hash_map<const char *, XrmValue *> db_type;
typedef std::pair<const char *, XrmValue *> db_entry;

XrmDatabase
get_file_database(const char *f)
{
    return xmlParseFile(f);
}

bool
get_resource(XrmDatabase database, const char *str_name, const char *str_class,
	     XrmValue &value_return)
{
    std::cerr << "get_resource not implemented\n";
    return false;
}

bool
put_resource(XrmDatabase database, const char *str_name, const char *str_class,
	     XrmValue &value)
{
    std::cerr << "put_resource not implemented\n";
    return false;
}

void
merge_databases(XrmDatabase source_db, XrmDatabase target_db)
{
    std::cerr << "CANNOT MERGE DATABASES\n";
}

XrmDatabase
get_string_database(const char *s)
{
    std::cerr << "STRING TO DATABASE NOT IMPLEMENTED\n";
    return NULL;
}

