/*
//Name: Tyler Ykema
//ID#: 1062564
//Date: Febuary 15, 2019
//Email: tykema@uoguelph.ca
*/

#include "SVGhelper.h"

/****************A1 Stuff******************/
SVGimage* createSVGimage(char* fileName)
{
    if (fileName == NULL)
        return NULL;

    xmlDoc *doc = NULL;
    xmlNode *root_node = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    //Check file name is good and checks to see if file exists and is readable
    //if not then returns null
    if (doc == NULL)
    {
        return NULL;
    }

    char *empty_string = "empty string";

    SVGimage *img = NULL;

    img = malloc(sizeof(SVGimage));
    strncpy(img->namespace, empty_string, strlen(empty_string) + 1);
    strncpy(img->title, empty_string, strlen(empty_string) + 1);
    strncpy(img->description, empty_string, strlen(empty_string) + 1);

    img->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    img->circles = initializeList(circleToString, deleteCircle, compareCircles);
    img->paths = initializeList(pathToString, deletePath, comparePaths);
    img->groups = initializeList(groupToString, deleteGroup, compareGroups);
    img->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);



    root_node = xmlDocGetRootElement(doc);
    //determine if the xml file is valid, returns null if not
    if (root_node == NULL || root_node->type != XML_ELEMENT_NODE)
    {
        return NULL;
    }

    //checks to see if the first xml element is an svg and copies namespace into struct
    //, returns null if not
    if (root_node->type == XML_ELEMENT_NODE && !strcmp((char*)root_node->name, "svg"))
    {
        if (root_node->nsDef->href != NULL)
            strncpy(img->namespace, (char*)root_node->nsDef->href, 255);
        else
            strncpy(img->namespace, empty_string, strlen(empty_string));
    }
    else
    {
        return NULL;
    }

    getData(img, root_node);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    return img;
}

void getData(SVGimage *img, xmlNode *prev_node)
{
    Rectangle *rect = NULL;
    Circle *circ = NULL;
    Path *path = NULL;
    Group *group = NULL;

    char *empty_string = "empty string";

    xmlNode *cur_node = NULL;

    //This function is based off of the example from starter.zip, specifically
    //the printxml function where the names and attributes are printed
    for (cur_node = prev_node; cur_node != NULL; cur_node = cur_node->next)
    {
        //Finds title node and copies content into SVGimage struct
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (!strcmp((char*)cur_node->name, "title"))
            {
                if (cur_node->children->content != NULL)
                    strncpy(img->title, (char*)cur_node->children->content, 255);
                else
                    strncpy(img->title, empty_string, 255);
            }

            //Finds description node if it exists and copies content into struct
            else if (!strcmp((char*)cur_node->name, "desc"))
            {
                if (cur_node->children->content != NULL)
                    strncpy(img->description, (char*)cur_node->children->content, 255);
                else
                    strncpy(img->description, empty_string, 255);
            }

            //If node is a rect then stores attributes in rect struct and adds to list
            else if (!strcmp((char*)cur_node->name, "rect"))
            {
                rect = malloc(sizeof(Rectangle));
                rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

                xmlAttr *attr;
                for (attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    fillRect(rect, attr);
                }

                insertBack(img->rectangles, rect);
            }

            else if (!strcmp((char*)cur_node->name, "circle"))
            {
                circ = malloc(sizeof(Circle));
                circ->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

                xmlAttr *attr;
                for (attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    fillCircle(circ, attr);
                }

                insertBack(img->circles, circ);
            }

            else if (!strcmp((char*)cur_node->name, "path"))
            {
                path = malloc(sizeof(Path));
                path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

                xmlAttr *attr;
                for (attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    fillPath(path, attr);
                }

                insertBack(img->paths, path);
            }

            else if (!strcmp((char *)cur_node->name, "g"))
            {
                group = malloc(sizeof(Group));
                group->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
                group->circles = initializeList(circleToString, deleteCircle, compareCircles);
                group->paths = initializeList(pathToString, deletePath, comparePaths);
                group->groups = initializeList(groupToString, deleteGroup, compareGroups);
                group->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

                fillGroup(group, cur_node->children);

                xmlAttr *attr;
                for (attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    Attribute *attrStruct = malloc(sizeof(Attribute));
                    fillAttr(attrStruct, attr);
                    insertBack(group->otherAttributes, attrStruct);
                }

                insertBack(img->groups, group);
            }

            else
            {
                xmlAttr *attr;
                for (attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    Attribute *attrStruct = malloc(sizeof(Attribute));
                    fillAttr(attrStruct, attr);
                    insertBack(img->otherAttributes, attrStruct);
                }
            }
        }

        if (strcmp((char *)cur_node->name, "g") != 0)
            getData(img, cur_node->children);
    }
}

//Fills rectangle struct with found data
void fillRect(Rectangle *rect, xmlAttr *attr)
{
    xmlNode *value = attr->children;
    char *attrName = (char *)attr->name;
    char *cont = (char *)(value->content);

    if (!strcmp(attrName, "x"))
    {
        rect->x = atof(cont);
    }
    else if (!strcmp(attrName, "y"))
    {
        rect->y = atof(cont);
    }
    else if (!strcmp(attrName, "width"))
    {
        rect->width = atof(cont);
    }
    else if (!strcmp(attrName, "height"))
    {
        rect->height = atof(cont);

        char *tmp = malloc(sizeof(char) * strlen(cont) + 1);
        strncpy(tmp, cont, strlen(cont) + 1);
        if (strcmp(cont, RemoveDigits(tmp)))
            strncpy(rect->units, RemoveDigits(tmp), 50);
        free(tmp);
    }
    else
    {
        Attribute *attrStruct = malloc(sizeof(Attribute));
        fillAttr(attrStruct, attr);
        insertBack(rect->otherAttributes, attrStruct);
    }
}

void fillCircle(Circle *circ, xmlAttr *attr)
{
    xmlNode *value = attr->children;
    char *attrName = (char *)attr->name;
    char *cont = (char *)(value->content);
    Attribute *attrStruct = NULL;

    if (circ == NULL)
        return;

    if (!strcmp(attrName, "cx"))
    {
        circ->cx = atof(cont);
    }

    else if (!strcmp(attrName, "cy"))
    {
        circ->cy = atof(cont);
    }

    else if (!strcmp(attrName, "r"))
    {
        circ->r = atof(cont);

        char *tmp = malloc(sizeof(char) * strlen(cont) + 1);
        strncpy(tmp, cont, strlen(cont) + 1);
        if (strcmp(cont, RemoveDigits(tmp)))
            strncpy(circ->units, RemoveDigits(tmp), 50);
        free(tmp);
    }

    else
    {
        //Allocates space to the struct that holds attribute data
        attrStruct = malloc(sizeof(Attribute));

        //Fills struct with data from circle node
        fillAttr(attrStruct, attr);

        //insert struct onto list
        insertBack(circ->otherAttributes, attrStruct);
    }
}

void fillPath(Path *path, xmlAttr *attr)
{
    xmlNode *value = attr->children;
    char *attrName = (char *)attr->name;
    char *cont = (char *)(value->content);
    char tmp[strlen(cont)];

    if (!strcmp(attrName, "d"))
    {
        strncpy(tmp, cont, strlen(cont) + 1);
        path->data = malloc(sizeof(char) * strlen(tmp) + 1);
        strncpy(path->data, tmp, strlen(tmp) + 1);
    }

    else
    {
        Attribute *attrStruct = malloc(sizeof(Attribute));

        fillAttr(attrStruct, attr);

        insertBack(path->otherAttributes, attrStruct);
    }
}

