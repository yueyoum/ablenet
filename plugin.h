#ifndef PLUGIN_H_INCLUDE
#define PLUGIN_H_INCLUDE

#include <Python.h>
#include <structmember.h>
#include <string>

namespace Plugin
{


class Instance
{
public:
    Instance(PyObject* obj);
    void on_data(const std::string& data);

private:
    PyObject * obj_;
};


class Plugin
{
public:
    Plugin();
    ~Plugin();

    Instance* new_instance(uint64_t client_id);

private:
    PyObject* cls_;
};

}

#endif //PLUGIN_H_INCLUDE
