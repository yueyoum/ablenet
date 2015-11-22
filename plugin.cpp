#include <iostream>
#include <memory>

#include "ablenet.h"
#include "plugin.h"

typedef struct
{
    PyObject_HEAD
    uint64_t id;
} Process;

static void Process_dealloc(Process* self)
{
    self->ob_type->tp_free((PyObject *)self);
}


static PyObject* Process_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Process* self;
    self = (Process *)type->tp_alloc(type, 0);
    self->id = 0;
    return (PyObject *)self;
}

static int Process_init(Process* self, PyObject* args, PyObject* kwargs)
{
    if(!PyArg_ParseTuple(args, "i", &self->id))
    {
        return -1;
    }

    return 0;
}

static PyMemberDef Process_members[] = {
    {(char *)"process_id", T_INT, offsetof(Process, id), 0, (char *)"process id"},
    {NULL, 0, 0, 0, NULL}
};

static PyObject* Process_send(Process* self, PyObject* args)
{
    char *data;

    if(!PyArg_ParseTuple(args, "s", &data))
    {
        return NULL;
    }

    uint64_t id = self->id;
    ablenet::Server::instance()->send(id, std::string(data));
    Py_RETURN_NONE;
}

static PyObject* Process_broadcast(Process* self, PyObject* args)
{
    char *data;

    if(!PyArg_ParseTuple(args, "s", &data))
    {
        return NULL;
    }

    ablenet::Server::instance()->broadcast(std::string(data));
    Py_RETURN_NONE;
}

static PyMethodDef Process_methods[] = {
    {"send", (PyCFunction)Process_send, METH_VARARGS, ""},
    {"broadcast", (PyCFunction)Process_broadcast, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}

};

static PyTypeObject Process_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "ablenet.Process",
    sizeof(Process),
    0,
    (destructor)Process_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    "",
    0,
    0,
    0,
    0,
    0,
    0,
    Process_methods,
    Process_members,
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)Process_init,
    0,
    Process_new,
};


static PyMethodDef module_methods[] = {
        {NULL, NULL, 0, NULL}
};


Plugin::Instance::Instance(PyObject* obj): obj_(obj)
{
    PyObject *func = PyObject_GetAttrString(obj_, "connection_made");
    if(!func)
    {
        printf("can not find func: connection_made\n");
        exit(1);
    }

    PyObject_CallFunction(func, NULL);
}


void Plugin::Instance::on_data(const std::string& data) {
    PyObject *func = PyObject_GetAttrString(obj_, "on_data");
    if(!func) {
        printf("can not find func: on_data\n");
        exit(1);
    }

    PyObject* arg = Py_BuildValue("s", data.data());
    PyObject_CallFunctionObjArgs(func, arg, NULL);
}

Plugin::Plugin::Plugin()
{
    Py_Initialize();

    char *module_name = (char *)"app";
    char *class_name = (char *)"Client";


    PyObject *module = Py_InitModule("ablenet", module_methods);
    if(!module) {
        std::cout << "error init module" << std::endl;
        exit(1);
    }

    if(PyType_Ready(&Process_Type) < 0)
    {
        std::cout << "error pytype ready" << std::endl;
        exit(1);
    }

    PyModule_AddObject(module, (char *)"Process", (PyObject *)&Process_Type);

    PyRun_SimpleString("import sys; sys.path.insert(0, '.')");

    PyObject * pModule = PyImport_Import(PyString_FromString(module_name));
    if(pModule == NULL) {
        std::cout << "can not import " << module_name << std::endl;
        exit(1);
    }

    cls_ = PyObject_GetAttrString(pModule, class_name);
    if(!cls_) {
        std::cout << "can not get class: " << class_name << std::endl;
        exit(1);
    }

//    Py_DECREF(pModule);
    printf("Python INIT done\n");
}


Plugin::Plugin::~Plugin()
{
    Py_XDECREF(cls_);
    Py_Finalize();
}


Plugin::Instance* Plugin::Plugin::new_instance(uint64_t client_id)
{
    PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyLong_FromLong(client_id));

    PyObject* obj = PyObject_CallObject(cls_, args);
    Instance* ins = new Instance(obj);
    printf("plugin new_instance\n");
    return ins;
}