void fillGroup(Group *group, xmlNode *node)
{
    char *attrName = NULL;

    xmlNode *cur_node = node;
    //This function is based off of the example from starter.zip, specifically
    //the printxml function where the names and attributes are printed
    for (cur_node = node; cur_node != NULL; cur_node = cur_node->next)
    {
        Rectangle *rect = NULL;
        Circle *circle = NULL;
        Path *path = NULL;
        Group *newGroup = NULL;

        attrName = (char *)cur_node->name;

        if (!strncmp(attrName, "rect", 10))
        {
            rect = malloc(sizeof(Rectangle));
            rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

            xmlAttr *attr;
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                fillRect(rect, attr);
            }

            insertBack(group->rectangles, rect);
        }

        else if (!strcmp(attrName, "circle"))
        {
            circle = malloc(sizeof(Circle));
            circle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

            xmlAttr *attr = NULL;
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                fillCircle(circle, attr);
            }

            insertBack(group->circles, circle);
        }

        else if (!strcmp(attrName, "path"))
        {
            path = malloc(sizeof(Path));
            path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

            xmlAttr *attr = NULL;
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                fillPath(path, attr);
            }

            insertBack(group->paths, path);
        }

        else if (!strcmp(attrName, "g"))
        {
            newGroup = malloc(sizeof(Group));
            newGroup->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
            newGroup->circles = initializeList(circleToString, deleteCircle, compareCircles);
            newGroup->paths = initializeList(pathToString, deletePath, comparePaths);
            newGroup->groups = initializeList(groupToString, deleteGroup, compareGroups);
            newGroup->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

            fillGroup(newGroup, cur_node->children);

            xmlAttr *attr;
            for (attr = cur_node->properties; attr != NULL; attr = attr->next)
            {
                Attribute *attrStruct = malloc(sizeof(Attribute));
                fillAttr(attrStruct, attr);
                insertBack(newGroup->otherAttributes, attrStruct);
            }

            insertBack(group->groups, newGroup);
        }
    }
}

//Removes any digits from a string
char *RemoveDigits(char *input)
{
    char *dest = input;
    char *in = input;

    while(*in)
    {
        if (isdigit(*in) || !isalpha(*in))
        {
            in++;
            continue;
        }
        *dest++ = *in++;
    }
    *dest = '\0';
    return input;
}

//fills the attribute struct with data
void fillAttr(Attribute *attrStruct, xmlAttr *attr)
{
    //malloc space for string and copy name to struct
    if (attr->name != NULL)
    {
        attrStruct->name = malloc(sizeof(char) * strlen((char *)attr->name) + 1);
        strncpy(attrStruct->name, (char *)attr->name, strlen((char *)attr->name) + 1);
    }
        //malloc space for string and copy value to struct
    if (attr->children->content != NULL)
    {
        attrStruct->value = malloc(sizeof(char) * strlen((char *)attr->children->content) + 1);
        strncpy(attrStruct->value, (char *)attr->children->content, strlen((char *)attr->children->content) + 1);
    }
}

char* SVGimageToString(SVGimage* img)
{
    char *str = NULL;
    char *rectStr = NULL;
    char *circStr = NULL;
    char *groupStr = NULL;
    char *pathStr = NULL;
    char *attrStr = NULL;

    int length = 0;

    if (img == NULL)
    {
        return NULL;
    }

    length += strlen(img->namespace) + strlen(img->title) + strlen(img->description) + 13 + 9 + 15;

    if (img->rectangles != NULL)
    {
        rectStr = toString(img->rectangles);
        length += strlen(rectStr);
    }

    if (img->circles != NULL)
    {
        circStr = toString(img->circles);
        length += strlen(circStr);
    }

    if (img->paths != NULL)
    {
        pathStr = toString(img->paths);
        length += strlen(pathStr);
    }

    if (img->groups != NULL)
    {
        groupStr = toString(img->groups);
        length += strlen(groupStr);
    }

    if (img->otherAttributes != NULL)
    {
        attrStr = toString(img->otherAttributes);
        length += strlen(attrStr);
    }

    str = malloc(sizeof(char) * length + 1);

    strncpy(str, "\nNamespace = ", 25);
    strncat(str, img->namespace, 256);
    strncat(str, "\nTitle = ", 25);
    strncat(str, img->title, 256);
    strncat(str, "\nDescription = ", 25);
    strncat(str, img->description, 256);

    if (attrStr != NULL)
    {
        strncat(str, attrStr, strlen(attrStr) + 1);
        free(attrStr);
    }


    if (rectStr != NULL)
    {
        strncat(str, rectStr, strlen(rectStr) + 1);
        free(rectStr);
    }

    if (circStr != NULL)
    {
        strncat(str, circStr, strlen(circStr) + 1);
        free(circStr);
    }

    if (pathStr != NULL)
    {
        strncat(str, pathStr, strlen(pathStr) + 1);
        free(pathStr);
    }

    if (groupStr != NULL)
    {
        strncat(str, groupStr, strlen(groupStr) + 1);
        free(groupStr);
    }

    return str;
}

void deleteSVGimage(SVGimage* img)
{
    if (img == NULL)
        return;

    if (img->rectangles != NULL)
        freeList(img->rectangles);

    if (img->circles != NULL)
        freeList(img->circles);

    if (img->paths != NULL)
        freeList(img->paths);

    if (img->groups != NULL)
        freeList(img->groups);

    if (img->otherAttributes != NULL)
        freeList(img->otherAttributes);

    free(img);
}

List* getRects(SVGimage* img)
{
    if (img == NULL)
        return NULL;

    List *rectList = initializeList(rectangleToString, deleteRectangle, compareRectangles);

    listAdd(rectList, img->rectangles);

    getInGroup(rectList, img->groups, "rect");

    return rectList;
}

List* getCircles(SVGimage* img)
{
    if (img == NULL)
        return NULL;

    List *circList = initializeList(circleToString, deleteCircle, compareCircles);

    listAdd(circList, img->circles);

    getInGroup(circList, img->groups, "circ");

    return circList;
}

List* getPaths(SVGimage* img)
{
    if (img == NULL)
        return NULL;

    List *pathList = initializeList(pathToString, deletePath, comparePaths);

    listAdd(pathList, img->paths);

    getInGroup(pathList, img->groups, "path");

    return pathList;
}

List* getGroups(SVGimage* img)
{
    if (img == NULL)
        return NULL;

    List *groupList = initializeList(groupToString, deleteGroup, compareGroups);

    listAdd(groupList, img->groups);

    getInGroup(groupList, img->groups, "group");

    return groupList;
}

void getInGroup(List* curList, List* list_group, char *type)
{
    if (list_group == NULL || curList == NULL)
        return;

    Group *cur_group = NULL;

    int listLength = getLength(list_group);
    ListIterator group_iter = createIterator(list_group);

    for (int j = 0; j < listLength; j++)
    {
        cur_group = nextElement(&group_iter);

        if (!strcmp(type, "rect") && getLength(cur_group->rectangles) != 0)
        {
            listAdd(curList, cur_group->rectangles);
        }

        else if (!strcmp(type, "circ") && cur_group->circles != NULL && getLength(cur_group->circles) > 0)
        {
            listAdd(curList, cur_group->circles);
        }

        else if (!strcmp(type, "path") && cur_group->paths != NULL && getLength(cur_group->paths) > 0)
        {
            listAdd(curList, cur_group->paths);
        }

        else if (!strcmp(type, "group") && cur_group->groups != NULL && getLength(cur_group->groups))
        {
            listAdd(curList, cur_group->groups);
        }

        if (cur_group->groups != NULL && getLength(cur_group->groups) > 0)
        {
            getInGroup(curList, cur_group->groups, type);
        }
    }
}

void listAdd(List* list, List* toAdd)
{
    void *tmp = NULL;
    ListIterator iter = createIterator(toAdd);

    for (int i = 0; i < getLength(toAdd); i++)
    {
        tmp = nextElement(&iter);
        insertBack(list, tmp);
    }
}

