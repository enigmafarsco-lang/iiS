#include "datatypes.h"

DataTypes::DataTypes()
{

}

Transform *DataTypes::Transform_new(int type)
{
    Transform *tr = (Transform *)calloc(1, sizeof(Transform));

    tr->type_id = (type > NO_TRANSFORM_TYPE &&
                   type < TRANSFORMS_TYPES_COUNT) ? type
                                                  : NO_TRANSFORM_TYPE;

    return tr;
}

void DataTypes::Transform_destroy(Transform *tr)
{
    if (tr) {
        if (tr->x_axis && tr->destroy_x_axis) {
            free(tr->x_axis);
            tr->x_axis = NULL;
        }
        if (tr->y_axis && tr->destroy_y_axis) {
            free(tr->y_axis);
            tr->y_axis = NULL;
        }
        if (tr->settings) {
//            free(tr->settings);
            tr->settings = NULL;
        }
        if (tr->plot_channels) {
            g_slist_free(tr->plot_channels);
            tr->plot_channels = NULL;
        }
        free(tr);
    }
}

void DataTypes::Transform_resize_x_axis(Transform *tr, int new_size)
{
    tr->destroy_x_axis = true;

    if (new_size > 0) {
        tr->x_axis_size = new_size;
        tr->x_axis =new  QVector<double>(new_size); //realloc(tr->x_axis, sizeof(gfloat) * new_size);
    } else {
        if (tr->x_axis)
            free(tr->x_axis);
        tr->x_axis_size = 0;
        tr->x_axis = NULL;
    }
}

void DataTypes::Transform_resize_y_axis(Transform *tr, int new_size)
{
    if (tr->destroy_y_axis == false)
        tr->y_axis = NULL;
    tr->destroy_y_axis = true;

    if (new_size > 0) {
        tr->y_axis_size = new_size;
        tr->y_axis = new  QVector<double>(new_size);
//        memset(tr->y_axis, 0, sizeof(gfloat) * new_size);
    } else {
        if (tr->y_axis)
            free(tr->y_axis);
        tr->y_axis_size = 0;
        tr->y_axis = NULL;
    }
}

void DataTypes::Transform_resize_seek_x_axis(Transform *tr, int new_size)
{
    tr->destroy_seek_x_axis = true;

    if (new_size > 0) {
        tr->seek_x_axis_size = new_size;
        tr->seek_x_axis =new  QVector<double>(new_size); //realloc(tr->x_axis, sizeof(gfloat) * new_size);
    } else {
        if (tr->seek_x_axis)
            free(tr->seek_x_axis);

        tr->seek_x_axis_size = 0;
        tr->seek_x_axis = NULL;
    }
}

void DataTypes::Transform_resize_seek_y_axis(Transform *tr, int new_size)
{
    if (tr->destroy_seek_y_axis == false)
        tr->seek_y_axis = NULL;
    tr->destroy_seek_y_axis = true;

    if (new_size > 0) {
        tr->seek_y_axis_size = new_size;
        tr->seek_y_axis = new  QVector<double>(new_size);
//        memset(tr->y_axis, 0, sizeof(gfloat) * new_size);
    } else {
        if (tr->seek_y_axis)
            free(tr->seek_y_axis);
        tr->seek_y_axis_size = 0;
        tr->seek_y_axis = NULL;
    }
}

QVector<double> *DataTypes::Transform_get_x_axis_ref(Transform *tr)
{
    return tr->x_axis;
}

QVector<double> *DataTypes::Transform_get_y_axis_ref(Transform *tr)
{
    return tr->y_axis;
}

void DataTypes::Transform_attach_settings(Transform *tr, void *settings)
{
    tr->settings = settings;
}

void DataTypes::Transform_attach_function(Transform *tr, bool (*f)(Transform *, gboolean))
{
    tr->transform_function = f;
}

void DataTypes::Transform_setup(Transform *tr)
{
    tr->transform_function(tr, TRUE);
}

bool DataTypes::Transform_update_output(Transform *tr)
{
    return tr->transform_function(tr, FALSE);
}

QList<Transform*> *DataTypes::TrList_new()
{
    QList<Transform*> *list = new QList<Transform*>();

    return list;
}

void DataTypes::TrList_destroy(QList<Transform*> *list)
{
//    foreach(Transform *tr:list)
//    free(tr);
    free(list);
}

void DataTypes::TrList_add_transform(QList<Transform*> *list, Transform *tr)
{        
    list->append(tr);
}

void DataTypes::TrList_remove_transform(QList<Transform*> *list, Transform *tr)
{
    list->removeAll(tr);
}
