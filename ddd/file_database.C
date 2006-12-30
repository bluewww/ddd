#include <iostream>

#include "gtk_wrapper.h"

typedef __gnu_cxx::hash_map<const char *, XrmValue *> db_type;
typedef std::pair<const char *, XrmValue *> db_entry;

XrmDatabase
get_file_database(const char *f)
{
#if 0
    XrmDatabase db = new db_type;
    std::cerr << "WARNING: need to populate database\n";
    return db;
#else
    std::cerr << "WARNING: need to populate database\n";
    return xmlParseFile(f);
#endif
}

bool
get_resource(XrmDatabase database, const char *str_name, const char *str_class,
	     XrmValue &value_return)
{
#if 0
    db_type::iterator iter = database->find(str_name);
    if (iter != database->end()) {
	value_return = *iter->second;
	return true;
    }
#endif
    return false;
}

bool
put_resource(XrmDatabase database, const char *str_name, const char *str_class,
	     XrmValue &value)
{
#if 0
    XrmValue *copy = new XrmValue(value);
    db_entry p(str_name, copy);
    std::pair<db_type::iterator, bool> ret = database->insert(p);
    return ret.second;
#else
    return false;
#endif
}

void
merge_databases(XrmDatabase source_db, XrmDatabase target_db)
{
#if 0
    target_db->insert(source_db->begin(), source_db->end());
#else
    std::cerr << "CANNOT MERGE DATABASES\n";
#endif
}

XrmDatabase
get_string_database(const char *s)
{
#if 0
    db_type *db = new db_type;
    while (strlen(s)) {
	const char *p = strchr(s, '\n');
	if (!p) p = s+strlen(s);
	std::string str(s, p);
	const char *line = str.c_str();
	const char *q = strstr(line, " \t");
	if (q) {
	    std::string name(line, q);
	    int len = strspn(q, " \n");
	    std::string value(q+len);
	    Glib::Value<Glib::ustring> *v = new Glib::Value<Glib::ustring>;
	    v->set(value);
	    db_entry ent(name.c_str(), v);
	    db->insert(ent);
	}
	s = p;
    }
    return db;
#else
    std::cerr << "STRING TO DATABASE NOT IMPLEMENTED\n";
    return NULL;
#endif
}