int numRectsWithArea(SVGimage* img, float area)
{
    if (img == NULL || area < 0.0)
        return 0;

    List *rectList = getRects(img);
    ListIterator iter = createIterator(rectList);
    Rectangle *rect = nextElement(&iter);
    float calcArea = 0;
    int total = 0;

    for (int i = 0; i < getLength(rectList); i++)
    {
        calcArea = ceil(rect->height * rect->width);

        rect = nextElement(&iter);

        if (calcArea == ceil(area))
        {
            total++;
        }
    }

    freeGetList(rectList);

    return total;
}

int numCirclesWithArea(SVGimage* img, float area)
{
    if (img == NULL || area < 0.0)
        return 0;

    List *circList = getCircles(img);
    ListIterator iter = createIterator(circList);
    Circle *circ = nextElement(&iter);
    float calcArea = 0;
    int total = 0;

    for (int i = 0; i < getLength(circList); i++)
    {
        calcArea = ceil(PI * (circ->r * circ->r));

        circ = nextElement(&iter);

        if (calcArea == ceil(area))
        {
            total++;
        }
    }

    freeGetList(circList);

    return total;
}

int numPathsWithdata(SVGimage* img, char* data)
{
    if (img == NULL || data == NULL)
        return 0;

    int total = 0;
    List *paths = getPaths(img);
    ListIterator iter = createIterator(paths);
    Path *path = nextElement(&iter);

    for (int i = 0; i < getLength(paths); i++)
    {
        if (!strcmp(path->data, data))
            total++;

        path = nextElement(&iter);
    }

    freeGetList(paths);

    return total;
}

int numGroupsWithLen(SVGimage* img, int len)
{
    if (img == NULL || len < 0)
        return 0;

    int total = 0;
    List *pathList = getGroups(img);
    ListIterator iter = createIterator(pathList);
    Group *group = nextElement(&iter);

    for (int i = 0; i < getLength(pathList); i++)
    {
        int groupLength = getLength(group->rectangles) + getLength(group->circles) + getLength(group->paths) + getLength(group->groups);

        if (groupLength == len)
        {
            total++;
        }

        group = nextElement(&iter);
    }

    Node *node;
    while(pathList->head != NULL){
        node = pathList->head;
        pathList->head = pathList->head->next;
        free(node);
    }
    free(pathList);

    return total;
}

int numAttr(SVGimage* img)
{
    if (img == NULL)
        return 0;

    int total = 0;
    List *rectList = getRects(img);
    List *circList = getCircles(img);
    List *pathList = getPaths(img);
    List *groupList = getGroups(img);

    ListIterator rectIter = createIterator(rectList);
    Rectangle *rect = nextElement(&rectIter);
    for (int i = 0; i < getLength(rectList); i++)
    {
        total += getLength(rect->otherAttributes);
        rect = nextElement(&rectIter);
    }

    ListIterator circIter = createIterator(circList);
    Circle *circ = nextElement(&circIter);
    for (int i = 0; i < getLength(circList); i++)
    {
        total += getLength(circ->otherAttributes);
        circ = nextElement(&circIter);
    }

    ListIterator pathIter = createIterator(pathList);
    Path *path = nextElement(&pathIter);
    for (int i = 0; i < getLength(pathList); i++)
    {
        total += getLength(path->otherAttributes);
        path = nextElement(&pathIter);
    }

    ListIterator groupIter = createIterator(groupList);
    Group *group = nextElement(&groupIter);
    for (int i = 0; i < getLength(groupList); i++)
    {
        total += getLength(group->otherAttributes);
        group = nextElement(&groupIter);
    }

    total += getLength(img->otherAttributes);

    freeGetList(rectList);
    freeGetList(circList);
    freeGetList(pathList);
    freeGetList(groupList);

    return total;
}

void freeGetList(List *list)
{
    if (list == NULL)
        return;

    Node *node;
    while(list->head != NULL){
        node = list->head;
        list->head = list->head->next;
        free(node);
    }
    free(list);
}

/****************A2 Stuff******************/
SVGimage* createValidSVGimage(char* fileName, char* schemaFile)
{
    FILE *fptr = fopen(fileName, "r");
    if (fptr == NULL || fileName == NULL || schemaFile == NULL)
        return NULL;
    else
        fclose(fptr);

    char *empty_string = "empty string";

    xmlDocPtr doc = NULL;
    xmlNode *root_node = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    //Check file name is good and checks to see if file exists and is readable
    //if not then returns null
    if (doc == NULL)
    {
        freeXMLStuff(NULL, doc, NULL);
        return NULL;
    }

    root_node = xmlDocGetRootElement(doc);
    //determine if the xml file is valid, returns null if not
    if (root_node == NULL || root_node->type != XML_ELEMENT_NODE)
    {
        return NULL;
    }

    SVGimage *img = NULL;

    img = malloc(sizeof(SVGimage));
    strncpy(img->namespace, empty_string, strlen(empty_string) + 1);
    strncpy(img->title, empty_string, strlen(empty_string) + 1);
    strncpy(img->description, empty_string, strlen(empty_string) + 1);

    img->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    img->circles = initializeList(circleToString, deleteCircle, compareCircles);
    img->paths = initializeList(pathToString, deletePath, comparePaths);
    img->groups = initializeList(groupToString, deleteGroup, compareGroups);
    img->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    //checks to see if the first xml element is an svg and copies namespace into struct
    //, returns null if not
    if (root_node->type == XML_ELEMENT_NODE && !strcmp((char*)root_node->name, "svg"))
    {
        if (root_node->nsDef->href != NULL)
            strncpy(img->namespace, (char*)root_node->nsDef->href, 255);
        else
            strncpy(img->namespace, empty_string, strlen(empty_string));
    }
    else
    {
        return NULL;
    }

    getData(img, root_node);

    freeXMLStuff(NULL, doc, NULL);

    if (validateSVGimage(img, schemaFile) == false)
    {
        deleteSVGimage(img);
        return NULL;
    }

    return img;
}

bool writeSVGimage(SVGimage* image, char* fileName)
{
    if (image == NULL || fileName == NULL || strstr(fileName, ".svg") == NULL || !validateSVGimage(image, "svg.xsd"))
        return false;

    xmlDocPtr docPtr = NULL;

    docPtr = imageToxmlTree(image);

    if (docPtr == NULL)
    {
        return false;
    }

    xmlSaveFormatFileEnc(fileName, docPtr, "UTF-8", 1);
    xmlFreeDoc(docPtr);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}

bool validateSVGimage (SVGimage* image, char* schemaFile)
{
    if (image == NULL || schemaFile == NULL)
    {
        return false;
    }

    xmlDocPtr doc = NULL;

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt = NULL;

    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = imageToxmlTree(image);

    if (doc == NULL)
    {
        return false;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt = NULL;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);

        if (ret == 0)
        {
            xmlSchemaFreeValidCtxt(ctxt);
        }
        else
        {
            freeXMLStuff(schema, doc, ctxt);
            return false;
        }
    }

    freeXMLStuff(schema, doc, NULL);

    if (checkAttrList(image->otherAttributes) == false)
    {
        return false;
    }

    if (checkRectList(image->rectangles) == false)
    {
        return false;
    }

    if (checkCircList(image->circles) == false)
    {
        return false;
    }

    if (checkPathList(image->paths) == false)
    {
        return false;
    }

    if (checkGroupList(image->groups) == false)
    {
        return false;
    }

    return true;
}

void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute)
{
    if (image == NULL || newAttribute == NULL || !validateSVGimage(image, "svg.xsd"))
    {
        return;
    }

    if (newAttribute->name == NULL || newAttribute->value == NULL)
    {
        return;
    }

    if (elemType == SVG_IMAGE)
    {
        setAttr(image->otherAttributes, newAttribute);
    }
    else if (elemType == RECT)
    {
        int rectLength = getLength(image->rectangles);
        if (elemIndex >= rectLength || rectLength <= 0)
        {
            deleteAttribute(newAttribute);
            return;
        }

        setRect(image->rectangles, newAttribute, elemIndex);
    }
    else if (elemType == CIRC)
    {
        int circLength = getLength(image->circles);
        if (elemIndex >= circLength || circLength <= 0)
        {
            deleteAttribute(newAttribute);
            return;
        }

        setCirc(image->circles, newAttribute, elemIndex);
    }
    else if (elemType == PATH)
    {
        int pathLength = getLength(image->paths);
        if (elemIndex >= pathLength || pathLength <= 0)
        {
            deleteAttribute(newAttribute);
            return;
        }

        setPath(image->paths, newAttribute, elemIndex);
    }
    else if (elemType == GROUP)
    {
        int groupLength = getLength(image->groups);
        if (elemIndex >= groupLength || groupLength <= 0)
        {
            deleteAttribute(newAttribute);
            return;
        }

        setGroup(image->groups, newAttribute, elemIndex);
    }

    deleteAttribute(newAttribute);
}

void addComponent(SVGimage* image, elementType type, void* newElement)
{
    if (image == NULL || !validateSVGimage(image, "svg.xsd") || newElement == NULL)
    {
        return;
    }

    if (type == CIRC)
    {
        Circle *circ = newElement;
        if (circ->r < 0 || checkAttrList(circ->otherAttributes) == false)
        {
            return;
        }
        insertBack(image->circles, circ);
    }
    else if (type == RECT)
    {
        Rectangle *rect = newElement;
        if (rect->width < 0 || rect->height < 0 || checkAttrList(rect->otherAttributes) == false)
        {
            return;
        }
        insertBack (image->rectangles, rect);
    }
    else if (type == PATH)
    {
        Path *path = newElement;
        if (path->data == NULL || checkAttrList(path->otherAttributes) == false)
        {
            return;
        }
        insertBack (image->paths, path);
    }
}

char* attrToJSON(const Attribute *a)
{
    char *jStr = NULL;
    int length = 0;
    char name[] = "name";
    char value[] = "value";

    if (a == NULL)
    {
        return nullHandler();
    }

    length += strlen(a->name) + strlen(a->value) + strlen(name) + strlen(value) + strlen("{\"\":\"\",\"\":\"\"}");
    jStr = malloc(sizeof(char) * (length + 1));

    sprintf(jStr, "{\"%s\":\"%s\",\"%s\":\"%s\"}", name, a->name, value, a->value);

    return jStr;
}

char* circleToJSON(const Circle *c)
{
    char *cStr = NULL;

    if (c == NULL)
    {
        return nullHandler();
    }

    int length = 0;
    char cx[] = "cx";
    char cy[] = "cy";
    char r[] = "r";
    char numAttr[] = "numAttr";
    char units[] = "units";

    length += strlen(cx) + strlen(cy) + strlen(r) + strlen(numAttr) + strlen(units) + strlen("{\"\":,\"\":,\"\":,\"\":,\"\":\"\"}");

    char cxVal[80];
    char cyVal[80];
    char rVal[80];
    char lengthAttr[80];
    sprintf(cxVal, "%.2f", c->cx);
    sprintf(cyVal, "%.2f", c->cy);
    sprintf(rVal, "%.2f", c->r);
    sprintf(lengthAttr, "%d", getLength(c->otherAttributes));
    length += strlen(cxVal) + strlen(cyVal) + strlen(rVal) + strlen(lengthAttr) + strlen(c->units);

    cStr = malloc(sizeof(char) * (length+ 1));
    sprintf(cStr, "{\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":\"%s\"}",
            cx, cxVal, cy, cyVal, r, rVal, numAttr, lengthAttr, units, c->units);

    return cStr;
}

char* rectToJSON(const Rectangle *r)
{
    char *str = NULL;

    if (r == NULL)
    {
        return nullHandler();
    }

    int length = 0;
    char x[] = "x";
    char y[] = "y";
    char width[] = "w";
    char height[] = "h";
    char numAttr[] = "numAttr";
    char units[] = "units";

    length += strlen(x) + strlen(y) + strlen(width) + strlen(height)
            + strlen(numAttr) + strlen(units) + strlen("{\"\":,\"\":,\"\":,\"\":,\"\":,\"\":\"\"}");

    char xVal[80];
    char yVal[80];
    char widthVal[80];
    char heightVal[80];
    char lengthAttr[80];
    sprintf(xVal, "%.2f", r->x);
    sprintf(yVal, "%.2f", r->y);
    sprintf(widthVal, "%.2f", r->width);
    sprintf(heightVal, "%.2f", r->height);
    sprintf(lengthAttr, "%d", getLength(r->otherAttributes));
    length += strlen(xVal) + strlen(yVal) + strlen(widthVal) + strlen(heightVal) + strlen(lengthAttr) + strlen(r->units);

    str = malloc(sizeof(char) * (length+ 1));
    sprintf(str, "{\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":\"%s\"}",
            x, xVal, y, yVal, width, widthVal, height, heightVal, numAttr, lengthAttr, units, r->units);

    return str;
}

char* pathToJSON(const Path *p)
{
    char *str = NULL;

    if (p == NULL)
    {
        return nullHandler();
    }

    int length = 0;
    char data[] = "d";
    char numAttr[] = "numAttr";
    char dVal[65] = "\0";

    char lengthAttr[40];
    sprintf(lengthAttr, "%d", getLength(p->otherAttributes));

    strncpy(dVal, p->data, 64);

    length += strlen(data) + strlen(dVal) + strlen(numAttr) + strlen(lengthAttr) + strlen("{\"\":\"\",\"\":}");

    str = malloc(sizeof(char) * (length+ 1));
    sprintf(str, "{\"%s\":\"%s\",\"%s\":%s}", data, dVal, numAttr, lengthAttr);

    return str;
}

char* groupToJSON(const Group *g)
{
    char *str = NULL;

    if (g == NULL)
    {
        return nullHandler();
    }

    int length = 0;
    char name[] = "children";
    char numAttr[] = "numAttr";

    int cVal = 0;
    char cValStr[40];

    cVal = getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups);
    sprintf(cValStr, "%d", cVal);

    char lengthAttr[40];
    sprintf(lengthAttr, "%d", getLength(g->otherAttributes));

    length += strlen(name) + strlen(cValStr) + strlen(numAttr) + strlen(lengthAttr) + strlen("{\"\":,\"\":}");

    str = malloc(sizeof(char) * (length + 1));
    sprintf(str, "{\"%s\":%s,\"%s\":%s}", name, cValStr, numAttr, lengthAttr);

    return str;
}

char* SVGtoJSON(const SVGimage* imge)
{
    char *str = NULL;

    if (imge == NULL)
    {
        return nullHandler();
    }

    int length = 0;

    char rName[] = "numRect";
    char cName[] = "numCirc";
    char pName[] = "numPaths";
    char gName[] = "numGroups";

    length += strlen(rName) + strlen(cName) + strlen(pName) + strlen(gName)
            + strlen("{\"\":,\"\":,\"\":,\"\":}");

    SVGimage *img = (SVGimage *) imge;

    List *rectList = getRects(img);
    List *circList = getCircles(img);
    List *pathList = getPaths(img);
    List *groupList = getGroups(img);

    int rectLength = getLength(rectList);
    int circLength = getLength(circList);
    int pathLength = getLength(pathList);
    int groupLength =getLength(groupList);

    char rStr[40];
    char cStr[40];
    char pStr[40];
    char gStr[40];

    sprintf(rStr, "%d", rectLength);
    sprintf(cStr, "%d", circLength);
    sprintf(pStr, "%d", pathLength);
    sprintf(gStr, "%d", groupLength);

    length += strlen(rStr) + strlen(cStr) + strlen(pStr) + strlen(gStr);

    str = malloc(sizeof(char) * (length + 1));
    sprintf(str, "{\"%s\":%s,\"%s\":%s,\"%s\":%s,\"%s\":%s}", rName, rStr, cName, cStr, pName, pStr, gName, gStr);

    freeGetList(rectList);
    freeGetList(circList);
    freeGetList(pathList);
    freeGetList(groupList);

    return str;
}

char* attrListToJSON(const List *list)
{
    char *str = NULL;

    if (list == NULL)
    {
        return nullHandler2();
    }

    int length = 0;

    length += strlen("[]");

    ListIterator iter = createIterator((List *)list);
    Attribute *attr = nextElement(&iter);
    while (attr != NULL)
    {
        char *attrStr = attrToJSON(attr);
        length += strlen(attrStr) + 1;
        attr = nextElement(&iter);
        free(attrStr);
    }
    length += -1; //accounts for not putting comma at the end of the last attribute

    int listLength = getLength((List *)list);

    str = malloc(sizeof(char) * (length + (listLength - 1) + 3));
    strcpy(str, "[");

    iter = createIterator((List *)list);
    attr = nextElement(&iter);
    for (int i = 0; i < listLength; i++)
    {
        char *attrStr = attrToJSON(attr);

        strncat(str, attrStr, strlen(attrStr));

        if (i < listLength - 1)
        {
            strcat(str, ",");
        }
        attr = nextElement(&iter);
        free(attrStr);
    }

    strcat(str, "]\0");

    return str;
}

char* circListToJSON(const List *list)
{
    char *str = NULL;

    if (list == NULL)
    {
        return nullHandler2();
    }

    int length = 0;

    length += strlen("[]");

    ListIterator iter = createIterator((List *)list);
    Circle *circ = nextElement(&iter);
    while (circ != NULL)
    {
        char *circStr = circleToJSON(circ);
        length += strlen(circStr) + 1;
        circ = nextElement(&iter);
        free(circStr);
    }
    length += -1; //accounts for not putting comma at the end of the last attribute
    int listLength = getLength((List *)list);

    str = malloc(sizeof(char) * (length + (listLength - 1) + 3));
    strcpy(str, "[");

    iter = createIterator((List *)list);
    circ = nextElement(&iter);
    for (int i = 0; i < listLength; i++)
    {
        char *circStr = circleToJSON(circ);

        strncat(str, circStr, strlen(circStr));

        if (i < listLength - 1)
        {
            strcat(str, ",");
        }
        circ = nextElement(&iter);
        free(circStr);
    }

    strcat(str, "]\0");

    return str;
}

char* rectListToJSON(const List *list)
{
    char *str = NULL;

    if (list == NULL || getLength((List *)list) == 0)
    {
        return nullHandler2();
    }

    int length = 0;

    length += strlen("[]");

    ListIterator iter = createIterator((List *)list);
    Rectangle *rect = nextElement(&iter);
    while (rect != NULL)
    {
        char *rectStr = rectToJSON(rect);
        length += strlen(rectStr) + 1;
        rect = nextElement(&iter);
        free(rectStr);
    }
    length += -1; //accounts for not putting comma at the end of the last attribute

    int listLength = getLength((List *)list);

    str = malloc(sizeof(char) * (length + (listLength - 1) + 3));
    strcpy(str, "[");

    iter = createIterator((List *)list);
    rect = nextElement(&iter);
    for (int i = 0; i < listLength; i++)
    {
        char *rectStr = rectToJSON(rect);

        strncat(str, rectStr, strlen(rectStr));

        if (i < listLength - 1)
        {
            strcat(str, ",");
        }
        rect = nextElement(&iter);
        free(rectStr);
    }

    strcat(str, "]");

    return str;
}

char* pathListToJSON(const List *list)
{
    char *str = NULL;

    if (list == NULL)
    {
        return nullHandler2();
    }

    int length = 0;

    length += strlen("[]");

    ListIterator iter = createIterator((List *)list);
    Path *path = nextElement(&iter);
    while (path != NULL)
    {
        char *pathStr = pathToJSON(path);
        length += strlen(pathStr) + 1;
        path = nextElement(&iter);
        free(pathStr);
    }
    length += -1; //accounts for not putting comma at the end of the last attribute
    int listLength = getLength((List *)list);

    str = malloc(sizeof(char) * (length + (listLength - 1) + 3));
    strncpy(str, "[", 2);

    iter = createIterator((List *)list);
    path = nextElement(&iter);
    for (int i = 0; i < listLength; i++)
    {
        char *pathStr = pathToJSON(path);

        strncat(str, pathStr, strlen(pathStr));

        if (i < listLength - 1)
        {
            strncat(str, ",", 2);
        }
        path = nextElement(&iter);
        free(pathStr);
    }

    strncat(str, "]", 2);

    return str;
}

char* groupListToJSON(const List *list)
{
    char *str = NULL;

    if (list == NULL)
    {
        return nullHandler2();
    }

    int length = 0;

    length += strlen("[]");

    ListIterator iter = createIterator((List *)list);
    Group *group = nextElement(&iter);
    while (group != NULL)
    {
        char *groupStr = groupToJSON(group);
        length += strlen(groupStr) + 1;
        group = nextElement(&iter);
        free(groupStr);
    }
    length += -1; //accounts for not putting comma at the end of the last attribute

    int listLength = getLength((List *)list);

    str = malloc(sizeof(char) * (length + (listLength - 1) + 3));
    strncpy(str, "[", 2);

    iter = createIterator((List *)list);
    group = nextElement(&iter);
    for (int i = 0; i < listLength; i++)
    {
        char *groupStr = groupToJSON(group);

        strncat(str, groupStr, strlen(groupStr));

        if (i < listLength - 1)
        {
            strncat(str, ",", 2);
        }
        group = nextElement(&iter);
        free(groupStr);
    }

    strncat(str, "]", 2);

    return str;
}

/****************A3 Stuff******************/

char *getTitleAndDesc(const SVGimage *img)
{
    if (img == NULL)
    {
        return NULL;
    }

    int tLength = 0;
    int dLength = 0;

    tLength = strlen(img->title);
    dLength = strlen(img->description);

    char tStr[] = "title:";
    char dStr[] = "description:";

    int length = tLength + dLength + strlen(tStr) + strlen(dStr);

    char *str = NULL;
    str = malloc(sizeof(char) * (length) + 11 + 1);
    sprintf(str, "{\"title\":\"%s\", \"description\":\"%s\"}", img->title, img->description);

    return str;
}

char *JSONcreator (const SVGimage *img)
{
    if (img == NULL)
    {
        return NULL;
    }

    char allRect[5000] = "\0";
    char allCirc[5000] = "\0";
    char allPath[5000] = "\0";
    char allGroup[5000] = "\0";

    int length = 0;

    char *rectTmp = NULL;
    rectTmp = rectListToJSON(img->rectangles);
    length += strlen(rectTmp) + 1;
    char *rectAttrList = NULL;
    ListIterator iter = createIterator(img->rectangles);
    Rectangle *rect = nextElement(&iter);
    if (rect == NULL)
    {
        rectAttrList = nullHandler2();
        strcat(allRect, rectAttrList);
    }
    int counter = 0;
    while (rect != NULL)
    {
        if (getLength(rect->otherAttributes) > 0)
        {
            rectAttrList = attrListToJSON(rect->otherAttributes);
            length += strlen(rectAttrList);
        }
        else
        {
            rectAttrList = nullHandler2();
            length += strlen(rectAttrList);
        }
        strcat(allRect, rectAttrList);
        rect = nextElement(&iter);
        counter++;
        if (counter < getLength(img->rectangles))
        {
            strcat(allRect, ",");
        }
    }

    char *circTmp = circListToJSON(img->circles);
    length += strlen(circTmp);
    char *circList = NULL;
    iter = createIterator(img->circles);
    Circle *circ = nextElement(&iter);
    if (circ == NULL)
    {
        circList = nullHandler2();
        strcat(allCirc, circList);
    }
    counter = 0;
    while (circ != NULL)
    {
        if (getLength(circ->otherAttributes) > 0)
        {
            circList = attrListToJSON(circ->otherAttributes);
            length += strlen(circList);
        }
        else
        {
            circList = nullHandler2();
            length += strlen(circList);
        }
        strcat(allCirc, circList);
        circ = nextElement(&iter);
        counter++;
        if (counter < getLength(img->circles))
        {
            strcat(allCirc, ",");
        }
    }

    char *pathTmp = pathListToJSON(img->paths);
    length += strlen(pathTmp);
    char *pathList = NULL;
    iter = createIterator(img->paths);
    Path *path = nextElement(&iter);
    if (path == NULL)
    {
        pathList = nullHandler2();
        strcat(allPath, pathList);
    }
    counter = 0;
    while (path != NULL)
    {
        if (getLength(path->otherAttributes) > 0)
        {
            pathList = attrListToJSON(path->otherAttributes);
            length += strlen(pathList);
        }
        else
        {
            pathList = nullHandler2();
            length += strlen(pathList);
        }
        strcat(allPath, pathList);
        path = nextElement(&iter);
        counter++;
        if (counter < getLength(img->paths))
        {
            strcat(allPath, ",");
        }
    }

    char *groupTmp = groupListToJSON(img->groups);
    length += strlen(groupTmp);
    char *groupList = NULL;
    iter = createIterator(img->groups);
    Group *gr = nextElement(&iter);
    if (gr == NULL)
    {
        groupList = nullHandler2();
        strcat(allGroup, groupList);
    }
    counter = 0;
    while (gr != NULL)
    {
        if (getLength(gr->otherAttributes) > 0)
        {
            groupList = attrListToJSON(gr->otherAttributes);
            length += strlen(groupList);
        }
        else
        {
            groupList = nullHandler2();
            length += strlen(groupList);
        }
        gr = nextElement(&iter);
        strcat(allGroup, groupList);
        counter++;
        if (counter < getLength(img->groups))
        {
            strcat(allGroup, ",");
        }
    }

    char tmp[756];
    sprintf(tmp, "[{\"title\":\"%s\"},{\"description\":\"%s\"}]", img->title, img->description);
    length += strlen(tmp);

    char *str = malloc(sizeof(char) * (length + 25));

    strcpy(str, "[");

    strcat(str, rectTmp);
    strcat(str, ",[");
    strcat(str, allRect);
    strcat(str, "],");

    strncat(str, circTmp, strlen(circTmp));
    strcat(str, ",[");
    strcat(str, allCirc);
    strcat(str, "],");

    strncat(str, pathTmp, strlen(pathTmp));
    strcat(str, ",[");
    strcat(str, allPath);
    strcat(str, "],");

    strncat(str, groupTmp, strlen(groupTmp));
    strcat(str, ",[");
    strcat(str, allGroup);
    strcat(str, "],");

    strcat(str, tmp);
    strcat(str, "]\0");

    //printf("IN C: %s\n", str);

    free(rectTmp);
    free(circTmp);
    free(pathTmp);
    free(groupTmp);
    free(rectAttrList);
    free(circList);
    free(pathList);
    free(groupList);

    return str;
}

bool editTitleDesc(SVGimage *img, char *title, char *desc)
{
    if (img == NULL || title == NULL || desc == NULL)
    {
        return false;
    }

    if (strlen(title) > 256 || strlen(desc) > 256)
    {
        return false;
    }

    if (strlen(title) > 0)
    {
        strncpy(img->title, title, 255);
    }

    if (strlen(desc) > 0)
    {
        strncpy(img->description, desc, 255);
    }

    return true;
}

void freeStr(char *str)
{
    free(str);
}

/****************A2 Helpers******************/
void freeXMLStuff(xmlSchemaPtr schema, xmlDoc *doc, xmlSchemaValidCtxtPtr ctxt)
{
    if (schema != NULL)
        xmlSchemaFree(schema);

    if (ctxt != NULL)
        xmlSchemaFreeValidCtxt(ctxt);

    xmlFreeDoc(doc);
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
}

xmlDocPtr imageToxmlTree(SVGimage* img)
{
    if (img == NULL)
        return NULL;

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    xmlNs *nameSpace = xmlNewNs(root_node, BAD_CAST img->namespace, NULL);
    xmlSetNs(root_node, nameSpace);

    if (img->otherAttributes != NULL && getLength(img->otherAttributes) > 0)
    {
        addAttributeToTree(root_node, img->otherAttributes);
    }

    if (strlen(img->title) > 0)
    {
        xmlNewChild(root_node, nameSpace, BAD_CAST "title", BAD_CAST img->title);
    }

    if (strlen(img->description) > 0 && !strstr(img->description, "empty string"))
    {
        xmlNewChild(root_node, nameSpace, BAD_CAST "desc", BAD_CAST img->description);
    }

    if (img->rectangles != NULL && getLength(img->rectangles))
    {
        addRects(root_node, img->rectangles);
    }

    if (img->circles != NULL && getLength(img->circles))
    {
        addCircles(root_node, img->circles);
    }

    if (img->paths != NULL && getLength(img->paths))
    {
        addPaths(root_node, img->paths);
    }

    //Adds every group in the list of groups
    ListIterator iter = createIterator(img->groups);
    Group *group = nextElement(&iter);
    while (group != NULL)
    {
        addGroups(root_node, group);
        group = nextElement(&iter);
    }

    return doc;
}

void addRects (xmlNodePtr root_node, List *list)
{
    char tmp[100];
    char units[50];

    xmlNodePtr node = NULL;

    ListIterator iter = createIterator(list);
    Rectangle *rect = nextElement(&iter);

    //while (rect != NULL)
    for (int i = 0; i < getLength(list); i++)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL); //1st node, namspace, name, content

        if (strlen(rect->units) > 0)
        {
            strncpy(units, rect->units, 49);
        }

        gcvt(rect->x, 49, tmp);
        if (strlen(rect->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST tmp);

        gcvt(rect->y, 49, tmp);
        if (strlen(rect->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST tmp);

        gcvt(rect->width, 49, tmp);
        if (strlen(rect->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST tmp);

        gcvt(rect->height, 49, tmp);
        if (strlen(rect->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST tmp);

        if (rect->otherAttributes != NULL && getLength(rect->otherAttributes) > 0)
        {
            addAttributeToTree(node, rect->otherAttributes);
        }

        rect = nextElement(&iter);
    }
}

void addCircles (xmlNodePtr root_node, List* list)
{
    char tmp[100];
    char units[50];
    xmlNodePtr node = NULL;

    ListIterator iter = createIterator(list);
    Circle *circ = nextElement(&iter);
    //while (circ != NULL)
    for (int i = 0; i < getLength(list); i++)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL); //1st node, namspace, name, content

        if (strlen(circ->units) > 0)
        {
            strncpy(units, circ->units, 49);
        }

        gcvt(circ->cx, 49, tmp);
        if (strlen(circ->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "cx", BAD_CAST tmp);

        gcvt(circ->cy, 49, tmp);
        if (strlen(circ->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST tmp);

        gcvt(circ->r, 49, tmp);
        if (strlen(circ->units) > 0)
            strncat(tmp, units, 49);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST tmp);

        if (circ->otherAttributes != NULL && getLength(circ->otherAttributes) > 0)
        {
            addAttributeToTree(node, circ->otherAttributes);
        }

        circ = nextElement(&iter);
    }
}

void addPaths (xmlNodePtr root_node, List* list)
{
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(list);
    Path *path = nextElement(&iter);
    while (path != NULL)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL); //1st node, namspace, name, content

        xmlNewProp(node, BAD_CAST "d", BAD_CAST path->data);

        if (path->otherAttributes != NULL && getLength(path->otherAttributes) > 0)
        {
            addAttributeToTree(node, path->otherAttributes);
        }

        path = nextElement(&iter);
    }
}

void addGroups (xmlNodePtr parent_node, Group *group)
{
    xmlNodePtr node = parent_node;

    node = xmlNewChild(node, NULL, BAD_CAST "g", NULL);

    if (group->otherAttributes != NULL && getLength(group->otherAttributes) > 0)
    {
        addAttributeToTree(node, group->otherAttributes);
    }

    if (group->rectangles != NULL && getLength(group->rectangles) > 0)
    {
        addRects(node, group->rectangles);
    }

    if (group->circles != NULL && getLength(group->circles) > 0)
    {
        addCircles(node, group->circles);
    }

    if (group->paths != NULL && getLength(group->paths) > 0)
    {
        addPaths(node, group->paths);
    }

    //add the list of groups recursively
    ListIterator iter = createIterator(group->groups);
    Group *groupTwo = nextElement(&iter);
    while (groupTwo != NULL)
    {
        addGroups(node, groupTwo);
        groupTwo = nextElement(&iter);
    }
}

void getUnits (char *str, char *units)
{
    if (str == NULL || units == NULL || strlen(units) <= 0)
        return;

    if (strlen(units) > 0)
    {
        strncpy(str, units, 49);
    }
}

void addAttributeToTree (xmlNodePtr node, List *list)
{
    ListIterator attrIter = createIterator(list);
    Attribute *attr = nextElement(&attrIter);
    while (attr != NULL)
    {
        xmlNewProp(node, BAD_CAST attr->name, BAD_CAST attr->value);
        attr = nextElement(&attrIter);
    }
}

void setRect(List *list, Attribute *newAttribute, int elemIndex)
{
    ListIterator iter = createIterator(list);
    Rectangle *rect = nextElement(&iter);
    for (int i = 0; i < elemIndex; i++)
    {
        rect = nextElement(&iter);
    }
    if (!strcmp(newAttribute->name, "x"))
    {
        rect->x = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "y"))
    {
        rect->y = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "height"))
    {
        rect->height = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "width"))
    {
        rect->width = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "units"))
    {
        strncpy(rect->units, newAttribute->value, 49);
    }
    else
    {
        setAttr(rect->otherAttributes, newAttribute);
    }
}

void setCirc(List *list, Attribute *newAttribute, int elemIndex)
{
    ListIterator iter = createIterator(list);
    Circle *circ = nextElement(&iter);
    for (int i = 0; i < elemIndex; i++)
    {
        circ = nextElement(&iter);
    }
    if (!strcmp(newAttribute->name, "cx"))
    {
        circ->cx = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "cy"))
    {
        circ->cy = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "r"))
    {
        circ->r = atof(newAttribute->value);
    }
    else if (!strcmp(newAttribute->name, "units"))
    {
        strncpy(circ->units, newAttribute->value, 49);
    }
    else
    {
        setAttr(circ->otherAttributes, newAttribute);
    }
}

void setPath(List *list, Attribute *newAttribute, int elemIndex)
{
    ListIterator iter = createIterator(list);
    Path *path = nextElement(&iter);
    for (int i = 0; i < elemIndex; i++)
    {
        path = nextElement(&iter);
    }
    if (!strcmp(newAttribute->name, "d"))
    {
        path->data = (char *) realloc(path->data, strlen(newAttribute->value) + 1);
        strncpy(path->data, newAttribute->value, strlen(newAttribute->value) + 1);
    }
    else
    {
        setAttr(path->otherAttributes, newAttribute);
    }
}

void setGroup(List *list, Attribute *newAttribute, int elemIndex)
{
    ListIterator iter = createIterator(list);
    Group *group = nextElement(&iter);
    for (int i = 0; i < elemIndex; i++)
    {
        group = nextElement(&iter);
    }

    setAttr(group->otherAttributes, newAttribute);
}

void setAttr(List *list, Attribute *newAttribute)
{
    if (list == NULL)
    {
        list = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }

    bool found = false;
    ListIterator attrIter = createIterator(list);
    Attribute *attr = nextElement(&attrIter);
    for (int i = 0; i < getLength(list); i++)
    {
        if (!strcmp(newAttribute->name, attr->name))
        {
            attr->value = (char *) realloc(attr->value, strlen(newAttribute->value) + 1);
            strncpy(attr->value, newAttribute->value, strlen(newAttribute->value) + 1);
            found = true;
        }
        attr = nextElement(&attrIter);
    }

    if (found == false)
    {
        Attribute *newAttr = malloc(sizeof(Attribute));
        newAttr->name = malloc(sizeof(char) * (strlen(newAttribute->name) + 1));
        newAttr->value = malloc(sizeof(char) * (strlen(newAttribute->value) + 1));
        strncpy(newAttr->name, newAttribute->name, strlen(newAttribute->name) + 1);
        strncpy(newAttr->value, newAttribute->value, strlen(newAttribute->value) + 1);
        insertBack(list, newAttr);
    }
}

char *nullHandler()
{
    char *str = NULL;
    char empStr[] = "{}";
    str = malloc(sizeof(char) * (strlen(empStr) + 1));
    sprintf(str, "%s", empStr);
    return str;
}

char *nullHandler2()
{
    char *str = NULL;
    char empStr[] = "[]";
    str = malloc(sizeof(char) * (strlen(empStr) + 1));
    sprintf(str, "%s", empStr);
    return str;
}

bool checkAttrList(List *list)
{
    if (list == NULL)
    {
        return false;
    }

    ListIterator iter = createIterator(list);
    Attribute *attr = nextElement(&iter);
    while (attr != NULL)
    {
        if (attr->name == NULL || attr->value == NULL)
        {
            return false;
        }
        attr = nextElement(&iter);
    }

    return true;
}

bool checkRectList(List *list)
{
    if (list == NULL)
    {
        return false;
    }

    ListIterator iter = createIterator(list);
    Rectangle *rect = nextElement(&iter);
    while (rect != NULL)
    {
        if (checkAttrList(rect->otherAttributes) == false || rect->width < 0 || rect->height < 0)
        {
            return false;
        }
        rect = nextElement(&iter);
    }

    return true;
}

bool checkCircList(List *list)
{
    if (list == NULL)
    {
        return false;
    }

    ListIterator iter = createIterator(list);
    Circle *circ = nextElement(&iter);
    while (circ != NULL)
    {
        if (checkAttrList(circ->otherAttributes) == false || circ->r < 0)
        {
            return false;
        }
        circ = nextElement(&iter);
    }

    return true;
}

bool checkPathList(List *list)
{
    if (list == NULL)
    {
        return false;
    }

    ListIterator iter = createIterator(list);
    Path *path = nextElement(&iter);
    while (path != NULL)
    {
        if (checkAttrList(path->otherAttributes) == false || path->data == NULL)
        {
            return false;
        }
        path = nextElement(&iter);
    }

    return true;
}

bool checkGroupList(List *list)
{
    if (list == NULL)
    {
        return false;
    }

    ListIterator iter = createIterator(list);
    Group *group = nextElement(&iter);
    while (group != NULL)
    {
        if (checkAttrList(group->otherAttributes) == false || checkRectList(group->rectangles) == false
         || checkCircList(group->circles) == false || checkPathList(group->paths) == false || checkGroupList(group->groups) == false)
        {
            return false;
        }
        group = nextElement(&iter);
    }

    return true;
}

/********A1 HELPER FUNCTIONS*********/
void deleteAttribute( void* data)
{
    Attribute *tmpAttr = data;

    if (data == NULL)
        return;

    if (tmpAttr->name != NULL)
    {
        free(tmpAttr->name);
        tmpAttr->name = NULL;
    }

    if (tmpAttr->value != NULL)
    {
        free(tmpAttr->value);
        tmpAttr->value = NULL;
    }

    free(tmpAttr);
    tmpAttr = NULL;
}

char* attributeToString( void* data)
{
    Attribute *attr = data;
    char *attrStr = NULL;

    int length = 0;

    if (data == NULL)
        return NULL;

    length += strlen("Attribute: ") + strlen(attr->name) + strlen(", value = ") + strlen(attr->value);

    attrStr = malloc(sizeof(char) * (length + 1));

    strncpy(attrStr, "Attribute: ", 20);
    strncat(attrStr, attr->name, strlen(attr->name) + 1);
    strncat(attrStr, ", value = ", 20);
    strncat(attrStr, attr->value, strlen(attr->value) + 1);

    return attrStr;
}

int compareAttributes(const void *first, const void *second)
{
    return 0;
}

void deleteGroup(void* data)
{
    Group *group = data;

    if (data == NULL)
        return;

    if (group->rectangles != NULL)
        freeList(group->rectangles);

    if (group->circles != NULL)
        freeList(group->circles);

    if (group->paths != NULL)
        freeList(group->paths);

    if (group->groups != NULL)
        freeList(group->groups);

    if (group->otherAttributes != NULL)
        freeList(group->otherAttributes);

    free(group);
    group = NULL;
}

char* groupToString( void* data)
{
    Group *group = data;
    char *rectList = NULL;
    char *circleList = NULL;
    char *pathList = NULL;
    char *groupList = NULL;
    char *attrList = NULL;
    char *returnStr = NULL;
    char tmpStr[] = "\nGroup:";

    int strLengths = 0;

    if (data == NULL)
        return NULL;

    if (group->rectangles != NULL && getLength(group->rectangles) > 0)
    {
        char *rectStr = toString(group->rectangles);
        rectList = malloc(sizeof(char) * strlen(rectStr) + 1);
        strncpy(rectList, rectStr, strlen(rectStr) + 1);
        strLengths += strlen(rectList);
        free(rectStr);
    }

    if (group->circles != NULL || getLength(group->circles) > 0)
    {
        char *circStr = toString(group->circles);
        circleList = malloc(sizeof(char) * strlen(circStr) + 1);
        strncpy(circleList, circStr, strlen(circStr) + 1);
        strLengths += strlen(circleList);
        free(circStr);
    }


    if (group->paths != NULL && getLength(group->paths) > 0)
    {
        char *pathStr = toString(group->paths);
        pathList = malloc(sizeof(char) * strlen(pathStr) + 1);
        strncpy(pathList, pathStr, strlen(pathStr) + 1);
        strLengths += strlen(pathList);
        free(pathStr);
    }

    if (group->groups != NULL && getLength(group->groups) > 0)
    {
        char *groupStr = toString(group->groups);
        groupList = malloc(sizeof(char) * strlen(groupStr) + 1);
        strncpy(groupList, groupStr, strlen(groupStr) + 1);
        strLengths += strlen(groupList);
        free(groupStr);
    }

    if (group->otherAttributes != NULL && getLength(group->otherAttributes) > 0)
    {
        char *attrStr = toString(group->otherAttributes);
        attrList = malloc(sizeof(char) * (strlen(attrStr) + 20 + 1));
        strncpy(attrList, "\nOther Attributes: ", 25);
        strncat(attrList, attrStr, strlen(attrStr) + 1);
        strLengths += strlen(attrList);
        free(attrStr);
    }

    returnStr = calloc(sizeof(char) * strLengths + strlen(tmpStr) + 1, 1);
    strncpy(returnStr, tmpStr, strlen(tmpStr));

    if (attrList != NULL)
    {
        strncat(returnStr, attrList, strlen(attrList));
        free(attrList);
    }

    if (rectList != NULL)
    {
        strncat(returnStr, rectList, strlen(rectList));
        free(rectList);
    }

    if (circleList != NULL)
    {
        strncat(returnStr, circleList, strlen(circleList));
        free(circleList);
    }

    if (pathList != NULL)
    {
        strncat(returnStr, pathList, strlen(pathList));
        free(pathList);
    }

    if (groupList != NULL)
    {
        strncat(returnStr, groupList, strlen(groupList));
        free(groupList);
    }

    return returnStr;
}

int compareGroups(const void *first, const void *second)
{
    return 0;
}

void deleteRectangle(void* data)
{
    Rectangle *rect = data;

    if (data == NULL)
        return;

    if (rect->otherAttributes != NULL)
        freeList(rect->otherAttributes);

    free(rect);
    rect = NULL;
}

char* rectangleToString(void* data)
{
    Rectangle *tmpRect = data;
    char *rectStr = NULL;
    char convertx[100];
    char converty[100];
    char converth[100];
    char convertw[100];

    int length = 0;

    if (data == NULL)
        return NULL;

    gcvt(tmpRect->x, 6, convertx);
    length += strlen(convertx) + strlen("\nRectangle\nx = ");

    gcvt(tmpRect->y, 6, converty);
    length += strlen(converty) + strlen("\ny = ");

    gcvt(tmpRect->width, 6, convertw);
    length += strlen(convertw) + strlen("\nWidth = ");

    gcvt(tmpRect->height, 6, converth);
    length += strlen(converth) + strlen("\nheight = ");

    length += strlen("\nUnits = ") + strlen(tmpRect->units);

    char *listStr = toString(tmpRect->otherAttributes);
    length += strlen(listStr);

    rectStr = malloc(sizeof(char) * (length + 1));
    sprintf(rectStr, "\nRectangle\nx = %s\ny = %s\nWidth = %s\nheight = %s\nUnits = %s%s",
     convertx, converty, convertw, converth, tmpRect->units, listStr);

    free (listStr);

    return rectStr;
}

int compareRectangles(const void *first, const void *second)
{
    return 0;
}

void deleteCircle(void* data)
{
    Circle *circ = data;

    if (data == NULL)
        return;

    if (circ->otherAttributes != NULL)
        freeList(circ->otherAttributes);

    free(circ);
    circ = NULL;
}

char* circleToString(void* data)
{
    Circle *circle = data;
    char *circStr = NULL;
    char convertx[100];
    char converty[100];
    char convertr[100];

    int length = 0;

    if (data == NULL)
        return NULL;

    gcvt(circle->cx, 6, convertx);
    length += strlen(convertx) + strlen("\nCircle\nx = ");

    gcvt(circle->cy, 6, converty);
    length += strlen(converty) + strlen("\ny = ");

    gcvt(circle->r, 6, convertr);
    length += strlen(convertr) + strlen("\nr = ");

    length += strlen("\nUnits = ") + strlen(circle->units);

    char *listStr = toString(circle->otherAttributes);
    length += strlen(listStr);

    circStr = malloc(sizeof(char) * (length + 1));
    sprintf(circStr, "\nCircle\nx = %s\ny = %s\nr = %s\nUnits = %s%s",
     convertx, converty, convertr, circle->units, listStr);

    free (listStr);

    return circStr;
}

int compareCircles(const void *first, const void *second)
{
    return 0;
}

void deletePath(void* data)
{
    Path *path = data;

    if (data == NULL)
        return;

    if (path->data != NULL)
        free(path->data);

    if (path->otherAttributes != NULL)
        freeList(path->otherAttributes);

    free(path);
    path = NULL;
}

char* pathToString(void* data)
{
    Path *path = data;
    char *pathStr = NULL;
    char *listStr = NULL;
    char title[] = "\nPath\nData = ";
    int length = 0;

    if (data == NULL)
        return NULL;

    length += strlen(path->data) + strlen(title);

    if (path->otherAttributes != NULL)
    {
        listStr = toString(path->otherAttributes);
        length += strlen(listStr);
    }

    pathStr = malloc(sizeof(char) * (length + 1));
    strncpy(pathStr, title, 25);
    strncat(pathStr, path->data, strlen(path->data) + 1);

    if (listStr != NULL)
    {
        strncat(pathStr, listStr, strlen(listStr) + 1);
        free(listStr);
    }

    return pathStr;
}

int comparePaths(const void *first, const void *second)
{
    return 0;
}
